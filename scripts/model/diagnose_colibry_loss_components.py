#!/usr/bin/env python3
"""Break down detector loss and output behavior on a balanced training subset.

Diagnostic only: loads a saved Keras model and does not train, export, or access
the physical Colibry board.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import tensorflow as tf
import tf_keras as keras

import train_colibry_person_detector as detector


def stats(values: np.ndarray) -> dict[str, float]:
    a = np.asarray(values, dtype=np.float64).reshape(-1)
    q = np.quantile(a, [0, .5, .9, .99, 1])
    return {
        "min": float(q[0]), "median": float(q[1]), "p90": float(q[2]),
        "p99": float(q[3]), "max": float(q[4]), "mean": float(a.mean()),
    }


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dataset", type=Path, required=True)
    p.add_argument("--model", type=Path, required=True)
    p.add_argument("--positive-images", type=int, default=16)
    p.add_argument("--empty-images", type=int, default=16)
    args = p.parse_args()

    x, y, gt_by_image, split_summary = detector.load_split(args.dataset.expanduser().resolve(), "train")
    positive = [i for i, gt in enumerate(gt_by_image) if len(gt)]
    empty = [i for i, gt in enumerate(gt_by_image) if not len(gt)]
    if len(positive) < args.positive_images or len(empty) < args.empty_images:
        p.error("Not enough positive or empty training images")
    rng = np.random.default_rng(42)
    chosen = sorted(
        rng.choice(positive, args.positive_images, replace=False).tolist()
        + rng.choice(empty, args.empty_images, replace=False).tolist()
    )
    x_sub, y_sub = x[chosen], y[chosen]
    positive_image = np.asarray([bool(len(gt_by_image[i])) for i in chosen])
    model = keras.models.load_model(args.model.expanduser().resolve(), compile=False)
    raw = model.predict(x_sub, batch_size=8, verbose=0)
    logits = raw[..., 0]
    target = y_sub[..., 0]
    bce = tf.nn.sigmoid_cross_entropy_with_logits(labels=target, logits=logits).numpy()
    pos_mask = target > 0.5
    neg_mask = ~pos_mask
    probs = 1.0 / (1.0 + np.exp(-np.clip(logits, -30, 30)))

    per_image_positive = []
    per_image_negative = []
    for i in range(len(chosen)):
        pvals = bce[i][pos_mask[i]]
        nvals = bce[i][neg_mask[i]]
        per_image_positive.append(float(pvals.mean()) if pvals.size else None)
        per_image_negative.append(float(nvals.mean()) if nvals.size else None)

    report = {
        "purpose": "loss/output diagnosis only; not validation metrics",
        "model": str(args.model.expanduser().resolve()),
        "selected_train_images": len(chosen),
        "positive_images": int(positive_image.sum()),
        "known_empty_images": int((~positive_image).sum()),
        "positive_target_cells": int(pos_mask.sum()),
        "negative_target_cells": int(neg_mask.sum()),
        "objectness_logits": {
            "positive_target_cells": stats(logits[pos_mask]),
            "negative_target_cells_in_positive_images": stats(logits[neg_mask & np.broadcast_to(positive_image[:, None, None], logits.shape)]),
            "all_negative_target_cells": stats(logits[neg_mask]),
        },
        "objectness_probabilities": {
            "positive_target_cells": stats(probs[pos_mask]),
            "negative_target_cells": stats(probs[neg_mask]),
            "max_per_image_positive": stats(np.max(probs[positive_image], axis=(1, 2))),
            "max_per_image_empty": stats(np.max(probs[~positive_image], axis=(1, 2))),
        },
        "objectness_bce": {
            "positive_cells_mean": float(bce[pos_mask].mean()) if pos_mask.any() else None,
            "negative_cells_mean": float(bce[neg_mask].mean()) if neg_mask.any() else None,
            "per_image_positive_cell_loss": per_image_positive,
            "per_image_negative_cell_loss": per_image_negative,
        },
        "target_contract": {
            "target_shape": list(y_sub.shape),
            "positive_cells_per_image": [int(v) for v in pos_mask.sum(axis=(1, 2))],
            "positive_images_order": [bool(v) for v in positive_image],
        },
        "note": "Known-empty images are valid negative examples for a person-only detector, including animals and furniture. Do not deploy this diagnostic model.",
    }
    print(json.dumps(report, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
