#!/usr/bin/env python3
"""Small-subset overfit sanity check for the Colibry detector.

This is a diagnostic experiment, not a candidate model. It uses a deterministic,
balanced subset of the TRAIN split and reports whether the detector can memorize
its labelled examples. It does not access or program the physical board.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import tensorflow as tf
import tf_keras as keras

import train_colibry_person_detector as detector


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--positive-images", type=int, default=16)
    parser.add_argument("--empty-images", type=int, default=16)
    parser.add_argument("--epochs", type=int, default=100)
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--learning-rate", type=float, default=1e-3)
    parser.add_argument("--negative-loss-weight", type=float, default=1.0)
    args = parser.parse_args()
    if min(args.positive_images, args.empty_images, args.epochs, args.batch_size) < 1:
        parser.error("image counts, epochs, and batch-size must be positive")
    if args.learning_rate <= 0 or args.negative_loss_weight <= 0:
        parser.error("learning-rate and negative-loss-weight must be positive")

    dataset = args.dataset.expanduser().resolve()
    out = args.output_dir.expanduser().resolve()
    if out.exists() and any(out.iterdir()):
        parser.error(f"Output directory is not empty; choose a new path: {out}")
    out.mkdir(parents=True, exist_ok=True)

    x, y, gt_by_image, split_summary = detector.load_split(dataset, "train")
    positive = [i for i, boxes in enumerate(gt_by_image) if len(boxes) > 0]
    empty = [i for i, boxes in enumerate(gt_by_image) if len(boxes) == 0]
    if len(positive) < args.positive_images or len(empty) < args.empty_images:
        parser.error("Not enough positive/empty images in the training split")

    # Sorted split order and fixed seed make this reproducible.
    rng = np.random.default_rng(42)
    selected_pos = sorted(rng.choice(positive, args.positive_images, replace=False).tolist())
    selected_neg = sorted(rng.choice(empty, args.empty_images, replace=False).tolist())
    selected = np.asarray(sorted(selected_pos + selected_neg), dtype=np.int64)
    x_small, y_small = x[selected], y[selected]
    gt_small = [gt_by_image[i] for i in selected]

    detector.NEGATIVE_LOSS_WEIGHT = args.negative_loss_weight
    np.random.seed(42)
    tf.random.set_seed(42)
    model = detector.build_model()
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=args.learning_rate),
        loss=detector.hard_negative_giou_detection_loss,
    )
    history = model.fit(
        x_small, y_small,
        epochs=args.epochs,
        batch_size=args.batch_size,
        shuffle=True,
        verbose=2,
    )
    raw = model.predict(x_small, batch_size=args.batch_size, verbose=0)
    predictions = [detector.decode_predictions(item, score_floor=0.01) for item in raw]
    thresholds = [0.25, 0.50, 0.70, 0.90]
    report = {
        "purpose": "training-subset memorization check only; not validation performance",
        "dataset": str(dataset),
        "source_split": "train",
        "source_split_summary": split_summary,
        "selected_images": len(selected),
        "selected_positive_images": len(selected_pos),
        "selected_known_empty_images": len(selected_neg),
        "epochs_requested": args.epochs,
        "epochs_completed": len(history.history.get("loss", [])),
        "first_training_loss": float(history.history["loss"][0]),
        "last_training_loss": float(history.history["loss"][-1]),
        "minimum_training_loss": float(min(history.history["loss"])),
        "loss_history": [float(v) for v in history.history["loss"]],
        "training_subset_metrics": {
            f"confidence_{t:.2f}": detector.evaluate_predictions(predictions, gt_small, t)
            for t in thresholds
        },
        "objectness_probability": {
            "positive_cells": _distribution(
                (1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30, 30))))[
                    np.asarray([len(gt_by_image[i]) > 0 for i in selected])
                ]
            ),
            "empty_images_max": _distribution(
                np.max(1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30, 30))), axis=(1, 2))[
                    np.asarray([len(gt_by_image[i]) == 0 for i in selected])
                ]
            ),
        },
        "note": "Do not export or deploy this diagnostic model. The purpose is to test whether the current architecture/loss can fit a small balanced subset.",
    }
    report_path = out / "overfit_sanity_report.json"
    report_path.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    model.save(str(out / "overfit_sanity_model.keras"))
    print(json.dumps(report, indent=2))
    print(f"Saved report: {report_path}")
    print("Diagnostic model only: do not export or deploy.")
    return 0


def _distribution(values: np.ndarray) -> dict[str, float]:
    values = np.asarray(values, dtype=np.float64).reshape(-1)
    if not values.size:
        return {"count": 0}
    q = np.quantile(values, [0, .5, .9, .99, 1])
    return {
        "count": int(values.size),
        "min": float(q[0]),
        "median": float(q[1]),
        "p90": float(q[2]),
        "p99": float(q[3]),
        "max": float(q[4]),
        "mean": float(values.mean()),
    }


if __name__ == "__main__":
    raise SystemExit(main())
