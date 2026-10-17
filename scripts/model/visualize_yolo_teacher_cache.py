#!/usr/bin/env python3
"""Visualize cached YOLO-teacher boxes against validation ground truth.

This diagnostic needs no TensorFlow or PyTorch: it uses the cached normalized
teacher_xyxy boxes/scores produced by the reference detector.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

import numpy as np
from PIL import Image, ImageDraw, ImageFont

IMAGE_WIDTH = 160
IMAGE_HEIGHT = 128
SCALE = 4
GT_COLOR = (0, 220, 80)
TP_COLOR = (0, 185, 255)
FP_COLOR = (245, 45, 45)


def load_labels(path: Path) -> list[tuple[float, float, float, float]]:
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
            max(0.0, cx - width / 2), max(0.0, cy - height / 2),
            min(1.0, cx + width / 2), min(1.0, cy + height / 2),
        ))
    return boxes


def box_iou(a: tuple[float, ...] | np.ndarray, b: tuple[float, ...] | np.ndarray) -> float:
    x1, y1 = max(float(a[0]), float(b[0])), max(float(a[1]), float(b[1]))
    x2, y2 = min(float(a[2]), float(b[2])), min(float(a[3]), float(b[3]))
    intersection = max(0.0, x2 - x1) * max(0.0, y2 - y1)
    area_a = max(0.0, float(a[2]) - float(a[0])) * max(0.0, float(a[3]) - float(a[1]))
    area_b = max(0.0, float(b[2]) - float(b[0])) * max(0.0, float(b[3]) - float(b[1]))
    return intersection / (area_a + area_b - intersection + 1e-9)


def parse_teacher_record(record: dict[str, Any], image_id: str) -> list[tuple[np.ndarray, float]]:
    boxes = record.get("teacher_xyxy", [])
    scores = record.get("teacher_scores", [])
    if len(boxes) != len(scores):
        raise ValueError(f"{image_id}: teacher box/score counts differ")
    result: list[tuple[np.ndarray, float]] = []
    for index, (box, score_value) in enumerate(zip(boxes, scores)):
        arr = np.asarray(box, dtype=np.float32)
        score = float(score_value)
        if arr.shape != (4,) or not np.isfinite(arr).all() or not np.isfinite(score):
            raise ValueError(f"{image_id}: invalid teacher prediction at index {index}")
        if not (0 <= arr[0] < arr[2] <= 1 and 0 <= arr[1] < arr[3] <= 1):
            raise ValueError(f"{image_id}: normalized teacher box is invalid: {arr.tolist()}")
        if not 0 <= score <= 1:
            raise ValueError(f"{image_id}: teacher score outside [0,1]")
        result.append((arr, score))
    return result


def match_boxes(
    predictions: list[tuple[np.ndarray, float]],
    ground_truth: list[tuple[float, float, float, float]],
    confidence: float,
) -> tuple[list[tuple[np.ndarray, float, bool]], dict[str, int]]:
    chosen = [(box, score) for box, score in predictions if score >= confidence]
    matched_gt: set[int] = set()
    shown: list[tuple[np.ndarray, float, bool]] = []
    tp = fp = 0
    for box, score in sorted(chosen, key=lambda item: item[1], reverse=True):
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
    return shown, {
        "gt": len(ground_truth), "pred": len(chosen), "tp": tp,
        "fp": fp, "fn": len(ground_truth) - tp,
    }


def rect(box: tuple[float, ...] | np.ndarray) -> tuple[int, int, int, int]:
    return tuple(
        int(round(float(v) * (IMAGE_WIDTH if index % 2 == 0 else IMAGE_HEIGHT) * SCALE))
        for index, v in enumerate(box)
    )  # type: ignore[return-value]


def render_panel(
    pixels: np.ndarray,
    image_id: str,
    ground_truth: list[tuple[float, float, float, float]],
    predictions: list[tuple[np.ndarray, float, bool]],
    stats: dict[str, int],
    confidence: float,
    max_draw_predictions: int,
) -> Image.Image:
    gray = Image.fromarray(pixels.astype(np.uint8), mode="L")
    canvas = gray.convert("RGB").resize(
        (IMAGE_WIDTH * SCALE, IMAGE_HEIGHT * SCALE), Image.Resampling.NEAREST
    )
    draw = ImageDraw.Draw(canvas)
    for gt in ground_truth:
        draw.rectangle(rect(gt), outline=GT_COLOR, width=max(2, SCALE))
    # Predictions are sorted high score first; cap only what is drawn, not metric counts.
    for box, score, is_tp in predictions[:max_draw_predictions]:
        coords = rect(box)
        color = TP_COLOR if is_tp else FP_COLOR
        draw.rectangle(coords, outline=color, width=max(2, SCALE))
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
    ImageDraw.Draw(panel).text((5, 8), caption, fill="black", font=ImageFont.load_default())
    return panel


def select_ids(records: list[dict[str, Any]], positive_count: int, empty_count: int) -> list[str]:
    positives = [r for r in records if r["gt_count"] > 0]
    empty = [r for r in records if r["gt_count"] == 0]
    rankings = [
        sorted(positives, key=lambda item: item["fp"], reverse=True),
        sorted(positives, key=lambda item: item["fn"], reverse=True),
        sorted(positives, key=lambda item: item["pred"], reverse=True),
    ]
    chosen: list[dict[str, Any]] = []
    chosen_ids: set[str] = set()
    max_rank = max((len(rows) for rows in rankings), default=0)
    for rank in range(max_rank):
        for ranking in rankings:
            if rank >= len(ranking):
                continue
            item = ranking[rank]
            if item["image_id"] not in chosen_ids:
                chosen.append(item)
                chosen_ids.add(item["image_id"])
                if len(chosen) >= positive_count:
                    break
        if len(chosen) >= positive_count:
            break
    chosen_empty = sorted(empty, key=lambda item: item["pred"], reverse=True)[:empty_count]
    return [r["image_id"] for r in chosen[:positive_count] + chosen_empty]


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--teacher-cache", type=Path, required=True, help="YOLO teacher validation JSON cache.")
    parser.add_argument("--dataset", type=Path, required=True, help="Processed dataset with images/val and labels/val.")
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--confidence", type=float, default=0.25)
    parser.add_argument("--positive-count", type=int, default=6)
    parser.add_argument("--empty-count", type=int, default=4)
    parser.add_argument("--max-draw-predictions", type=int, default=15)
    args = parser.parse_args()
    if not 0 <= args.confidence <= 1:
        parser.error("confidence must be in [0,1]")
    if args.positive_count < 0 or args.empty_count < 0 or args.max_draw_predictions < 1:
        parser.error("panel counts must be non-negative and max-draw-predictions must be positive")
    args.teacher_cache = args.teacher_cache.expanduser().resolve()
    args.dataset = args.dataset.expanduser().resolve()
    args.output_dir = args.output_dir.expanduser().resolve()
    records = json.loads(args.teacher_cache.read_text(encoding="utf-8"))
    if not isinstance(records, list):
        raise ValueError("Teacher cache must contain a JSON list")

    image_dir = args.dataset / "images" / "val"
    label_dir = args.dataset / "labels" / "val"
    image_map = {
        path.stem: path
        for pattern in ("*.png", "*.jpg", "*.jpeg")
        for path in image_dir.glob(pattern)
    }
    label_map = {path.stem: path for path in label_dir.glob("*.txt")}
    if not image_map or set(image_map) != set(label_map):
        raise RuntimeError(f"Validation image/label IDs differ or are missing: {len(image_map)} vs {len(label_map)}")
    cache: dict[str, dict[str, Any]] = {}
    for record in records:
        image_id = Path(str(record.get("image", ""))).stem
        if not image_id or image_id in cache:
            raise ValueError(f"Teacher cache has a missing or duplicate image ID: {image_id!r}")
        cache[image_id] = record
    if set(cache) != set(image_map):
        raise RuntimeError(
            f"Teacher cache IDs don't match validation images: "
            f"missing={sorted(set(image_map) - set(cache))[:5]}, "
            f"extra={sorted(set(cache) - set(image_map))[:5]}"
        )

    image_ids = sorted(image_map)
    pixels_by_id: dict[str, np.ndarray] = {}
    gt_by_id: dict[str, list[tuple[float, float, float, float]]] = {}
    detections_by_id: dict[str, list[tuple[np.ndarray, float]]] = {}
    per_image: list[dict[str, Any]] = []
    total_tp = total_fp = total_fn = 0
    empty_images = empty_images_with_detections = 0
    for image_id in image_ids:
        with Image.open(image_map[image_id]) as image:
            pixels = np.asarray(image.convert("L"), dtype=np.uint8)
        if pixels.shape != (IMAGE_HEIGHT, IMAGE_WIDTH):
            raise ValueError(f"{image_id}: expected (height,width)=({IMAGE_HEIGHT},{IMAGE_WIDTH}), got {pixels.shape}")
        ground_truth = load_labels(label_map[image_id])
        detections = parse_teacher_record(cache[image_id], image_id)
        shown, stats = match_boxes(detections, ground_truth, args.confidence)
        pixels_by_id[image_id] = pixels
        gt_by_id[image_id] = ground_truth
        detections_by_id[image_id] = detections
        total_tp += stats["tp"]
        total_fp += stats["fp"]
        total_fn += stats["fn"]
        empty_images += int(stats["gt"] == 0)
        empty_images_with_detections += int(stats["gt"] == 0 and stats["pred"] > 0)
        per_image.append({
            "image_id": image_id,
            "gt_count": stats["gt"],
            "pred": stats["pred"],
            "tp": stats["tp"],
            "fp": stats["fp"],
            "fn": stats["fn"],
            "negative_image": stats["gt"] == 0,
            "negative_image_has_detection": stats["gt"] == 0 and stats["pred"] > 0,
        })

    selected_ids = select_ids(per_image, args.positive_count, args.empty_count)
    args.output_dir.mkdir(parents=True, exist_ok=True)
    panels: list[Image.Image] = []
    by_id = {r["image_id"]: r for r in per_image}
    for image_id in selected_ids:
        shown, stats = match_boxes(detections_by_id[image_id], gt_by_id[image_id], args.confidence)
        panels.append(render_panel(
            pixels_by_id[image_id], image_id, gt_by_id[image_id], shown,
            stats, args.confidence, args.max_draw_predictions
        ))
        panels[-1].save(args.output_dir / f"{image_id}_teacher_diagnostic.png")

    montage_path: Path | None = None
    if panels:
        columns = 2
        rows = (len(panels) + columns - 1) // columns
        cell_width = max(panel.width for panel in panels)
        cell_height = max(panel.height for panel in panels)
        montage = Image.new("RGB", (columns * cell_width, rows * cell_height), (235, 235, 235))
        for index, panel in enumerate(panels):
            montage.paste(panel, ((index % columns) * cell_width, (index // columns) * cell_height))
        montage_path = args.output_dir / "teacher_diagnostic_montage.png"
        montage.save(montage_path)

    precision = total_tp / max(total_tp + total_fp, 1)
    recall = total_tp / max(total_tp + total_fn, 1)
    report = {
        "teacher_cache": str(args.teacher_cache),
        "dataset": str(args.dataset),
        "confidence": args.confidence,
        "box_match_iou_threshold": 0.50,
        "validation_images": len(image_ids),
        "ground_truth_boxes": total_tp + total_fn,
        "true_positives": total_tp,
        "false_positives": total_fp,
        "false_negatives": total_fn,
        "predicted_boxes": total_tp + total_fp,
        "precision": precision,
        "recall": recall,
        "f1": 2 * precision * recall / max(precision + recall, 1e-12),
        "empty_images": empty_images,
        "empty_images_with_detections": empty_images_with_detections,
        "selected_image_ids": selected_ids,
        "color_legend": {"ground_truth": "green", "matched_prediction": "blue/cyan", "false_positive_prediction": "red"},
        "note": "Evaluates cached YOLO teacher boxes at the given confidence threshold, matching IoU >= 0.50. Teacher boxes are assumed to be normalized xyxy and already postprocessed by the teacher inference pipeline.",
        "per_image": per_image,
    }
    report_path = args.output_dir / "teacher_diagnostic_report.json"
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print(json.dumps({
        key: report[key] for key in (
            "confidence", "validation_images", "ground_truth_boxes", "true_positives",
            "false_positives", "false_negatives", "predicted_boxes", "precision",
            "recall", "f1", "empty_images", "empty_images_with_detections",
            "selected_image_ids",
        )
    }, indent=2))
    print("Saved teacher montage:", montage_path)
    print("Saved teacher report:", report_path)


if __name__ == "__main__":
    main()
