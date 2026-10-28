#!/usr/bin/env python3
"""Run a controlled small-subset overfit test; host-only and isolated from existing artifacts."""
from __future__ import annotations

import argparse
import csv
import json
from pathlib import Path

import numpy as np
import tensorflow as tf
import tf_keras as keras

import train_colibry_person_detector as detector


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True,
                        help="New directory; this script refuses to reuse an existing directory.")
    parser.add_argument("--positive", type=int, default=24,
                        help="Number of person-containing training images.")
    parser.add_argument("--empty", type=int, default=8,
                        help="Number of known-empty training images.")
    parser.add_argument("--epochs", type=int, default=40)
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--learning-rate", type=float, default=1e-3)
    args = parser.parse_args()

    if args.positive < 1 or args.empty < 1 or args.epochs < 1 or args.batch_size < 1:
        parser.error("positive, empty, epochs and batch-size must all be positive")
    if args.learning_rate <= 0:
        parser.error("learning-rate must be positive")

    dataset = args.dataset.expanduser().resolve()
    output_dir = args.output_dir.expanduser().resolve()
    if output_dir.exists():
        parser.error(f"Output directory already exists; choose a new path to preserve artifacts: {output_dir}")

    # Load labels and tensors using the same official trainer path.
    x_all, y_all, gt_all, train_summary = detector.load_split(dataset, "train")
    positive_ids = [i for i, boxes in enumerate(gt_all) if len(boxes) > 0]
    empty_ids = [i for i, boxes in enumerate(gt_all) if len(boxes) == 0]
    if len(positive_ids) < args.positive or len(empty_ids) < args.empty:
        parser.error(
            f"Requested {args.positive} positive and {args.empty} empty images, "
            f"but split has {len(positive_ids)} positive and {len(empty_ids)} empty."
        )

    # Deterministic, fixed sample: sorted IDs from the trainer's deterministic image order.
    selected = positive_ids[:args.positive] + empty_ids[:args.empty]
    x = x_all[selected]
    y = y_all[selected]
    gt = [gt_all[i] for i in selected]
    selected_positive = sum(len(gt_all[i]) > 0 for i in selected)
    selected_empty = len(selected) - selected_positive

    np.random.seed(42)
    tf.random.set_seed(42)
    model = detector.build_model()
    model.compile(
        optimizer=keras.optimizers.Adam(learning_rate=args.learning_rate),
        loss=detector.hard_negative_giou_detection_loss,
    )

    output_dir.mkdir(parents=True, exist_ok=False)
    history_path = output_dir / "overfit_history.csv"
    callback = keras.callbacks.CSVLogger(str(history_path))
    history = model.fit(
        x, y,
        epochs=args.epochs,
        batch_size=args.batch_size,
        shuffle=True,
        callbacks=[callback],
        verbose=2,
    )

    raw = np.asarray(model.predict(x, batch_size=args.batch_size, verbose=0))
    scores = 1.0 / (1.0 + np.exp(-np.clip(raw[..., 0], -30, 30)))
    decoded = [detector.decode_predictions(item, score_floor=0.01) for item in raw]
    metrics = {
        "at_confidence_0_25": detector.evaluate_predictions(decoded, gt, 0.25),
        "at_confidence_0_50": detector.evaluate_predictions(decoded, gt, 0.50),
    }
    loss_values = [float(v) for v in history.history.get("loss", [])]
    result = {
        "purpose": "fixed-subset overfit diagnostic, not a deployment evaluation",
        "dataset": str(dataset),
        "training_split_summary": train_summary,
        "selected_images": len(selected),
        "selected_positive_images": int(selected_positive),
        "selected_known_empty_images": int(selected_empty),
        "selected_ground_truth_boxes": int(sum(len(boxes) for boxes in gt)),
        "input_shape": list(x.shape),
        "target_shape": list(y.shape),
        "epochs_requested": args.epochs,
        "epochs_completed": len(loss_values),
        "learning_rate": args.learning_rate,
        "initial_train_loss": loss_values[0] if loss_values else None,
        "final_train_loss": loss_values[-1] if loss_values else None,
        "minimum_train_loss": min(loss_values) if loss_values else None,
        "objectness_logits": {
            "min": float(raw[..., 0].min()),
            "max": float(raw[..., 0].max()),
            "std": float(raw[..., 0].std()),
        },
        "objectness_scores": {
            "min": float(scores.min()),
            "median": float(np.median(scores)),
            "max": float(scores.max()),
            "std": float(scores.std()),
            "cells_ge_0_60": int(np.count_nonzero(scores >= 0.60)),
            "cells_ge_0_90": int(np.count_nonzero(scores >= 0.90)),
        },
        "training_subset_metrics": metrics,
        "artifacts": {
            "model": str(output_dir / "small_subset_overfit.keras"),
            "history_csv": str(history_path),
        },
        "interpretation": [
            "If training loss falls substantially and predictions become image-dependent, the model can fit a tiny subset; investigate full-dataset optimization/generalization next.",
            "If training loss and outputs remain nearly constant, inspect loss gradients, target weighting and optimization before a full run.",
            "Subset metrics are diagnostic only and must not be reported as validation performance.",
            "This test does not access or flash the physical Colibry board.",
        ],
    }
    model.save(str(output_dir / "small_subset_overfit.keras"))
    report_path = output_dir / "small_subset_overfit_report.json"
    report_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    print(f"Saved report: {report_path}")
    print(f"Saved model: {output_dir / 'small_subset_overfit.keras'}")
    print(f"Saved history: {history_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
