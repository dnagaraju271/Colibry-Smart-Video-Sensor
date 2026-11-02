#!/usr/bin/env python3
"""Check objectness-gradient alignment across deterministic mini-batches.

This is a read-only diagnostic. It loads a saved model and training split, does
not run optimizer steps, does not write model files, and does not access hardware.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Any

import numpy as np
import tensorflow as tf
import tf_keras as keras

import train_colibry_person_detector as detector


def global_norm(grads: list[Any]) -> float:
    values = [g for g in grads if g is not None]
    return float(tf.linalg.global_norm(values).numpy()) if values else 0.0


def flatten_gradients(variables: list[Any], gradients: list[Any]) -> tf.Tensor:
    by_id = {id(v): g for v, g in zip(variables, gradients)}
    chunks = [
        tf.reshape(by_id[id(v)], [-1])
        for v in variables
        if by_id.get(id(v)) is not None
    ]
    return tf.concat(chunks, axis=0) if chunks else tf.zeros([0], tf.float32)


def cosine_and_norms(left: tf.Tensor, right: tf.Tensor) -> dict[str, Any]:
    left_norm = tf.linalg.norm(left)
    right_norm = tf.linalg.norm(right)
    denom = left_norm * right_norm
    cosine = tf.where(
        denom > 0,
        tf.reduce_sum(left * right) / denom,
        tf.constant(0.0, tf.float32),
    )
    return {
        "cosine_similarity": float(cosine.numpy()),
        "positive_gradient_norm": float(left_norm.numpy()),
        "hard_negative_gradient_norm": float(right_norm.numpy()),
        "positive_norm_fraction_of_combined": None,
    }


def objectness_losses(y: tf.Tensor, raw: tf.Tensor) -> tuple[tf.Tensor, tf.Tensor]:
    target = y[..., 0]
    logits = raw[..., 0]
    bce = tf.nn.sigmoid_cross_entropy_with_logits(labels=target, logits=logits)

    counts = tf.reduce_sum(target, axis=[1, 2])
    positive_images = tf.cast(counts > 0.0, tf.float32)
    pos_per_image = tf.reduce_sum(bce * target, axis=[1, 2]) / tf.maximum(counts, 1.0)
    positive_loss = tf.reduce_sum(pos_per_image * positive_images) / tf.maximum(
        tf.reduce_sum(positive_images), 1.0
    )

    negatives = tf.reshape(
        bce * (1.0 - target),
        [tf.shape(bce)[0], detector.GRID_ROWS * detector.GRID_COLS],
    )
    k = min(detector.HARD_NEGATIVE_CELLS_PER_IMAGE, detector.GRID_ROWS * detector.GRID_COLS - 1)
    hard_negative_loss = tf.reduce_mean(tf.math.top_k(negatives, k=k, sorted=False).values)
    return positive_loss, hard_negative_loss


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--batches", type=int, default=6,
                        help="Number of deterministic batches to examine.")
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--output", type=Path, default=None,
                        help="Optional JSON report; existing files are never overwritten.")
    args = parser.parse_args()
    if args.batches < 2 or args.batch_size < 2:
        parser.error("batches must be >= 2 and batch-size must be >= 2")

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    output_path = args.output.expanduser().resolve() if args.output else None
    if output_path and output_path.exists():
        parser.error(f"Refusing to overwrite existing report: {output_path}")

    x, y, gt_by_image, _ = detector.load_split(dataset, "train")
    pos_ids = np.asarray([i for i, gt in enumerate(gt_by_image) if len(gt)], dtype=np.int64)
    empty_ids = np.asarray([i for i, gt in enumerate(gt_by_image) if not len(gt)], dtype=np.int64)
    if len(pos_ids) < args.batch_size or len(empty_ids) < args.batch_size:
        parser.error("Training split does not contain enough positive and empty images")

    # Select a reproducible pool, then form batches with positive and empty images.
    rng = np.random.default_rng(20261028)
    pool_size = args.batches * args.batch_size
    pos_pool = rng.choice(pos_ids, size=min(len(pos_ids), (pool_size + 1) // 2), replace=False)
    empty_pool = rng.choice(empty_ids, size=min(len(empty_ids), pool_size // 2), replace=False)
    combined = np.concatenate([pos_pool, empty_pool])
    rng.shuffle(combined)
    batches = [combined[i:i + args.batch_size] for i in range(0, len(combined), args.batch_size)]
    batches = [b for b in batches if len(b) == args.batch_size][:args.batches]
    if len(batches) < 2:
        parser.error("Could not form at least two full batches; reduce batch-size or batches")

    model = keras.models.load_model(model_path, compile=False)
    variables = model.trainable_variables
    head = model.get_layer("grid_predictions")
    head_ids = {id(v) for v in head.trainable_variables}
    backbone_vars = [v for v in variables if id(v) not in head_ids]
    head_vars = [v for v in variables if id(v) in head_ids]

    records = []
    for batch_number, indices in enumerate(batches, start=1):
        xb = tf.convert_to_tensor(x[indices], tf.float32)
        yb = tf.convert_to_tensor(y[indices], tf.float32)
        with tf.GradientTape(persistent=True) as tape:
            raw = model(xb, training=False)
            pos_loss, neg_loss = objectness_losses(yb, raw)
            combined_loss = pos_loss + detector.NEGATIVE_LOSS_WEIGHT * neg_loss
        pos_grads = tape.gradient(pos_loss, variables)
        neg_grads = tape.gradient(neg_loss, variables)
        combined_grads = tape.gradient(combined_loss, variables)
        del tape

        p_all = flatten_gradients(variables, pos_grads)
        n_all = flatten_gradients(variables, neg_grads)
        c_all = flatten_gradients(variables, combined_grads)
        p_back = flatten_gradients(backbone_vars, [dict(zip(map(id, variables), pos_grads)).get(id(v)) for v in backbone_vars])
        n_back = flatten_gradients(backbone_vars, [dict(zip(map(id, variables), neg_grads)).get(id(v)) for v in backbone_vars])
        p_head = flatten_gradients(head_vars, [dict(zip(map(id, variables), pos_grads)).get(id(v)) for v in head_vars])
        n_head = flatten_gradients(head_vars, [dict(zip(map(id, variables), neg_grads)).get(id(v)) for v in head_vars])

        records.append({
            "batch": batch_number,
            "training_split_indices": [int(i) for i in indices],
            "positive_images": int(sum(bool(len(gt_by_image[i])) for i in indices)),
            "known_empty_images": int(sum(not bool(len(gt_by_image[i])) for i in indices)),
            "positive_target_cells": int(np.sum(y[indices][..., 0] > 0.5)),
            "loss_values": {
                "positive_objectness_bce": float(pos_loss.numpy()),
                "hard_negative_objectness_bce": float(neg_loss.numpy()),
                "weighted_combined_objectness_loss": float(combined_loss.numpy()),
            },
            "gradient_alignment": {
                "all_trainable": cosine_and_norms(p_all, n_all),
                "shared_backbone": cosine_and_norms(p_back, n_back),
                "output_head": cosine_and_norms(p_head, n_head),
            },
            "gradient_norms": {
                "positive_all": global_norm(pos_grads),
                "hard_negative_all": global_norm(neg_grads),
                "combined_all": global_norm(combined_grads),
                "positive_backbone": global_norm([dict(zip(map(id, variables), pos_grads)).get(id(v)) for v in backbone_vars]),
                "hard_negative_backbone": global_norm([dict(zip(map(id, variables), neg_grads)).get(id(v)) for v in backbone_vars]),
                "combined_backbone": global_norm([dict(zip(map(id, variables), combined_grads)).get(id(v)) for v in backbone_vars]),
                "positive_head": global_norm([dict(zip(map(id, variables), pos_grads)).get(id(v)) for v in head_vars]),
                "hard_negative_head": global_norm([dict(zip(map(id, variables), neg_grads)).get(id(v)) for v in head_vars]),
                "combined_head": global_norm([dict(zip(map(id, variables), combined_grads)).get(id(v)) for v in head_vars]),
            },
        })

    cosines = [r["gradient_alignment"]["all_trainable"]["cosine_similarity"] for r in records]
    report = {
        "purpose": "read-only mini-batch objectness gradient alignment diagnostic",
        "model": str(model_path),
        "dataset": str(dataset),
        "configuration": {
            "seed": 20261028,
            "requested_batches": args.batches,
            "batch_size": args.batch_size,
            "actual_batches": len(records),
            "negative_loss_weight": detector.NEGATIVE_LOSS_WEIGHT,
        },
        "summary": {
            "all_trainable_cosine_min": float(np.min(cosines)),
            "all_trainable_cosine_median": float(np.median(cosines)),
            "all_trainable_cosine_max": float(np.max(cosines)),
            "batches_with_cosine_below_minus_0_9": int(np.sum(np.asarray(cosines) < -0.9)),
            "warning": "Gradient alignment depends on this model checkpoint, sample selection, and current top-k hard-negative cells. It does not prove a universal cause.",
        },
        "batches": records,
    }
    rendered = json.dumps(report, indent=2)
    print(rendered)
    if output_path:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(rendered + "\n", encoding="utf-8")
        print(f"Saved report: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
