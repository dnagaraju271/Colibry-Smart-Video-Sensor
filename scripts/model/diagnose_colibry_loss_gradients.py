#!/usr/bin/env python3
"""Measure detector loss-component and output-gradient magnitudes on a small subset.

Diagnostic only. Does not train, save, export, or deploy a model.
"""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np
import tensorflow as tf
import tf_keras as keras

import train_colibry_person_detector as detector


def scalar_stats(value: tf.Tensor) -> dict[str, float]:
    a = np.asarray(value.numpy(), dtype=np.float64).reshape(-1)
    return {
        "mean": float(a.mean()),
        "min": float(a.min()),
        "max": float(a.max()),
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--positive-images", type=int, default=8)
    parser.add_argument("--empty-images", type=int, default=8)
    args = parser.parse_args()

    x, y, gt_by_image, split_summary = detector.load_split(args.dataset.expanduser().resolve(), "train")
    positive = [i for i, gt in enumerate(gt_by_image) if len(gt)]
    empty = [i for i, gt in enumerate(gt_by_image) if not len(gt)]
    if len(positive) < args.positive_images or len(empty) < args.empty_images:
        parser.error("Not enough positive or empty training images")
    rng = np.random.default_rng(43)
    indices = sorted(
        rng.choice(positive, args.positive_images, replace=False).tolist()
        + rng.choice(empty, args.empty_images, replace=False).tolist()
    )
    xb = tf.convert_to_tensor(x[indices], dtype=tf.float32)
    yb = tf.convert_to_tensor(y[indices], dtype=tf.float32)
    model = keras.models.load_model(args.model.expanduser().resolve(), compile=False)

    with tf.GradientTape(persistent=True) as tape:
        raw = model(xb, training=False)
        tape.watch(raw)
        target_obj = yb[..., 0]
        logits = raw[..., 0]
        bce = tf.nn.sigmoid_cross_entropy_with_logits(labels=target_obj, logits=logits)
        positive_mask = target_obj > 0.5
        negative_mask = tf.logical_not(positive_mask)
        pos_bce = tf.reduce_sum(tf.where(positive_mask, bce, 0.0)) / tf.maximum(
            tf.reduce_sum(tf.cast(positive_mask, tf.float32)), 1.0
        )
        neg_bce = tf.reduce_sum(tf.where(negative_mask, bce, 0.0)) / tf.maximum(
            tf.reduce_sum(tf.cast(negative_mask, tf.float32)), 1.0
        )

        true_v = yb[..., 1:5]
        pred_v = tf.sigmoid(raw[..., 1:5])
        rows = tf.cast(tf.range(detector.GRID_ROWS)[None, :, None], tf.float32)
        cols = tf.cast(tf.range(detector.GRID_COLS)[None, None, :], tf.float32)
        tx = (cols + true_v[..., 0]) / detector.GRID_COLS
        ty = (rows + true_v[..., 1]) / detector.GRID_ROWS
        tw, th = true_v[..., 2], true_v[..., 3]
        px = (cols + pred_v[..., 0]) / detector.GRID_COLS
        py = (rows + pred_v[..., 1]) / detector.GRID_ROWS
        pw, ph = pred_v[..., 2], pred_v[..., 3]
        tx1, ty1 = tf.clip_by_value(tx - tw / 2, 0., 1.), tf.clip_by_value(ty - th / 2, 0., 1.)
        tx2, ty2 = tf.clip_by_value(tx + tw / 2, 0., 1.), tf.clip_by_value(ty + th / 2, 0., 1.)
        px1, py1 = tf.clip_by_value(px - pw / 2, 0., 1.), tf.clip_by_value(py - ph / 2, 0., 1.)
        px2, py2 = tf.clip_by_value(px + pw / 2, 0., 1.), tf.clip_by_value(py + ph / 2, 0., 1.)
        intersection = tf.maximum(tf.minimum(tx2, px2) - tf.maximum(tx1, px1), 0.) * tf.maximum(
            tf.minimum(ty2, py2) - tf.maximum(ty1, py1), 0.
        )
        area_true = (tx2 - tx1) * (ty2 - ty1)
        area_pred = (px2 - px1) * (py2 - py1)
        union = area_true + area_pred - intersection
        iou = intersection / (union + 1e-7)
        enclosing = (tf.maximum(tx2, px2) - tf.minimum(tx1, px1)) * (
            tf.maximum(ty2, py2) - tf.minimum(ty1, py1)
        )
        giou = iou - (enclosing - union) / (enclosing + 1e-7)
        pos_float = tf.cast(positive_mask, tf.float32)
        giou_loss = tf.reduce_sum((1. - giou) * pos_float) / tf.maximum(tf.reduce_sum(pos_float), 1.)
        error = tf.abs(pred_v - true_v)
        delta = 0.1
        huber = tf.where(error <= delta, .5 * tf.square(error), delta * (error - .5 * delta))
        coord_loss = tf.reduce_sum(huber * pos_float[..., None]) / tf.maximum(
            tf.reduce_sum(pos_float) * 4., 1.
        )
        current_combined_loss = pos_bce + detector.NEGATIVE_LOSS_WEIGHT * neg_bce + detector.GIOU_WEIGHT * giou_loss + coord_loss

    components = {
        "positive_objectness_bce": pos_bce,
        "negative_objectness_bce": neg_bce,
        "giou_loss": giou_loss,
        "coordinate_huber_loss": coord_loss,
        "combined_reconstructed_loss": current_combined_loss,
    }
    gradients = {}
    for name, component in components.items():
        grad = tape.gradient(component, raw)
        if grad is None:
            gradients[name] = {"gradient_norm": 0.0, "objectness_channel_abs_mean": 0.0, "box_channels_abs_mean": 0.0}
        else:
            gradients[name] = {
                "gradient_norm": float(tf.linalg.global_norm([grad]).numpy()),
                "objectness_channel_abs_mean": float(tf.reduce_mean(tf.abs(grad[..., 0])).numpy()),
                "box_channels_abs_mean": float(tf.reduce_mean(tf.abs(grad[..., 1:5])).numpy()),
            }
    del tape

    report = {
        "purpose": "diagnostic only; not validation metrics",
        "model": str(args.model.expanduser().resolve()),
        "subset_images": len(indices),
        "positive_images": int(sum(bool(len(gt_by_image[i])) for i in indices)),
        "known_empty_images": int(sum(not bool(len(gt_by_image[i])) for i in indices)),
        "positive_cells": int(tf.reduce_sum(tf.cast(positive_mask, tf.int32)).numpy()),
        "negative_cells": int(tf.reduce_sum(tf.cast(negative_mask, tf.int32)).numpy()),
        "loss_components": {name: float(value.numpy()) for name, value in components.items()},
        "output_gradient_magnitudes": gradients,
        "interpretation_note": "Compare component magnitudes and gradients; this diagnostic does not by itself establish a correct fix. Do not deploy the diagnostic model.",
    }
    print(json.dumps(report, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
