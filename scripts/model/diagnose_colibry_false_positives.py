#!/usr/bin/env python3
"""Diagnose objectness scores and false positives on the labelled validation split.

Host-only diagnostic. Loads a saved Keras model, never accesses the board, and
writes a JSON report to the requested output path.
"""
from __future__ import annotations

import argparse
import importlib.util
import json
from pathlib import Path

import numpy as np
import tf_keras as keras

from train_colibry_person_detector import (
    IMAGE_HEIGHT,
    IMAGE_WIDTH,
    load_split,
    decode_predictions,
    evaluate_predictions,
)


def quantiles(values: np.ndarray) -> dict[str, float]:
    values = np.asarray(values, dtype=np.float64).reshape(-1)
    if values.size == 0:
        return {"count": 0}
    qs = np.quantile(values, [0.0, 0.25, 0.5, 0.75, 0.9, 0.95, 0.99, 1.0])
    return {
        "count": int(values.size),
        "min": float(qs[0]),
        "p25": float(qs[1]),
        "median": float(qs[2]),
        "p75": float(qs[3]),
        "p90": float(qs[4]),
        "p95": float(qs[5]),
        "p99": float(qs[6]),
        "max": float(qs[7]),
        "mean": float(np.mean(values)),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    output_path = args.output.expanduser().resolve()
    if not model_path.is_file():
        parser.error(f"Model not found: {model_path}")

    x_val, _y_val, gt_by_image, split_summary = load_split(dataset, "val")
    image_dir = dataset / "images" / "val"
    image_paths = sorted(
        [p for ext in ("*.png", "*.jpg", "*.jpeg") for p in image_dir.glob(ext)],
        key=lambda p: p.stem,
    )
    if len(image_paths) != len(x_val):
        raise RuntimeError(f"Image order/count mismatch: {len(image_paths)} paths vs {len(x_val)} images")

    model = keras.models.load_model(str(model_path), compile=False)
    raw = model.predict(x_val, batch_size=8, verbose=1)
    if raw.shape != (len(x_val), 32, 40, 5):
        raise ValueError(f"Unexpected output shape: {raw.shape}")

    scores = 1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30.0, 30.0)))
    positive_mask = np.asarray([len(boxes) > 0 for boxes in gt_by_image], dtype=bool)
    empty_mask = ~positive_mask
    image_max = scores.max(axis=(1, 2))
    image_mean = scores.mean(axis=(1, 2))
    image_paths_stems = [p.stem for p in image_paths]

    predictions = [decode_predictions(item, score_floor=0.01) for item in raw]
    thresholds = [0.10, 0.20, 0.25, 0.30, 0.35, 0.40, 0.50, 0.60, 0.70, 0.80, 0.90]
    threshold_metrics = {
        f"{threshold:.2f}": evaluate_predictions(predictions, gt_by_image, threshold)
        for threshold in thresholds
    }

    empty_indices = np.flatnonzero(empty_mask)
    empty_ranked = sorted(
        empty_indices.tolist(), key=lambda i: float(image_max[i]), reverse=True
    )
    report = {
        "model": str(model_path),
        "dataset": str(dataset),
        "split_summary": split_summary,
        "output_shape": list(raw.shape),
        "objectness_probability_distribution": {
            "all_grid_cells": quantiles(scores),
            "positive_images_per_cell": quantiles(scores[positive_mask]),
            "known_empty_images_per_cell": quantiles(scores[empty_mask]),
            "positive_image_max_score": quantiles(image_max[positive_mask]),
            "empty_image_max_score": quantiles(image_max[empty_mask]),
            "positive_image_mean_score": quantiles(image_mean[positive_mask]),
            "empty_image_mean_score": quantiles(image_mean[empty_mask]),
        },
        "per_image_threshold_counts": {
            f"{threshold:.2f}": {
                "positive_images_mean_cells_above_threshold": float(
                    np.mean((scores[positive_mask] >= threshold).sum(axis=(1, 2)))
                ),
                "empty_images_mean_cells_above_threshold": float(
                    np.mean((scores[empty_mask] >= threshold).sum(axis=(1, 2)))
                ),
                "empty_images_with_any_cell_above_threshold": int(
                    np.sum((scores[empty_mask] >= threshold).any(axis=(1, 2)))
                ),
            }
            for threshold in [0.25, 0.35, 0.50, 0.70, 0.90]
        },
        "validation_metrics_after_nms": threshold_metrics,
        "highest_scoring_known_empty_images": [
            {
                "image": image_paths_stems[i],
                "path": str(image_paths[i]),
                "max_objectness": float(image_max[i]),
                "mean_objectness": float(image_mean[i]),
                "cells_at_or_above_0.50": int(np.sum(scores[i] >= 0.50)),
                "cells_at_or_above_0.70": int(np.sum(scores[i] >= 0.70)),
                "decoded_boxes_at_0.50": int(sum(score >= 0.50 for _, score in predictions[i])),
            }
            for i in empty_ranked[:10]
        ],
        "note": (
            "Host-side diagnostic only. Confidence metrics use the training script's "
            "decoder/NMS and greedy IoU>=0.50 matching; not COCO mAP and not board inference."
        ),
    }
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))
    print(f"Saved diagnostic report: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
