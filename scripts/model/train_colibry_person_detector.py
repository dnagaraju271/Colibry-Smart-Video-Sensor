#!/usr/bin/env python3
"""Train a compact grayscale person detector for the ColibryNPU target.

Input convention: 160 pixels wide x 128 pixels high, one grayscale channel.
This is a hardware-oriented grid detector, not the original YOLOv8 graph.
Optional YOLO teacher boxes can supplement ground truth on positive images only.
"""
from __future__ import annotations

import argparse
import json
from collections import Counter
from pathlib import Path
from typing import Any

import numpy as np
import tensorflow as tf
import tf_keras as keras
from PIL import Image

ROOT = Path(__file__).resolve().parents[2]
IMAGE_WIDTH = 160
IMAGE_HEIGHT = 128
INPUT_CHANNELS = 1
GRID_COLS = 40
GRID_ROWS = 32
VALUES_PER_CELL = 5  # objectness, x offset, y offset, width, height
MIN_PERSON_HEIGHT_PX = 24
NEGATIVE_LOSS_WEIGHT = 1.0
GIOU_WEIGHT = 1.0


def build_model() -> keras.Model:
    """Build a small residual grid detector using simple Conv/ReLU/Add ops."""
    inputs = keras.Input(
        shape=(IMAGE_HEIGHT, IMAGE_WIDTH, INPUT_CHANNELS),
        name="grayscale_input",
    )
    x = keras.layers.Conv2D(
        8, 3, strides=2, padding="same", activation="relu",
        kernel_initializer="he_normal", bias_initializer="zeros",
        name="stem_conv",
    )(inputs)
    x = keras.layers.DepthwiseConv2D(
        3, strides=2, padding="same", activation="relu",
        depthwise_initializer="he_normal", bias_initializer="zeros",
        name="downsample_depthwise",
    )(x)
    x = keras.layers.Conv2D(
        24, 1, padding="same", activation="relu",
        kernel_initializer="he_normal", bias_initializer="zeros",
        name="pointwise_1",
    )(x)

    # Identity shortcuts help preserve image-dependent features in deeper layers.
    for block in range(1, 7):
        shortcut = x
        y = keras.layers.DepthwiseConv2D(
            3, padding="same", activation="relu",
            depthwise_initializer="he_normal", bias_initializer="zeros",
            name=f"depthwise_{block}",
        )(x)
        y = keras.layers.Conv2D(
            24, 1, padding="same", activation="linear",
            kernel_initializer=keras.initializers.RandomNormal(stddev=0.02),
            bias_initializer="zeros",
            name=f"pointwise_{block + 1}_residual",
        )(y)
        x = keras.layers.Add(name=f"residual_add_{block}")([shortcut, y])
        x = keras.layers.ReLU(name=f"residual_relu_{block}")(x)

    outputs = keras.layers.Conv2D(
        VALUES_PER_CELL, 1, padding="same", activation="linear",
        kernel_initializer=keras.initializers.RandomNormal(stddev=0.01),
        bias_initializer="zeros", name="grid_predictions",
    )(x)

    model = keras.Model(inputs, outputs, name="colibry_residual_person_detector")
    kernel, bias = model.get_layer("grid_predictions").get_weights()
    bias[0] = -2.0
    model.get_layer("grid_predictions").set_weights([kernel, bias])
    return model


def hard_negative_giou_detection_loss(y_true: tf.Tensor, y_pred: tf.Tensor) -> tf.Tensor:
    """Balance positive images against hard negatives instead of rewarding constant low scores.

    Positive objectness losses are averaged over images that contain positives.
    Negative losses are averaged over images, using the highest-loss background cells.
    The independent averages avoid the earlier loss's positive/negative count imbalance.
    """
    obj = y_true[..., 0]
    logits = y_pred[..., 0]
    bce = tf.nn.sigmoid_cross_entropy_with_logits(labels=obj, logits=logits)

    pos_count = tf.reduce_sum(obj, axis=[1, 2])
    positive_image_mask = tf.cast(pos_count > 0.0, tf.float32)
    pos_loss_per_image = (
        tf.reduce_sum(bce * obj, axis=[1, 2]) /
        tf.maximum(pos_count, 1.0)
    )
    pos_loss = (
        tf.reduce_sum(pos_loss_per_image * positive_image_mask) /
        tf.maximum(tf.reduce_sum(positive_image_mask), 1.0)
    )

    def hard_negatives_for_image(inputs: tuple[tf.Tensor, tf.Tensor]) -> tf.Tensor:
        image_bce, image_obj = inputs
        losses = tf.boolean_mask(
            tf.reshape(image_bce, [-1]),
            tf.reshape(image_obj, [-1]) < 0.5,
        )
        n = tf.size(losses)

        def calculate() -> tf.Tensor:
            image_positive_count = tf.reduce_sum(image_obj)
            k = tf.maximum(tf.cast(image_positive_count * 3.0, tf.int32), 32)
            k = tf.minimum(tf.minimum(k, 128), n)
            return tf.reduce_mean(tf.math.top_k(losses, k=k, sorted=False).values)

        return tf.cond(n > 0, calculate, lambda: tf.constant(0.0, tf.float32))

    negative_loss_per_image = tf.map_fn(
        hard_negatives_for_image,
        (bce, obj),
        fn_output_signature=tf.float32,
    )
    negative_loss = tf.reduce_mean(negative_loss_per_image)
    objectness_loss = pos_loss + NEGATIVE_LOSS_WEIGHT * negative_loss

    true_v = y_true[..., 1:5]
    pred_v = tf.sigmoid(y_pred[..., 1:5])

    rows = tf.cast(tf.range(GRID_ROWS)[None, :, None], tf.float32)
    cols = tf.cast(tf.range(GRID_COLS)[None, None, :], tf.float32)

    tx = (cols + true_v[..., 0]) / GRID_COLS
    ty = (rows + true_v[..., 1]) / GRID_ROWS
    tw, th = true_v[..., 2], true_v[..., 3]
    px = (cols + pred_v[..., 0]) / GRID_COLS
    py = (rows + pred_v[..., 1]) / GRID_ROWS
    pw, ph = pred_v[..., 2], pred_v[..., 3]

    tx1 = tf.clip_by_value(tx - tw / 2, 0.0, 1.0)
    ty1 = tf.clip_by_value(ty - th / 2, 0.0, 1.0)
    tx2 = tf.clip_by_value(tx + tw / 2, 0.0, 1.0)
    ty2 = tf.clip_by_value(ty + th / 2, 0.0, 1.0)
    px1 = tf.clip_by_value(px - pw / 2, 0.0, 1.0)
    py1 = tf.clip_by_value(py - ph / 2, 0.0, 1.0)
    px2 = tf.clip_by_value(px + pw / 2, 0.0, 1.0)
    py2 = tf.clip_by_value(py + ph / 2, 0.0, 1.0)

    intersection = (
        tf.maximum(tf.minimum(tx2, px2) - tf.maximum(tx1, px1), 0.0)
        * tf.maximum(tf.minimum(ty2, py2) - tf.maximum(ty1, py1), 0.0)
    )
    area_true = (tx2 - tx1) * (ty2 - ty1)
    area_pred = (px2 - px1) * (py2 - py1)
    union = area_true + area_pred - intersection
    iou = intersection / (union + 1e-7)
    enclosing_area = (
        (tf.maximum(tx2, px2) - tf.minimum(tx1, px1))
        * (tf.maximum(ty2, py2) - tf.minimum(ty1, py1))
    )
    giou = iou - (enclosing_area - union) / (enclosing_area + 1e-7)

    positive_count = tf.maximum(tf.reduce_sum(obj), 1.0)
    giou_loss = tf.reduce_sum((1.0 - giou) * obj) / positive_count

    error = tf.abs(pred_v - true_v)
    delta = 0.1
    huber = tf.where(
        error <= delta,
        0.5 * tf.square(error),
        delta * (error - 0.5 * delta),
    )
    coordinate_count = tf.maximum(tf.reduce_sum(obj) * 4.0, 1.0)
    coordinate_loss = tf.reduce_sum(huber * obj[..., None]) / coordinate_count

    return objectness_loss + GIOU_WEIGHT * giou_loss + coordinate_loss


def read_ground_truth(label_path: Path) -> tuple[list[tuple[float, ...]], list[tuple[float, ...]]]:
    """Read normalized class-0 YOLO labels and return cxcywh and normalized xyxy."""
    cxcywh: list[tuple[float, ...]] = []
    xyxy: list[tuple[float, ...]] = []
    text = label_path.read_text(encoding="utf-8").strip()
    if not text:
        return cxcywh, xyxy

    for line_number, line in enumerate(text.splitlines(), start=1):
        values = line.split()
        if len(values) != 5:
            raise ValueError(f"{label_path}:{line_number}: expected 5 values")
        cls, cx, cy, width, height = map(float, values)
        if cls != 0 or not np.isfinite([cx, cy, width, height]).all():
            raise ValueError(f"{label_path}:{line_number}: invalid person label")
        if not (
            0.0 <= cx <= 1.0 and 0.0 <= cy <= 1.0
            and 0.0 < width <= 1.0 and 0.0 < height <= 1.0
        ):
            raise ValueError(f"{label_path}:{line_number}: box outside normalized range")
        cxcywh.append((cx, cy, width, height))
        xyxy.append((
            max(0.0, cx - width / 2), max(0.0, cy - height / 2),
            min(1.0, cx + width / 2), min(1.0, cy + height / 2),
        ))
    return cxcywh, xyxy


def encode_targets(
    label_path: Path,
    teacher_record: dict[str, Any] | None = None,
    pseudo_confidence: float = 0.35,
    pseudo_exclude_iou: float = 0.30,
) -> tuple[np.ndarray, list[tuple[float, ...]], Counter]:
    """Encode ground truth and optionally add YOLO boxes only to known-positive images."""
    boxes, gt_xyxy = read_ground_truth(label_path)
    target = np.zeros((GRID_ROWS, GRID_COLS, VALUES_PER_CELL), dtype=np.float32)
    counts: Counter = Counter()
    counts["gt_boxes"] = len(boxes)
    eligible: dict[tuple[int, int], list[tuple[float, float, float, float, float]]] = {}

    for cx, cy, width, height in boxes:
        if height * IMAGE_HEIGHT < MIN_PERSON_HEIGHT_PX:
            counts["small_gt_ignored"] += 1
            continue
        col = min(int(cx * GRID_COLS), GRID_COLS - 1)
        row = min(int(cy * GRID_ROWS), GRID_ROWS - 1)
        eligible.setdefault((row, col), []).append(
            (width * height, cx, cy, width, height)
        )

    for (row, col), cell_boxes in eligible.items():
        cell_boxes.sort(key=lambda item: (-item[0], item[2], item[1]))
        _, cx, cy, width, height = cell_boxes[0]
        target[row, col] = (
            1.0, cx * GRID_COLS - col, cy * GRID_ROWS - row, width, height
        )
        counts["gt_boxes_retained"] += 1
        counts["gt_collision_drops"] += len(cell_boxes) - 1

    # A known-empty image remains empty: never trust a teacher to override its label.
    if teacher_record is None or not boxes:
        counts["pseudo_boxes_added"] += 0
        return target, gt_xyxy, counts

    candidates = sorted(
        zip(teacher_record.get("teacher_xyxy", []), teacher_record.get("teacher_scores", [])),
        key=lambda item: float(item[1]),
        reverse=True,
    )
    for box, score_value in candidates:
        score = float(score_value)
        if score < pseudo_confidence:
            break
        arr = np.asarray(box, dtype=np.float32)
        if arr.shape != (4,) or not np.isfinite(arr).all():
            counts["pseudo_invalid"] += 1
            continue
        x1, y1, x2, y2 = [float(v) for v in arr]
        x1, y1, x2, y2 = (
            max(0.0, x1), max(0.0, y1),
            min(1.0, x2), min(1.0, y2),
        )
        width, height = x2 - x1, y2 - y1
        if width <= 0 or height * IMAGE_HEIGHT < MIN_PERSON_HEIGHT_PX:
            continue

        if any(_box_iou((x1, y1, x2, y2), gt) >= pseudo_exclude_iou for gt in gt_xyxy):
            counts["pseudo_overlaps_gt"] += 1
            continue

        cx, cy = (x1 + x2) / 2, (y1 + y2) / 2
        col = min(int(cx * GRID_COLS), GRID_COLS - 1)
        row = min(int(cy * GRID_ROWS), GRID_ROWS - 1)
        if target[row, col, 0] >= 0.5:
            counts["pseudo_cell_conflicts"] += 1
            continue

        target[row, col] = (
            1.0, cx * GRID_COLS - col, cy * GRID_ROWS - row, width, height
        )
        counts["pseudo_boxes_added"] += 1
        if counts["pseudo_boxes_added"] >= 20:
            break

    return target, gt_xyxy, counts


def _box_iou(a: tuple[float, ...], b: tuple[float, ...]) -> float:
    x1, y1 = max(a[0], b[0]), max(a[1], b[1])
    x2, y2 = min(a[2], b[2]), min(a[3], b[3])
    intersection = max(0.0, x2 - x1) * max(0.0, y2 - y1)
    area_a = max(0.0, a[2] - a[0]) * max(0.0, a[3] - a[1])
    area_b = max(0.0, b[2] - b[0]) * max(0.0, b[3] - b[1])
    return intersection / (area_a + area_b - intersection + 1e-9)


def _load_cache(path: Path | None) -> dict[str, dict[str, Any]]:
    if path is None:
        return {}
    records = json.loads(path.read_text(encoding="utf-8"))
    cache: dict[str, dict[str, Any]] = {}
    for record in records:
        image_id = Path(str(record["image"])).stem
        if image_id in cache:
            raise ValueError(f"Duplicate teacher cache image ID: {image_id}")
        if len(record.get("teacher_xyxy", [])) != len(record.get("teacher_scores", [])):
            raise ValueError(f"Teacher box/score count mismatch for {image_id}")
        cache[image_id] = record
    return cache


def load_split(
    dataset: Path,
    split: str,
    teacher_cache: dict[str, dict[str, Any]] | None = None,
    add_pseudo_labels: bool = False,
    pseudo_confidence: float = 0.35,
) -> tuple[np.ndarray, np.ndarray, list[np.ndarray], dict[str, Any]]:
    image_dir = dataset / "images" / split
    label_dir = dataset / "labels" / split
    image_map = {
        path.stem: path
        for pattern in ("*.png", "*.jpg", "*.jpeg")
        for path in image_dir.glob(pattern)
    }
    label_map = {path.stem: path for path in label_dir.glob("*.txt")}
    if not image_map:
        raise RuntimeError(f"No images found in {image_dir}")
    if set(image_map) != set(label_map):
        raise RuntimeError(
            f"{split} image/label IDs differ: "
            f"missing labels={sorted(set(image_map)-set(label_map))[:5]}, "
            f"missing images={sorted(set(label_map)-set(image_map))[:5]}"
        )
    cache = teacher_cache or {}
    if add_pseudo_labels and cache and set(image_map) != set(cache):
        raise RuntimeError(
            "Training image IDs do not match teacher cache IDs. "
            f"Missing cache={sorted(set(image_map)-set(cache))[:5]}, "
            f"extra cache={sorted(set(cache)-set(image_map))[:5]}"
        )

    images: list[np.ndarray] = []
    targets: list[np.ndarray] = []
    gt_by_image: list[np.ndarray] = []
    totals: Counter = Counter()
    for image_id in sorted(image_map):
        with Image.open(image_map[image_id]) as image:
            pixels = np.asarray(image.convert("L"), dtype=np.uint8)
        if pixels.shape != (IMAGE_HEIGHT, IMAGE_WIDTH):
            raise ValueError(
                f"{image_id}: expected height x width "
                f"{IMAGE_HEIGHT} x {IMAGE_WIDTH}, got {pixels.shape}"
            )

        gt_boxes, gt_xyxy = read_ground_truth(label_map[image_id])
        teacher = cache.get(image_id) if add_pseudo_labels else None
        target, _xyxy, stats = encode_targets(
            label_map[image_id],
            teacher,
            pseudo_confidence=pseudo_confidence,
        )
        images.append(pixels[..., None].astype(np.float32) / 255.0)
        targets.append(target)
        gt_by_image.append(np.asarray(gt_xyxy, dtype=np.float32).reshape(-1, 4))
        totals.update(stats)
        totals["empty_images"] += int(len(gt_boxes) == 0)

    x = np.stack(images).astype(np.float32)
    y = np.stack(targets).astype(np.float32)
    summary = {
        "split": split,
        "images": len(x),
        "input_shape": list(x.shape),
        "target_shape": list(y.shape),
        "ground_truth_boxes": totals["gt_boxes"],
        "retained_ground_truth_boxes": totals["gt_boxes_retained"],
        "small_boxes_ignored": totals["small_gt_ignored"],
        "ground_truth_collision_drops": totals["gt_collision_drops"],
        "pseudo_boxes_added": totals["pseudo_boxes_added"],
        "pseudo_boxes_overlapping_ground_truth": totals["pseudo_overlaps_gt"],
        "known_empty_images": totals["empty_images"],
        "pseudo_labels_in_known_empty_images": 0,
    }
    return x, y, gt_by_image, summary


def decode_predictions(raw: np.ndarray, score_floor: float = 0.01) -> list[tuple[np.ndarray, float]]:
    rows, cols = np.meshgrid(
        np.arange(GRID_ROWS, dtype=np.float32),
        np.arange(GRID_COLS, dtype=np.float32),
        indexing="ij",
    )
    values = 1.0 / (1.0 + np.exp(-np.clip(raw[..., 1:5], -30, 30)))
    scores = 1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30, 30)))
    cx = (cols + values[..., 0]) / GRID_COLS
    cy = (rows + values[..., 1]) / GRID_ROWS
    width, height = values[..., 2], values[..., 3]
    boxes = np.stack([
        np.clip(cx - width / 2, 0, 1),
        np.clip(cy - height / 2, 0, 1),
        np.clip(cx + width / 2, 0, 1),
        np.clip(cy + height / 2, 0, 1),
    ], axis=-1).reshape(-1, 4)
    scores = scores.reshape(-1)
    valid = scores >= score_floor
    boxes, scores = boxes[valid], scores[valid]
    if not len(scores):
        return []
    selected = tf.image.non_max_suppression(
        boxes[:, [1, 0, 3, 2]], scores, max_output_size=100,
        iou_threshold=0.50, score_threshold=score_floor,
    ).numpy()
    return [(boxes[index], float(scores[index])) for index in selected]


def evaluate_predictions(
    predictions: list[list[tuple[np.ndarray, float]]],
    ground_truth: list[np.ndarray],
    confidence_threshold: float,
) -> dict[str, Any]:
    tp = fp = fn = 0
    neg_images = neg_images_with_detections = 0
    for detections, gt in zip(predictions, ground_truth):
        gt = np.asarray(gt).reshape(-1, 4)
        chosen = [(box, score) for box, score in detections if score >= confidence_threshold]
        if len(gt) == 0:
            neg_images += 1
            neg_images_with_detections += int(bool(chosen))
        matched: set[int] = set()
        for pred_box, _score in chosen:
            best_idx, best_iou = -1, 0.0
            for index, gt_box in enumerate(gt):
                if index in matched:
                    continue
                overlap = _box_iou(tuple(pred_box), tuple(gt_box))
                if overlap > best_iou:
                    best_idx, best_iou = index, overlap
            if best_idx >= 0 and best_iou >= 0.50:
                tp += 1
                matched.add(best_idx)
            else:
                fp += 1
        fn += len(gt) - len(matched)
    precision = tp / max(tp + fp, 1)
    recall = tp / max(tp + fn, 1)
    return {
        "confidence_threshold": confidence_threshold,
        "box_match_iou_threshold": 0.50,
        "true_positives": tp,
        "false_positives": fp,
        "false_negatives": fn,
        "precision": precision,
        "recall": recall,
        "f1": 2 * precision * recall / max(precision + recall, 1e-12),
        "negative_images": neg_images,
        "negative_images_with_detections": neg_images_with_detections,
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--dataset", type=Path, required=True,
        help="Processed dataset directory containing images/{train,val} and labels/{train,val}.",
    )
    parser.add_argument("--teacher-train-cache", type=Path, default=None)
    parser.add_argument("--output-dir", type=Path, default=ROOT / "runs/colibry_person_detector")
    parser.add_argument("--epochs", type=int, default=40)
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--learning-rate", type=float, default=1e-3)
    parser.add_argument("--min-person-height-px", type=int, default=24)
    parser.add_argument("--pseudo-confidence", type=float, default=0.35)
    parser.add_argument("--check-only", action="store_true")
    args = parser.parse_args()

    if args.epochs < 1 or args.batch_size < 1 or args.learning_rate <= 0:
        parser.error("epochs and batch-size must be positive, as must learning-rate")
    if not 0.0 <= args.pseudo_confidence <= 1.0:
        parser.error("pseudo-confidence must be between 0 and 1")

    global MIN_PERSON_HEIGHT_PX
    MIN_PERSON_HEIGHT_PX = args.min_person_height_px
    args.dataset = args.dataset.expanduser().resolve()
    args.output_dir = args.output_dir.expanduser().resolve()
    args.output_dir.mkdir(parents=True, exist_ok=True)

    teacher_train = _load_cache(args.teacher_train_cache.expanduser()) if args.teacher_train_cache else {}
    if args.teacher_train_cache and args.pseudo_confidence != 0.35:
        print(f"Using YOLO teacher confidence threshold: {args.pseudo_confidence:.2f}")

    x_train, y_train, _train_gt, train_summary = load_split(
        args.dataset, "train", teacher_train,
        add_pseudo_labels=bool(teacher_train),
        pseudo_confidence=args.pseudo_confidence,
    )
    x_val, y_val, val_gt, val_summary = load_split(args.dataset, "val")

    expected_x = (len(x_train), IMAGE_HEIGHT, IMAGE_WIDTH, INPUT_CHANNELS)
    expected_y = (len(x_train), GRID_ROWS, GRID_COLS, VALUES_PER_CELL)
    if x_train.shape != expected_x or y_train.shape != expected_y:
        raise ValueError(f"Training shapes don't match expected: {x_train.shape}, {y_train.shape}")
    if x_val.shape[1:] != (IMAGE_HEIGHT, IMAGE_WIDTH, INPUT_CHANNELS):
        raise ValueError(f"Invalid validation image shape: {x_val.shape}")

    model = build_model()
    expected_output = (None, GRID_ROWS, GRID_COLS, VALUES_PER_CELL)
    if model.output_shape != expected_output:
        raise ValueError(f"Model output {model.output_shape} != {expected_output}")

    report: dict[str, Any] = {
        "input_contract": {
            "width": IMAGE_WIDTH,
            "height": IMAGE_HEIGHT,
            "channels": INPUT_CHANNELS,
            "keras_shape": [None, IMAGE_HEIGHT, IMAGE_WIDTH, INPUT_CHANNELS],
            "pixel_values": "grayscale uint8 converted to float32 in [0, 1]",
        },
        "target_contract": {
            "grid_width": GRID_COLS,
            "grid_height": GRID_ROWS,
            "values_per_cell": VALUES_PER_CELL,
        },
        "architecture": {
            "model_name": model.name,
            "stem_channels": 8,
            "feature_channels": 24,
            "residual_blocks": 6,
            "downsampling_factor": 4,
            "output_activation": "linear",
            "prediction_channels": VALUES_PER_CELL,
            "parameter_count": model.count_params(),
        },
        "training_config": {
            "epochs_requested": args.epochs,
            "batch_size": args.batch_size,
            "learning_rate_initial": args.learning_rate,
            "min_person_height_px": MIN_PERSON_HEIGHT_PX,
            "teacher_cache_enabled": bool(teacher_train),
            "pseudo_confidence": args.pseudo_confidence,
            "random_seed": 42,
        },
        "model_parameters": model.count_params(),
        "loss": {
            "name": "hard_negative_giou_detection_loss",
            "negative_loss_weight": NEGATIVE_LOSS_WEIGHT,
            "design": "positive-image and hard-negative image losses are averaged separately",
        },
        "training": train_summary,
        "validation": val_summary,
    }

    print(json.dumps(report, indent=2))
    if args.check_only:
        print("CHECK PASSED: shapes, labels, optional teacher cache, and model contract validated.")
        return

    np.random.seed(42)
    tf.random.set_seed(42)
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=args.learning_rate),
        loss=hard_negative_giou_detection_loss,
    )
    best_weights = args.output_dir / "best.weights.h5"
    callbacks = [
        keras.callbacks.ModelCheckpoint(
            str(best_weights), monitor="val_loss",
            save_best_only=True, save_weights_only=True, verbose=1,
        ),
        keras.callbacks.EarlyStopping(
            monitor="val_loss", patience=8, restore_best_weights=True, verbose=1,
        ),
        keras.callbacks.ReduceLROnPlateau(
            monitor="val_loss", factor=0.5, patience=3, min_lr=1e-6, verbose=1,
        ),
        keras.callbacks.CSVLogger(str(args.output_dir / "training_history.csv")),
    ]
    model.fit(
        x_train, y_train,
        validation_data=(x_val, y_val),
        epochs=args.epochs,
        batch_size=args.batch_size,
        shuffle=True,
        callbacks=callbacks,
        verbose=2,
    )
    if best_weights.exists():
        model.load_weights(str(best_weights))
    model.save_weights(str(args.output_dir / "final.weights.h5"))
    model.save(str(args.output_dir / "colibry_person_detector.keras"), include_optimizer=False)

    raw = model.predict(x_val, batch_size=args.batch_size, verbose=1)
    decoded = [decode_predictions(item) for item in raw]
    report["validation_metrics"] = {
        "metric_note": (
            "Greedy one-to-one matching at IoU >= 0.50 after NMS IoU 0.50. "
            "Validation labels do not use teacher pseudo-labels. These are not COCO mAP."
        ),
        "at_confidence_0.25": evaluate_predictions(decoded, val_gt, 0.25),
        "at_confidence_0.35": evaluate_predictions(decoded, val_gt, 0.35),
        "at_confidence_0.50": evaluate_predictions(decoded, val_gt, 0.50),
    }
    report_path = args.output_dir / "training_report.json"
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print("\n=== VALIDATION METRICS ===")
    print(json.dumps(report["validation_metrics"], indent=2))
    print("Saved model:", args.output_dir / "colibry_person_detector.keras")
    print("Saved report:", report_path)
    print("A successful training run does not prove compiler compatibility or physical-board inference.")


if __name__ == "__main__":
    main()
