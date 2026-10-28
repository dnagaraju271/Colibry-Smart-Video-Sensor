#!/usr/bin/env python3
"""Export a trained Colibry Keras grid detector as fully-int8 TFLite.

This is a host-side artifact preparation step. It does not invoke the NNPA
compiler, access the board, or flash firmware.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import tensorflow as tf
from PIL import Image

WIDTH, HEIGHT = 160, 128


def load_representative_images(dataset: Path, limit: int) -> list[np.ndarray]:
    image_dir = dataset / "images" / "train"
    paths = sorted(
        p for p in image_dir.iterdir()
        if p.is_file() and p.suffix.lower() in {".png", ".jpg", ".jpeg"}
    )
    if not paths:
        raise ValueError(f"No training images found in {image_dir}")
    selected = paths[:limit]
    samples: list[np.ndarray] = []
    for path in selected:
        with Image.open(path) as image:
            pixels = np.asarray(image.convert("L"), dtype=np.uint8)
        if pixels.shape != (HEIGHT, WIDTH):
            raise ValueError(
                f"{path}: expected {WIDTH}x{HEIGHT} grayscale, got {pixels.shape[::-1]}"
            )
        samples.append(pixels[..., None].astype(np.float32)[None, ...] / 255.0)
    return samples


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--model", type=Path, required=True, help="Trained .keras model")
    parser.add_argument("--dataset", type=Path, required=True, help="Processed dataset root")
    parser.add_argument("--output", type=Path, required=True, help="Output .tflite path")
    parser.add_argument("--representative-limit", type=int, default=200)
    args = parser.parse_args()

    model_path = args.model.expanduser().resolve()
    dataset = args.dataset.expanduser().resolve()
    output = args.output.expanduser().resolve()
    if not model_path.is_file():
        parser.error(f"model not found: {model_path}")
    if args.representative_limit < 1:
        parser.error("--representative-limit must be >= 1")

    model = tf.keras.models.load_model(model_path, compile=False)
    expected_input = (None, HEIGHT, WIDTH, 1)
    expected_output = (None, 32, 40, 5)
    if tuple(model.input_shape) != expected_input:
        raise ValueError(f"Unexpected model input {model.input_shape}; expected {expected_input}")
    if tuple(model.output_shape) != expected_output:
        raise ValueError(f"Unexpected model output {model.output_shape}; expected {expected_output}")

    samples = load_representative_images(dataset, args.representative_limit)

    def representative_dataset():
        for sample in samples:
            yield [sample]

    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    converter.optimizations = [tf.lite.Optimize.DEFAULT]
    converter.representative_dataset = representative_dataset
    converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
    converter.inference_input_type = tf.int8
    converter.inference_output_type = tf.int8
    tflite_bytes = converter.convert()

    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(tflite_bytes)

    interpreter = tf.lite.Interpreter(model_path=str(output))
    interpreter.allocate_tensors()
    input_detail = interpreter.get_input_details()[0]
    output_detail = interpreter.get_output_details()[0]

    def tensor_summary(detail: dict) -> dict:
        scales = np.asarray(detail.get("quantization_parameters", {}).get("scales", []))
        zero_points = np.asarray(detail.get("quantization_parameters", {}).get("zero_points", []))
        return {
            "name": detail.get("name"),
            "shape": [int(x) for x in detail.get("shape", [])],
            "dtype": str(detail.get("dtype")),
            "quantization_scale": scales.tolist(),
            "quantization_zero_point": zero_points.tolist(),
        }

    report = {
        "model": str(model_path),
        "dataset": str(dataset),
        "representative_images": len(samples),
        "output": str(output),
        "output_bytes": len(tflite_bytes),
        "fully_integer_quantization_requested": True,
        "input": tensor_summary(input_detail),
        "output_tensor": tensor_summary(output_detail),
        "limitations": [
            "TFLite conversion does not prove Colibry NNPA compiler compatibility.",
            "This model must be trained and validated before detections are trusted.",
            "The compiled board output layout and quantization still need independent verification.",
        ],
    }
    report_path = output.with_suffix(".quantization.json")
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))
    print(f"Saved quantization report: {report_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
