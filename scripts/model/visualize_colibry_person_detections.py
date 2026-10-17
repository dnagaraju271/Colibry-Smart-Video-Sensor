#!/usr/bin/env python3
"""Create a visual ground-truth/prediction montage for the Colibry person detector.

Example:
  python scripts/model/visualize_colibry_person_detections.py \
    --model runs/colibry_person_detector/colibry_person_detector.keras \
    --dataset /path/to/coco_person_hardneg/processed \
    --output-dir /tmp/colibry-person-diagnostics \
    --confidence 0.55

Ground-truth boxes are green, matched predictions blue, and false-positive
predictions red. The montage is for diagnosing labels/localization, not a
replacement for quantitative validation.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

import numpy as np
import tensorflow as tf
import tf_keras as keras
from PIL import Image, ImageDraw, ImageFont

IMAGE_WIDTH = 160
IMAGE_HEIGHT = 128
GRID_COLS = 40
GRID_ROWS = 32
VALUES_PER_CELL = 5
SCORE_FLOOR = 0.01
NMS_IOU = 0.50
MAX_NMS_OUTPUT = 100
SCALE = 4
GT_COLOR = (0, 220, 80)
TP_COLOR = (0, 185, 255)
FP_COLOR = (245, 45, 45)


def load_labels(path: Path) -> list[tuple[float, float, float, float]]:
    """Read normalized class-0 YOLO boxes and return xyxy coordinates."""
    text = path.read_text(encoding="utf-8").strip()
    boxes: list[tuple[float, float, float, float]] = []
    if not text:
        return boxes
    for line_number, line in enumerate(text.splitlines(), start=1):
        parts = line.split()
        if len(parts) != 5:
            raise ValueError(f"{path}:{line_number}: expected 5 YOLO label values")
        cls, cx, cy, width, height = map(float, parts)
        if cls != 0 or not np.isfinite([cx, cy, width, height]).all():
            raise ValueError(f"{path}:{line_number}: expected finite class-0 person box")
        if not (0 <= cx <= 1 and 0 <= cy <= 1 and 0 < width <= 1 and 0 < height <= 1):
            raise ValueError(f"{path}:{line_number}: box is outside normalized range")
        boxes.append((
            max(0.0, cx - width / 2),
            max(0.0, cy - height / 2),
            min(1.0, cx + width / 2),
            min(1.0, cy + height / 2),
        ))
    return boxes


def box_iou(a: tuple[float, ...] | np.ndarray, b: tuple[float, ...] | np.ndarray) -> float:
    x1, y1 = max(float(a[0]), float(b[0])), max(float(a[1]), float(b[1]))
    x2, y2 = min(float(a[2]), float(b[2])), min(float(a[3]), float(b[3]))
    intersection = max(0.0, x2 - x1) * max(0.0, y2 - y1)
    area_a = max(0.0, float(a[2]) - float(a[0])) * max(0.0, float(a[3]) - float(a[1]))
    area_b = max(0.0, float(b[2]) - float(b[0])) * max(0.0, float(b[3]) - float(b[1]))
    return intersection / (area_a + area_b - intersection + 1e-9)


def decode_predictions(raw: np.ndarray) -> list[tuple[np.ndarray, float]]:
    """Mirror the trainer's sigmoid box decoder and TensorFlow NMS behavior."""
    raw = np.asarray(raw, dtype=np.float32)
    if raw.shape != (GRID_ROWS, GRID_COLS, VALUES_PER_CELL):
        raise ValueError(f"Unexpected model output shape: {raw.shape}")
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
    valid = scores >= SCORE_FLOOR
    boxes, scores = boxes[valid], scores[valid]
    if not len(scores):
        return []
    selected = tf.image.non_max_suppression(
        boxes[:, [1, 0, 3, 2]],
        scores,
        max_output_size=MAX_NMS_OUTPUT,
        iou_threshold=NMS_IOU,
        score_threshold=SCORE_FLOOR,
    ).numpy()
    return [(boxes[int(index)], float(scores[int(index)])) for index in selected]


def match_for_display(
    predictions: list[tuple[np.ndarray, float]],
    ground_truth: list[tuple[float, float, float, float]],
    confidence: float,
) -> tuple[list[tuple[np.ndarray, float, bool]], dict[str, int]]:
    """Greedily match high-scoring predicted boxes to GT boxes at IoU >= 0.50."""
    chosen = [(box, score) for box, score in predictions if score >= confidence]
    matched_gt: set[int] = set()
    shown: list[tuple[np.ndarray, float, bool]] = []
    tp = fp = 0
    for box, score in chosen:
        best_index, best_iou = -1, 0.0
        for index, gt in enumerate(ground_truth):
            if index in matched_gt:
                continue
            overlap = box_iou(box, gt)
            if overlap > best_iou:
                best_index, best_iou = index, overlap
        is_tp = best_index >= 0 and best_iou >= 0.50
        if is_tp:
            tp += 1
            matched_gt.add(best_index)
        else:
            fp += 1
        shown.append((box, score, is_tp))
    return shown, {"gt": len(ground_truth), "pred": len(chosen), "tp": tp, "fp": fp, "fn": len(ground_truth) - tp}


def _rect(box: tuple[float, ...] | np.ndarray) -> tuple[int, int, int, int]:
    return tuple(int(round(float(v) * (IMAGE_WIDTH if i % 2 == 0 else IMAGE_HEIGHT) * SCALE)) for i, v in enumerate(box))  # type: ignore[return-value]


def render_panel(
    pixels: np.ndarray,
    image_id: str,
    ground_truth: list[tuple[float, float, float, float]],
    predictions: list[tuple[np.ndarray, float, bool]],
    stats: dict[str, int],
    confidence: float,
) -> Image.Image:
    gray = Image.fromarray(pixels.astype(np.uint8), mode="L")
    canvas = gray.convert("RGB").resize((IMAGE_WIDTH * SCALE, IMAGE_HEIGHT * SCALE), Image.Resampling.NEAREST)
    draw = ImageDraw.Draw(canvas)
    width = max(2, SCALE)
    for gt in ground_truth:
        draw.rectangle(_rect(gt), outline=GT_COLOR, width=width)
    for box, score, is_tp in predictions:
        coords = _rect(box)
        color = TP_COLOR if is_tp else FP_COLOR
        draw.rectangle(coords, outline=color, width=width)
        label = f"{score:.2f}"
        left, top = coords[0], max(0, coords[1] - 14)
        draw.rectangle((left, top, left + 29, top + 13), fill=color)
        draw.text((left + 1, top), label, fill=(0, 0, 0))
    caption = (
        f"{image_id} | GT {stats['gt']} | Pred {stats['pred']} | "
        f"TP {stats['tp']} FP {stats['fp']} FN {stats['fn']} | conf {confidence:.3f}"
    )
    panel = Image.new("RGB", (canvas.width, canvas.height + 38), "white")
    panel.paste(canvas, (0, 38))
    capdraw = ImageDraw.Draw(panel)
    capdraw.text((5, 8), caption, fill="black", font=ImageFont.load_default())
    return panel


def select_ids(records: list[dict[str, Any]], positive_count: int, empty_count: int) -> list[str]:
    positives = [r for r in records if r["gt_count"] > 0]
    empty = [r for r in records if r["gt_count"] == 0]
    # Include positive frames with many false detections and many missed boxes.
    chosen_pos: list[dict[str, Any]] = []
    for key in ("fp", "fn", "pred"):
        for record in sorted(positives, key=lambda item: item[key], reverse=True):
            if record not in chosen_pos:
                chosen_pos.append(record)
                if len(chosen_pos) >= positive_count:
                    break
        if len(chosen_pos) >= positive_count:
            break
    chosen_empty = sorted(empty, key=lambda item: item["pred"], reverse=True)[:empty_count]
    return [r["image_id"] for r in chosen_pos[:positive_count] + chosen_empty]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--model", type=Path, required=True, help="Saved .keras model file for the selected architecture.")
    parser.add_argument("--dataset", type=Path, required=True, help="Processed dataset with images/val and labels/val.")
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--confidence", type=float, default=0.55)
    parser.add_argument("--batch-size", type=int, default=16)
    parser.add_argument("--positive-count", type=int, default=6)
    parser.add_argument("--empty-count", type=int, default=4)
    parser.add_argument("--max-draw-predictions", type=int, default=15)
    args = parser.parse_args()
    if not 0 <= args.confidence <= 1 or args.batch_size < 1:
        parser.error("confidence must be in [0, 1] and batch-size must be positive")
    if args.positive_count < 0 or args.empty_count < 0 or args.max_draw_predictions < 1:
        parser.error("panel counts must be non-negative and max-draw-predictions must be positive")
    args.model = args.model.expanduser().resolve()
    args.dataset = args.dataset.expanduser().resolve()
    args.output_dir = args.output_dir.expanduser().resolve()
    if not args.model.is_file():
        parser.error(f"model file not found: {args.model}")

    image_dir = args.dataset / "images" / "val"
    label_dir = args.dataset / "labels" / "val"
    image_map = {
        path.stem: path
        for pattern in ("*.png", "*.jpg", "*.jpeg")
        for path in image_dir.glob(pattern)
    }
    label_map = {path.stem: path for path in label_dir.glob("*.txt")}
    if not image_map or set(image_map) != set(label_map):
        raise RuntimeError(
            f"Validation image/label IDs differ or are missing. "
            f"images={len(image_map)} labels={len(label_map)}"
        )

    images: list[np.ndarray] = []
    gt_by_id: dict[str, list[tuple[float, float, float, float]]] = {}
    image_ids = sorted(image_map)
    for image_id in image_ids:
        with Image.open(image_map[image_id]) as image:
            pixels = np.asarray(image.convert("L"), dtype=np.uint8)
        if pixels.shape != (IMAGE_HEIGHT, IMAGE_WIDTH):
            raise ValueError(f"{image_id}: expected (height,width)=({IMAGE_HEIGHT},{IMAGE_WIDTH}), got {pixels.shape}")
        images.append(pixels[..., None].astype(np.float32) / 255.0)
        gt_by_id[image_id] = load_labels(label_map[image_id])

    model = keras.models.load_model(str(args.model), compile=False)
    if tuple(model.input_shape[1:]) != (IMAGE_HEIGHT, IMAGE_WIDTH, 1):
        raise ValueError(f"Unexpected model input shape: {model.input_shape}")
    expected_output = (None, GRID_ROWS, GRID_COLS, VALUES_PER_CELL)
    if tuple(model.output_shape) != expected_output:
        raise ValueError(f"Unexpected model output shape: {model.output_shape}")

    x = np.stack(images).astype(np.float32)
    raw = model.predict(x, batch_size=args.batch_size, verbose=1)
    if raw.shape != (len(image_ids), GRID_ROWS, GRID_COLS, VALUES_PER_CELL):
        raise ValueError(f"Unexpected prediction tensor shape: {raw.shape}")

    records: list[dict[str, Any]] = []
    decoded_by_id: dict[str, list[tuple[np.ndarray, float]]] = {}
    for image_id, raw_item in zip(image_ids, raw):
        predictions = decode_predictions(raw_item)
        decoded_by_id[image_id] = predictions
        shown, stats = match_for_display(predictions, gt_by_id[image_id], args.confidence)
        records.append({
            "image_id": image_id,
            "gt_count": stats["gt"],
            "pred": stats["pred"],
            "tp": stats["tp"],
            "fp": stats["fp"],
            "fn": stats["fn"],
            "negative_image": stats["gt"] == 0,
            "negative_image_has_detection": stats["gt"] == 0 and stats["pred"] > 0,
        })

    selected_ids = select_ids(records, args.positive_count, args.empty_count)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    panels: list[Image.Image] = []
    by_id = {r["image_id"]: r for r in records}
    for image_id in selected_ids:
        rec = by_id[image_id]
        all_shown, stats = match_for_display(decoded_by_id[image_id], gt_by_id[image_id], args.confidence)
        # Keep the highest scoring boxes visually legible; summary counts above use all boxes at threshold.
        all_shown = all_shown[:args.max_draw_predictions]
        panel = render_panel(
            (x[image_ids.index(image_id), ..., 0] * 255.0).round().astype(np.uint8),
            image_id, gt_by_id[image_id], all_shown, stats, args.confidence
        )
        panel_path = args.output_dir / f"{image_id}_diagnostic.png"
        panel.save(panel_path)
        panels.append(panel)

    if panels:
        columns = 2
        rows = (len(panels) + columns - 1) // columns
        cell_width = max(p.width for p in panels)
        cell_height = max(p.height for p in panels)
        montage = Image.new("RGB", (columns * cell_width, rows * cell_height), (235, 235, 235))
        for index, panel in enumerate(panels):
            montage.paste(panel, ((index % columns) * cell_width, (index // columns) * cell_height))
        montage_path = args.output_dir / "diagnostic_montage.png"
        montage.save(montage_path)
        print("Saved diagnostic montage:", montage_path)
    report = {
        "model": str(args.model),
        "dataset": str(args.dataset),
        "confidence": args.confidence,
        "validation_images": len(image_ids),
        "positive_images": sum(r["gt_count"] > 0 for r in records),
        "empty_images": sum(r["gt_count"] == 0 for r in records),
        "negative_images_with_detections": sum(r["negative_image_has_detection"] for r in records),
        "selected_image_ids": selected_ids,
        "color_legend": {"ground_truth": "green", "matched_prediction": "blue/cyan", "false_positive_prediction": "red"},
        "note": "Diagnostic panels draw only the highest-scoring capped predictions, but summary counts include all detections at the selected confidence.",
        "per_image": records,
    }
    report_path = args.output_dir / "diagnostic_report.json"
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print("Saved diagnostic report:", report_path)
    print(json.dumps({
        "validation_images": report["validation_images"],
        "negative_images_with_detections": report["negative_images_with_detections"],
        "selected_image_ids": selected_ids,
        "montage": str(args.output_dir / "diagnostic_montage.png") if panels else None,
    }, indent=2))


if __name__ == "__main__":
    main()
