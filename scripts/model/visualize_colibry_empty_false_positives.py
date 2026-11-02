#!/usr/bin/env python3
"""Create a host-side contact sheet of highest-scoring known-empty validation images."""
from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
import tf_keras as keras
from PIL import Image, ImageDraw

from train_colibry_person_detector import load_split, decode_predictions

WIDTH, HEIGHT = 160, 128


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--confidence", type=float, default=0.50)
    parser.add_argument("--limit", type=int, default=10)
    args = parser.parse_args()
    if not 0 <= args.confidence <= 1:
        parser.error("--confidence must be in [0,1]")
    if args.limit < 1 or args.limit > 50:
        parser.error("--limit must be between 1 and 50")

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    output_path = args.output.expanduser().resolve()
    if not model_path.is_file():
        parser.error(f"Model not found: {model_path}")

    x_val, _y_val, gt_by_image, _summary = load_split(dataset, "val")
    image_dir = dataset / "images" / "val"
    image_paths = sorted(
        [p for ext in ("*.png", "*.jpg", "*.jpeg") for p in image_dir.glob(ext)],
        key=lambda p: p.stem,
    )
    if len(image_paths) != len(x_val):
        raise RuntimeError(f"Image ordering/count mismatch: {len(image_paths)} vs {len(x_val)}")

    model = keras.models.load_model(str(model_path), compile=False)
    raw = model.predict(x_val, batch_size=8, verbose=1)
    objectness = 1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30, 30)))
    empty_indices = [i for i, gt in enumerate(gt_by_image) if len(gt) == 0]
    ranked = sorted(empty_indices, key=lambda i: float(objectness[i].max()), reverse=True)
    selected = ranked[:args.limit]

    thumb_w, thumb_h = 480, 384
    header_h, gap = 42, 12
    cols = 2
    rows = (len(selected) + cols - 1) // cols
    sheet = Image.new("RGB", (cols * thumb_w + (cols + 1) * gap,
                              rows * (thumb_h + header_h) + (rows + 1) * gap),
                      (245, 245, 245))
    canvas = ImageDraw.Draw(sheet)
    summary = []
    for slot, idx in enumerate(selected):
        x0 = gap + (slot % cols) * (thumb_w + gap)
        y0 = gap + (slot // cols) * (thumb_h + header_h + gap)
        with Image.open(image_paths[idx]) as source:
            original = source.convert("RGB")
            thumb = original.resize((thumb_w, thumb_h), Image.Resampling.BILINEAR)
        preds = decode_predictions(raw[idx], score_floor=0.01)
        chosen = sorted(
            [(box, score) for box, score in preds if score >= args.confidence],
            key=lambda item: item[1], reverse=True
        )
        canvas.text((x0, y0 + 2),
                    f"{image_paths[idx].stem} | max cell={objectness[idx].max():.3f} "
                    f"| boxes >= {args.confidence:.2f}: {len(chosen)}",
                    fill=(15, 15, 15))
        sheet.paste(thumb, (x0, y0 + header_h))
        draw = ImageDraw.Draw(sheet)
        sx, sy = thumb_w / WIDTH, thumb_h / HEIGHT
        for box, score in chosen[:25]:
            x1, y1, x2, y2 = [float(v) for v in box]
            coords = (int(x1 * WIDTH * sx), int(y1 * HEIGHT * sy),
                      int(x2 * WIDTH * sx), int(y2 * HEIGHT * sy))
            draw.rectangle(coords, outline=(230, 25, 35), width=2)
            draw.text((coords[0], max(y0 + header_h, coords[1] - 12)),
                      f"{score:.2f}", fill=(255, 230, 0), stroke_width=1, stroke_fill=(0, 0, 0))
        summary.append({
            "image": image_paths[idx].stem,
            "max_cell_objectness": float(objectness[idx].max()),
            "mean_objectness": float(objectness[idx].mean()),
            "detections_at_threshold": len(chosen),
            "shown_boxes": min(25, len(chosen)),
            "source_path": str(image_paths[idx]),
        })

    output_path.parent.mkdir(parents=True, exist_ok=True)
    sheet.save(output_path)
    print(f"Saved contact sheet: {output_path}")
    print("Known-empty images selected by maximum cell objectness:")
    for item in summary:
        print(item)
    print("Red rectangles are model predictions, not ground-truth boxes.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
