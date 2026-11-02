#!/usr/bin/env python3
"""Read-only objectness loss-weight sweep for the Colibry detector.

Computes gradients of positive-cell and hard-negative objectness BCE separately,
then simulates their weighted sum for candidate negative-loss weights. No optimizer
step is performed and the saved model is never modified.
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


def obj_losses(y: tf.Tensor, raw: tf.Tensor) -> tuple[tf.Tensor, tf.Tensor]:
    target, logits = y[..., 0], raw[..., 0]
    bce = tf.nn.sigmoid_cross_entropy_with_logits(labels=target, logits=logits)
    counts = tf.reduce_sum(target, axis=[1, 2])
    has_pos = tf.cast(counts > 0.0, tf.float32)
    pos_per_image = tf.reduce_sum(bce * target, axis=[1, 2]) / tf.maximum(counts, 1.0)
    pos_loss = tf.reduce_sum(pos_per_image * has_pos) / tf.maximum(tf.reduce_sum(has_pos), 1.0)
    neg_bce = tf.reshape(bce * (1.0 - target), [tf.shape(bce)[0], -1])
    k = min(detector.HARD_NEGATIVE_CELLS_PER_IMAGE, detector.GRID_ROWS * detector.GRID_COLS - 1)
    neg_loss = tf.reduce_mean(tf.math.top_k(neg_bce, k=k, sorted=False).values)
    return pos_loss, neg_loss


def grad_for(tape: tf.GradientTape, loss: tf.Tensor, variables: list[Any]) -> list[tf.Tensor]:
    return [tf.zeros_like(v) if g is None else g for v, g in zip(variables, tape.gradient(loss, variables))]


def norm(grads: list[tf.Tensor]) -> float:
    return float(tf.linalg.global_norm(grads).numpy()) if grads else 0.0


def cosine(a: list[tf.Tensor], b: list[tf.Tensor]) -> float:
    aa = tf.concat([tf.reshape(v, [-1]) for v in a], axis=0)
    bb = tf.concat([tf.reshape(v, [-1]) for v in b], axis=0)
    denom = tf.linalg.norm(aa) * tf.linalg.norm(bb)
    return float(tf.where(denom > 0, tf.reduce_sum(aa * bb) / denom, 0.0).numpy())


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--dataset", type=Path, required=True)
    p.add_argument("--model", type=Path, required=True)
    p.add_argument("--batches", type=int, default=6)
    p.add_argument("--batch-size", type=int, default=8)
    p.add_argument("--weights", type=float, nargs="+", default=[0.1, 0.25, 0.5, 0.75, 1.0, 1.5, 2.0])
    p.add_argument("--output", type=Path, default=None)
    args = p.parse_args()
    if args.batches < 1 or args.batch_size < 2 or not args.weights:
        p.error("batches and weights must be positive; batch-size must be at least 2")
    if any(w <= 0 or not np.isfinite(w) for w in args.weights):
        p.error("all candidate weights must be finite and positive")
    out = args.output.expanduser().resolve() if args.output else None
    if out and out.exists():
        p.error(f"Refusing to overwrite existing report: {out}")

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    x, y, gt, _ = detector.load_split(dataset, "train")
    pos_ids = np.asarray([i for i, boxes in enumerate(gt) if len(boxes)], dtype=np.int64)
    empty_ids = np.asarray([i for i, boxes in enumerate(gt) if not len(boxes)], dtype=np.int64)
    pool_n = args.batches * args.batch_size
    rng = np.random.default_rng(20261028)
    pos = rng.choice(pos_ids, size=min(len(pos_ids), (pool_n + 1) // 2), replace=False)
    empty = rng.choice(empty_ids, size=min(len(empty_ids), pool_n // 2), replace=False)
    pool = np.concatenate([pos, empty])
    rng.shuffle(pool)
    batches = [pool[i:i + args.batch_size] for i in range(0, len(pool), args.batch_size)]
    batches = [b for b in batches if len(b) == args.batch_size][:args.batches]
    if not batches:
        p.error("Could not form a full batch from this dataset")

    model = keras.models.load_model(model_path, compile=False)
    variables = model.trainable_variables
    head = model.get_layer("grid_predictions")
    kernel, bias = head.trainable_variables
    kernel_i = next(i for i, v in enumerate(variables) if id(v) == id(kernel))
    bias_i = next(i for i, v in enumerate(variables) if id(v) == id(bias))
    all_records = []
    for batch_no, ids in enumerate(batches, 1):
        xb, yb = tf.convert_to_tensor(x[ids]), tf.convert_to_tensor(y[ids])
        with tf.GradientTape(persistent=True) as tape:
            raw = model(xb, training=False)
            pos_loss, neg_loss = obj_losses(yb, raw)
        gp = grad_for(tape, pos_loss, variables)
        gn = grad_for(tape, neg_loss, variables)
        del tape
        sweeps = []
        for weight in args.weights:
            gc = [a + tf.cast(weight, a.dtype) * b for a, b in zip(gp, gn)]
            sweeps.append({
                "negative_loss_weight": float(weight),
                "combined_all_trainable_norm": norm(gc),
                "combined_backbone_norm": norm([g for i, g in enumerate(gc) if i not in (kernel_i, bias_i)]),
                "combined_head_kernel_norm": norm([gc[kernel_i]]),
                "combined_objectness_kernel_channel_norm": float(tf.linalg.norm(gc[kernel_i][..., 0]).numpy()),
                "combined_box_kernel_channels_norm": float(tf.linalg.norm(gc[kernel_i][..., 1:]).numpy()),
                "combined_head_bias": [float(v) for v in gc[bias_i].numpy()],
                "combined_objectness_bias_gradient": float(gc[bias_i][0].numpy()),
                "combined_box_bias_gradient_norm": float(tf.linalg.norm(gc[bias_i][1:]).numpy()),
            })
        all_records.append({
            "batch": batch_no,
            "positive_images": int(sum(bool(len(gt[i])) for i in ids)),
            "known_empty_images": int(sum(not bool(len(gt[i])) for i in ids)),
            "positive_objectness_loss": float(pos_loss.numpy()),
            "hard_negative_objectness_loss": float(neg_loss.numpy()),
            "positive_gradient_norm_all": norm(gp),
            "negative_gradient_norm_all": norm(gn),
            "gradient_cosine_all": cosine(gp, gn),
            "positive_head_kernel_objectness_norm": float(tf.linalg.norm(gp[kernel_i][..., 0]).numpy()),
            "negative_head_kernel_objectness_norm": float(tf.linalg.norm(gn[kernel_i][..., 0]).numpy()),
            "positive_head_bias": [float(v) for v in gp[bias_i].numpy()],
            "negative_head_bias": [float(v) for v in gn[bias_i].numpy()],
            "weight_sweep": sweeps,
        })
    report = {
        "purpose": "read-only simulated gradient-weight sweep; no training or model updates",
        "model": str(model_path),
        "dataset": str(dataset),
        "seed": 20261028,
        "batches": len(all_records),
        "batch_size": args.batch_size,
        "weights_tested": args.weights,
        "interpretation_guardrail": "Choose a weight only after comparing validation precision, recall, empty-image false alarms, and loss curves in a fresh run. Gradient norms alone cannot establish the best training weight.",
        "records": all_records,
    }
    rendered = json.dumps(report, indent=2)
    print(rendered)
    if out:
        out.parent.mkdir(parents=True, exist_ok=True)
        out.write_text(rendered + "\n", encoding="utf-8")
        print(f"Saved report: {out}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
