#!/usr/bin/env python3
"""Inspect raw output-channel variation and saved training history for a detector model."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import tf_keras as keras

from train_colibry_person_detector import load_split


def describe(values: np.ndarray) -> dict[str, float]:
    flat = np.asarray(values, dtype=np.float64).reshape(-1)
    return {
        "min": float(np.min(flat)),
        "p01": float(np.percentile(flat, 1)),
        "p10": float(np.percentile(flat, 10)),
        "median": float(np.median(flat)),
        "mean": float(np.mean(flat)),
        "p90": float(np.percentile(flat, 90)),
        "p99": float(np.percentile(flat, 99)),
        "max": float(np.max(flat)),
        "std": float(np.std(flat)),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--report", type=Path, default=None,
                        help="Optional training_report.json from the same run.")
    parser.add_argument("--output", type=Path, required=True,
                        help="New JSON path; existing files are never overwritten.")
    args = parser.parse_args()

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    report_path = args.report.expanduser().resolve() if args.report else None
    output_path = args.output.expanduser().resolve()
    if not model_path.is_file():
        parser.error(f"Model not found: {model_path}")
    if report_path is not None and not report_path.is_file():
        parser.error(f"Training report not found: {report_path}")
    if output_path.exists():
        parser.error(f"Output exists; choose a new path to preserve previous results: {output_path}")

    x_val, _y_val, gt_by_image, split_summary = load_split(dataset, "val")
    model = keras.models.load_model(str(model_path), compile=False)
    raw = np.asarray(model.predict(x_val, batch_size=8, verbose=1), dtype=np.float64)
    if raw.ndim != 4 or raw.shape[-1] != 5:
        raise RuntimeError(f"Expected model output [N, 32, 40, 5], got {raw.shape}")

    # Channel 0 is objectness logits; channels 1-4 are box-parameter logits.
    objectness_logits = raw[..., 0]
    objectness_scores = 1.0 / (1.0 + np.exp(-np.clip(objectness_logits, -40, 40)))
    box_logits = raw[..., 1:5]
    box_values = 1.0 / (1.0 + np.exp(-np.clip(box_logits, -40, 40)))

    per_image = []
    for index in range(len(raw)):
        logits = objectness_logits[index]
        scores = objectness_scores[index]
        per_image.append({
            "index": index,
            "ground_truth_boxes": int(len(gt_by_image[index])),
            "max_objectness_logit": float(logits.max()),
            "mean_objectness_logit": float(logits.mean()),
            "std_objectness_logit": float(logits.std()),
            "max_objectness_score": float(scores.max()),
            "cells_score_ge_0_50": int(np.count_nonzero(scores >= 0.50)),
            "cells_score_ge_0_60": int(np.count_nonzero(scores >= 0.60)),
            "cells_score_ge_0_75": int(np.count_nonzero(scores >= 0.75)),
        })

    result = {
        "purpose": "host-side diagnostic for output collapse; not a model quality score",
        "model": str(model_path),
        "dataset": str(dataset),
        "validation_split": split_summary,
        "model_output_shape": list(raw.shape),
        "parameter_count": int(model.count_params()),
        "raw_objectness_logits": describe(objectness_logits),
        "sigmoid_objectness_scores": describe(objectness_scores),
        "box_parameter_logits_by_channel": {
            name: describe(box_logits[..., i])
            for i, name in enumerate(("x_offset", "y_offset", "width", "height"))
        },
        "sigmoid_box_parameters_by_channel": {
            name: describe(box_values[..., i])
            for i, name in enumerate(("x_offset", "y_offset", "width", "height"))
        },
        "per_image_objectness": {
            "max_score": describe(np.array([r["max_objectness_score"] for r in per_image])),
            "mean_logit": describe(np.array([r["mean_objectness_logit"] for r in per_image])),
            "logit_std": describe(np.array([r["std_objectness_logit"] for r in per_image])),
            "cells_score_ge_0_50_total": int(sum(r["cells_score_ge_0_50"] for r in per_image)),
            "cells_score_ge_0_60_total": int(sum(r["cells_score_ge_0_60"] for r in per_image)),
            "cells_score_ge_0_75_total": int(sum(r["cells_score_ge_0_75"] for r in per_image)),
        },
        "interpretation_guidance": [
            "If objectness-logit standard deviation and per-image spread are tiny, the objectness head is nearly constant.",
            "If box-channel standard deviations are tiny, predicted box parameters are nearly constant too.",
            "This diagnostic alone does not establish why collapse happened; compare saved training history and loss curves before changing the trainer.",
        ],
    }
    if report_path:
        try:
            training_report = json.loads(report_path.read_text(encoding="utf-8"))
            result["training_report_path"] = str(report_path)
            result["training_config"] = training_report.get("training_config")
            result["loss_config"] = training_report.get("loss")
            result["training_history"] = training_report.get("history", training_report.get("training_history"))
            result["training_report_top_level_keys"] = sorted(training_report.keys())
        except (json.JSONDecodeError, OSError) as exc:
            result["training_report_read_error"] = str(exc)

    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(result, indent=2))
    print(f"Saved diagnostic: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
