#!/usr/bin/env python3
"""Run a trained fully-int8 Colibry grid detector on one image (host-side).

This validates the trained model and decoder before attempting NNPA deployment.
It does not access the board. Input image is resized to 160x128 and converted
to grayscale, matching the training script's preprocessing contract.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
from PIL import Image, ImageDraw

WIDTH, HEIGHT = 160, 128
GRID_COLS, GRID_ROWS, VALUES_PER_CELL = 40, 32, 5


def sigmoid(x: np.ndarray) -> np.ndarray:
    return 1.0 / (1.0 + np.exp(-np.clip(x, -30.0, 30.0)))


def iou_one_to_many(box: np.ndarray, boxes: np.ndarray) -> np.ndarray:
    x1 = np.maximum(box[0], boxes[:, 0])
    y1 = np.maximum(box[1], boxes[:, 1])
    x2 = np.minimum(box[2], boxes[:, 2])
    y2 = np.minimum(box[3], boxes[:, 3])
    inter = np.maximum(x2 - x1, 0.0) * np.maximum(y2 - y1, 0.0)
    area_a = max(float(box[2] - box[0]), 0.0) * max(float(box[3] - box[1]), 0.0)
    area_b = np.maximum(boxes[:, 2] - boxes[:, 0], 0.0) * np.maximum(boxes[:, 3] - boxes[:, 1], 0.0)
    return inter / np.maximum(area_a + area_b - inter, 1e-9)


def decode(raw: np.ndarray, score_threshold: float, iou_threshold: float) -> list[dict]:
    if raw.shape == (1, GRID_ROWS, GRID_COLS, VALUES_PER_CELL):
        raw = raw[0]
    if raw.shape != (GRID_ROWS, GRID_COLS, VALUES_PER_CELL):
        raise ValueError(f"Unexpected output shape {raw.shape}; expected (1,32,40,5)")

    values = sigmoid(raw[..., 1:5])
    scores = sigmoid(raw[..., 0])
    rows, cols = np.meshgrid(
        np.arange(GRID_ROWS, dtype=np.float32),
        np.arange(GRID_COLS, dtype=np.float32),
        indexing="ij",
    )
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
    candidates = np.flatnonzero(scores >= score_threshold)
    candidates = candidates[np.argsort(scores[candidates])[::-1]]
    kept: list[int] = []
    while candidates.size:
        current = int(candidates[0])
        kept.append(current)
        if candidates.size == 1:
            break
        rest = candidates[1:]
        overlaps = iou_one_to_many(boxes[current], boxes[rest])
        candidates = rest[overlaps <= iou_threshold]

    return [
        {
            "class": "person",
            "confidence": float(scores[i]),
            "box_xyxy_normalized": [float(v) for v in boxes[i]],
            "box_xyxy_pixels": [
                int(round(boxes[i, 0] * WIDTH)),
                int(round(boxes[i, 1] * HEIGHT)),
                int(round(boxes[i, 2] * WIDTH)),
                int(round(boxes[i, 3] * HEIGHT)),
            ],
        }
        for i in kept
    ]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--model", type=Path, required=True, help="Fully-int8 .tflite model")
    parser.add_argument("--image", type=Path, required=True, help="Input image")
    parser.add_argument("--output-image", type=Path, default=Path("person_detection.png"))
    parser.add_argument("--output-json", type=Path, default=Path("person_detection.json"))
    parser.add_argument("--confidence", type=float, default=0.35)
    parser.add_argument("--nms-iou", type=float, default=0.50)
    args = parser.parse_args()

    model_path = args.model.expanduser().resolve()
    image_path = args.image.expanduser().resolve()
    if not model_path.is_file() or not image_path.is_file():
        parser.error("Both --model and --image must point to existing files.")
    if not 0 <= args.confidence <= 1 or not 0 <= args.nms_iou <= 1:
        parser.error("--confidence and --nms-iou must be in [0,1]")

    try:
        from tflite_runtime.interpreter import Interpreter
    except ImportError:
        import tensorflow as tf
        Interpreter = tf.lite.Interpreter

    interpreter = Interpreter(model_path=str(model_path))
    interpreter.allocate_tensors()
    input_detail = interpreter.get_input_details()[0]
    output_detail = interpreter.get_output_details()[0]
    if tuple(input_detail["shape"]) != (1, HEIGHT, WIDTH, 1):
        raise ValueError(f"Unexpected input shape {input_detail['shape']}")
    if tuple(output_detail["shape"]) != (1, GRID_ROWS, GRID_COLS, VALUES_PER_CELL):
        raise ValueError(f"Unexpected output shape {output_detail['shape']}")

    with Image.open(image_path) as image:
        original = image.convert("RGB")
        gray = image.convert("L").resize((WIDTH, HEIGHT), Image.Resampling.BILINEAR)
    pixels = np.asarray(gray, dtype=np.float32)[None, ..., None] / 255.0
    input_scale, input_zero = input_detail["quantization"]
    if input_scale <= 0:
        raise ValueError(f"Invalid input quantization: {input_detail['quantization']}")
    quantized = np.clip(np.rint(pixels / input_scale + input_zero), -128, 127).astype(np.int8)
    interpreter.set_tensor(input_detail["index"], quantized)
    interpreter.invoke()
    raw = interpreter.get_tensor(output_detail["index"])
    output_scale, output_zero = output_detail["quantization"]
    if output_scale <= 0:
        raise ValueError(f"Invalid output quantization: {output_detail['quantization']}")
    logits = (raw.astype(np.float32) - output_zero) * output_scale
    detections = decode(logits, args.confidence, args.nms_iou)

    draw = ImageDraw.Draw(original)
    sx, sy = original.width / WIDTH, original.height / HEIGHT
    for item in detections:
        x1, y1, x2, y2 = item["box_xyxy_pixels"]
        coords = (round(x1 * sx), round(y1 * sy), round(x2 * sx), round(y2 * sy))
        draw.rectangle(coords, outline=(255, 32, 32), width=max(2, original.width // 160))
        draw.text((coords[0], max(0, coords[1] - 12)), f"person {item['confidence']:.2f}", fill=(255, 32, 32))

    image_out = args.output_image.expanduser().resolve()
    json_out = args.output_json.expanduser().resolve()
    image_out.parent.mkdir(parents=True, exist_ok=True)
    json_out.parent.mkdir(parents=True, exist_ok=True)
    original.save(image_out)
    report = {
        "model": str(model_path),
        "image": str(image_path),
        "input_size": [WIDTH, HEIGHT],
        "confidence_threshold": args.confidence,
        "nms_iou_threshold": args.nms_iou,
        "person_count": len(detections),
        "detections": detections,
        "annotated_image": str(image_out),
        "note": "Host-side TFLite result only; this does not prove NNPA compatibility or board inference.",
    }
    json_out.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
