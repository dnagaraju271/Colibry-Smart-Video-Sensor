#!/usr/bin/env python3
"""Render validation examples with ground truth and detector predictions (host-only)."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
import tf_keras as keras
from PIL import Image, ImageDraw

from train_colibry_person_detector import decode_predictions, load_split

WIDTH, HEIGHT = 160, 128


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True,
                        help="New PNG path; existing files are never overwritten.")
    parser.add_argument("--confidence", type=float, default=0.50)
    parser.add_argument("--positive", type=int, default=6,
                        help="Number of positive validation images to include.")
    parser.add_argument("--empty", type=int, default=6,
                        help="Number of known-empty images with highest objectness to include.")
    parser.add_argument("--max-boxes", type=int, default=12,
                        help="Maximum predicted boxes drawn per image.")
    args = parser.parse_args()
    if not 0 <= args.confidence <= 1:
        parser.error("--confidence must be in [0, 1]")
    if args.positive < 0 or args.empty < 0 or args.positive + args.empty < 1:
        parser.error("positive/empty limits must be non-negative and sum to at least one")
    if args.max_boxes < 1:
        parser.error("--max-boxes must be at least 1")

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    output_path = args.output.expanduser().resolve()
    if not model_path.is_file():
        parser.error(f"Model not found: {model_path}")
    if output_path.exists():
        parser.error(f"Output already exists; choose a new path to preserve artifacts: {output_path}")

    x_val, _y_val, gt_by_image, split_summary = load_split(dataset, "val")
    image_dir = dataset / "images" / "val"
    image_paths = sorted(
        [p for ext in ("*.png", "*.jpg", "*.jpeg") for p in image_dir.glob(ext)],
        key=lambda p: p.stem,
    )
    if len(image_paths) != len(x_val):
        raise RuntimeError(
            f"Image ordering/count mismatch: {len(image_paths)} image files vs {len(x_val)} tensors"
        )

    model = keras.models.load_model(str(model_path), compile=False)
    raw = model.predict(x_val, batch_size=8, verbose=1)
    if len(raw) != len(image_paths):
        raise RuntimeError(f"Prediction count mismatch: {len(raw)} vs {len(image_paths)}")

    # Objectness is the first output channel. Use it only to rank empty examples.
    objectness = 1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30, 30)))
    positives = [i for i, boxes in enumerate(gt_by_image) if len(boxes) > 0]
    empties = [i for i, boxes in enumerate(gt_by_image) if len(boxes) == 0]
    # Prefer positive examples with multiple GT boxes, then stable dataset order.
    positives = sorted(positives, key=lambda i: (-len(gt_by_image[i]), i))
    empties = sorted(empties, key=lambda i: (-float(objectness[i].max()), i))
    selected = [(i, "POSITIVE") for i in positives[:args.positive]]
    selected += [(i, "KNOWN-EMPTY") for i in empties[:args.empty]]
    if not selected:
        raise RuntimeError("No validation images selected; check dataset labels.")

    thumb_w, thumb_h = 480, 384
    header_h, gap = 48, 14
    cols = 2
    rows = (len(selected) + cols - 1) // cols
    sheet = Image.new(
        "RGB",
        (cols * thumb_w + (cols + 1) * gap,
         rows * (thumb_h + header_h) + (rows + 1) * gap),
        (245, 245, 245),
    )
    draw = ImageDraw.Draw(sheet)
    report_lines = []
    for slot, (idx, kind) in enumerate(selected):
        x0 = gap + (slot % cols) * (thumb_w + gap)
        y0 = gap + (slot // cols) * (thumb_h + header_h + gap)
        with Image.open(image_paths[idx]) as source:
            thumb = source.convert("RGB").resize((thumb_w, thumb_h), Image.Resampling.BILINEAR)
        sheet.paste(thumb, (x0, y0 + header_h))
        draw = ImageDraw.Draw(sheet)
        sx, sy = thumb_w / WIDTH, thumb_h / HEIGHT

        # Green = labelled ground truth; red = model predictions.
        gt_boxes = gt_by_image[idx]
        for box in gt_boxes:
            x1, y1, x2, y2 = [float(v) for v in box]
            coords = (x0 + int(x1 * thumb_w), y0 + header_h + int(y1 * thumb_h),
                      x0 + int(x2 * thumb_w), y0 + header_h + int(y2 * thumb_h))
            draw.rectangle(coords, outline=(20, 210, 70), width=3)

        predictions = decode_predictions(raw[idx], score_floor=0.01)
        predictions = sorted(
            [(box, float(score)) for box, score in predictions if float(score) >= args.confidence],
            key=lambda item: item[1], reverse=True,
        )
        shown = predictions[:args.max_boxes]
        for box, score in shown:
            x1, y1, x2, y2 = [float(v) for v in box]
            coords = (x0 + int(x1 * WIDTH * sx), y0 + header_h + int(y1 * HEIGHT * sy),
                      x0 + int(x2 * WIDTH * sx), y0 + header_h + int(y2 * HEIGHT * sy))
            draw.rectangle(coords, outline=(235, 35, 35), width=2)
            label_y = max(y0 + header_h, coords[1] - 14)
            draw.text((coords[0], label_y), f"{score:.2f}", fill=(255, 245, 0),
                      stroke_width=1, stroke_fill=(0, 0, 0))

        title = (f"{kind} | {image_paths[idx].stem} | GT={len(gt_boxes)} "
                 f"| predictions >= {args.confidence:.2f}: {len(predictions)} "
                 f"| max cell obj={float(objectness[idx].max()):.3f}")
        draw.text((x0, y0 + 2), title, fill=(15, 15, 15))
        report_lines.append(
            f"{kind:11s} {image_paths[idx].stem:30s} GT={len(gt_boxes):2d} "
            f"predictions={len(predictions):3d} shown={len(shown):2d} "
            f"max_cell_objectness={float(objectness[idx].max()):.4f}"
        )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    sheet.save(output_path)
    print(f"Validation split: {split_summary}")
    print(f"Saved contact sheet: {output_path}")
    print("Legend: GREEN = ground-truth person boxes; RED = predicted boxes; yellow text = prediction score.")
    print("Selected examples:")
    print("\n".join(report_lines))
    print("This is a visual diagnostic only; it does not change model weights or access the board.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
