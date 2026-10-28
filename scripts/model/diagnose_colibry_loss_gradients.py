#!/usr/bin/env python3
"""Measure trainer-matched loss-component gradients on a small fixed subset.

Diagnostic only: does not train, save weights, export, or access the physical board.
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


def norm(gradients: list[Any]) -> float:
    present = [g for g in gradients if g is not None]
    return float(tf.linalg.global_norm(present).numpy()) if present else 0.0


def stats(tensor: tf.Tensor) -> dict[str, float | int]:
    a = np.asarray(tensor.numpy(), dtype=np.float64).reshape(-1)
    return {
        "count": int(a.size),
        "mean_abs": float(np.mean(np.abs(a))) if a.size else 0.0,
        "max_abs": float(np.max(np.abs(a))) if a.size else 0.0,
        "std": float(np.std(a)) if a.size else 0.0,
    }


def components(y_true: tf.Tensor, y_pred: tf.Tensor) -> dict[str, tf.Tensor]:
    """Reproduce the four terms in hard_negative_giou_detection_loss."""
    obj = y_true[..., 0]
    logits = y_pred[..., 0]
    bce = tf.nn.sigmoid_cross_entropy_with_logits(labels=obj, logits=logits)

    counts = tf.reduce_sum(obj, axis=[1, 2])
    positive_images = tf.cast(counts > 0.0, tf.float32)
    pos_per_image = tf.reduce_sum(bce * obj, axis=[1, 2]) / tf.maximum(counts, 1.0)
    positive_bce = tf.reduce_sum(pos_per_image * positive_images) / tf.maximum(
        tf.reduce_sum(positive_images), 1.0
    )

    neg_bce_cells = tf.reshape(
        bce * (1.0 - obj),
        [tf.shape(bce)[0], detector.GRID_ROWS * detector.GRID_COLS],
    )
    k = min(detector.HARD_NEGATIVE_CELLS_PER_IMAGE, detector.GRID_ROWS * detector.GRID_COLS - 1)
    hard_negative_bce = tf.reduce_mean(
        tf.math.top_k(neg_bce_cells, k=k, sorted=False).values
    )

    true_v = y_true[..., 1:5]
    pred_v = tf.sigmoid(y_pred[..., 1:5])
    rows = tf.cast(tf.range(detector.GRID_ROWS)[None, :, None], tf.float32)
    cols = tf.cast(tf.range(detector.GRID_COLS)[None, None, :], tf.float32)
    tx, ty = (cols + true_v[..., 0]) / detector.GRID_COLS, (rows + true_v[..., 1]) / detector.GRID_ROWS
    tw, th = true_v[..., 2], true_v[..., 3]
    px, py = (cols + pred_v[..., 0]) / detector.GRID_COLS, (rows + pred_v[..., 1]) / detector.GRID_ROWS
    pw, ph = pred_v[..., 2], pred_v[..., 3]

    tx1, ty1 = tf.clip_by_value(tx - tw / 2, 0., 1.), tf.clip_by_value(ty - th / 2, 0., 1.)
    tx2, ty2 = tf.clip_by_value(tx + tw / 2, 0., 1.), tf.clip_by_value(ty + th / 2, 0., 1.)
    px1, py1 = tf.clip_by_value(px - pw / 2, 0., 1.), tf.clip_by_value(py - ph / 2, 0., 1.)
    px2, py2 = tf.clip_by_value(px + pw / 2, 0., 1.), tf.clip_by_value(py + ph / 2, 0., 1.)
    intersection = (
        tf.maximum(tf.minimum(tx2, px2) - tf.maximum(tx1, px1), 0.)
        * tf.maximum(tf.minimum(ty2, py2) - tf.maximum(ty1, py1), 0.)
    )
    area_true, area_pred = (tx2 - tx1) * (ty2 - ty1), (px2 - px1) * (py2 - py1)
    union = area_true + area_pred - intersection
    iou = intersection / (union + 1e-7)
    enclosing = (
        (tf.maximum(tx2, px2) - tf.minimum(tx1, px1))
        * (tf.maximum(ty2, py2) - tf.minimum(ty1, py1))
    )
    giou = iou - (enclosing - union) / (enclosing + 1e-7)
    positive_count = tf.maximum(tf.reduce_sum(obj), 1.0)
    giou_loss = tf.reduce_sum((1.0 - giou) * obj) / positive_count

    error = tf.abs(pred_v - true_v)
    delta = 0.1
    huber = tf.where(error <= delta, 0.5 * tf.square(error), delta * (error - 0.5 * delta))
    coordinate_count = tf.maximum(tf.reduce_sum(obj) * 4.0, 1.0)
    coordinate_loss = tf.reduce_sum(huber * obj[..., None]) / coordinate_count

    return {
        "positive_objectness_bce": positive_bce,
        "hard_negative_objectness_bce": hard_negative_bce,
        "giou_box_loss": giou_loss,
        "coordinate_huber_loss": coordinate_loss,
    }


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--dataset", type=Path, required=True)
    parser.add_argument("--model", type=Path, required=True)
    parser.add_argument("--positive-images", type=int, default=16)
    parser.add_argument("--empty-images", type=int, default=16)
    parser.add_argument("--output", type=Path, default=None,
                        help="Optional JSON path. Existing files are never overwritten.")
    args = parser.parse_args()
    if args.positive_images < 1 or args.empty_images < 1:
        parser.error("positive-images and empty-images must be at least 1")

    dataset = args.dataset.expanduser().resolve()
    model_path = args.model.expanduser().resolve()
    output_path = args.output.expanduser().resolve() if args.output else None
    if output_path and output_path.exists():
        parser.error(f"Refusing to overwrite existing report: {output_path}")

    x, y, gt_by_image, _ = detector.load_split(dataset, "train")
    positive_ids = [i for i, gt in enumerate(gt_by_image) if len(gt)]
    empty_ids = [i for i, gt in enumerate(gt_by_image) if not len(gt)]
    if len(positive_ids) < args.positive_images or len(empty_ids) < args.empty_images:
        parser.error("Not enough positive or known-empty training images")

    # Match the seed and selection logic used by diagnose_colibry_loss_components.py.
    rng = np.random.default_rng(42)
    indices = sorted(
        rng.choice(positive_ids, args.positive_images, replace=False).tolist()
        + rng.choice(empty_ids, args.empty_images, replace=False).tolist()
    )
    xb, yb = tf.convert_to_tensor(x[indices]), tf.convert_to_tensor(y[indices])
    model = keras.models.load_model(model_path, compile=False)
    expected = (detector.GRID_ROWS, detector.GRID_COLS, detector.VALUES_PER_CELL)
    if tuple(model.output_shape[1:]) != expected:
        parser.error(f"Unexpected model output shape {model.output_shape}; expected (None, {expected})")

    head = model.get_layer("grid_predictions")
    head_ids = {id(v) for v in head.trainable_variables}
    variables = model.trainable_variables
    backbone_vars = [v for v in variables if id(v) not in head_ids]
    head_vars = [v for v in variables if id(v) in head_ids]

    with tf.GradientTape(persistent=True) as tape:
        raw = model(xb, training=False)
        loss_terms = components(yb, raw)

    # Compare whether the positive and hard-negative objectness terms push
    # shared parameters in similar or opposing directions.
    pos_grads = tape.gradient(loss_terms["positive_objectness_bce"], variables)
    neg_grads = tape.gradient(loss_terms["hard_negative_objectness_bce"], variables)

    def cosine_for(selected_vars: list[Any], left: list[Any], right: list[Any]) -> dict[str, Any]:
        left_by_id = {id(v): g for v, g in zip(variables, left)}
        right_by_id = {id(v): g for v, g in zip(variables, right)}
        lg = [left_by_id[id(v)] for v in selected_vars if left_by_id[id(v)] is not None and right_by_id[id(v)] is not None]
        rg = [right_by_id[id(v)] for v in selected_vars if left_by_id[id(v)] is not None and right_by_id[id(v)] is not None]
        if not lg:
            return {"cosine_similarity": None, "note": "No shared gradient tensors"}
        lflat = tf.concat([tf.reshape(g, [-1]) for g in lg], axis=0)
        rflat = tf.concat([tf.reshape(g, [-1]) for g in rg], axis=0)
        denom = tf.linalg.norm(lflat) * tf.linalg.norm(rflat)
        cosine = tf.where(denom > 0, tf.reduce_sum(lflat * rflat) / denom, tf.constant(0.0))
        return {
            "cosine_similarity": float(cosine.numpy()),
            "left_gradient_norm": float(tf.linalg.norm(lflat).numpy()),
            "right_gradient_norm": float(tf.linalg.norm(rflat).numpy()),
            "interpretation_guide": "near +1 = aligned; near 0 = mostly orthogonal; near -1 = opposing",
        }

    objectness_gradient_alignment = {
        "shared_backbone": cosine_for(backbone_vars, pos_grads, neg_grads),
        "output_head": cosine_for(head_vars, pos_grads, neg_grads),
        "all_trainable": cosine_for(variables, pos_grads, neg_grads),
    }

    report: dict[str, Any] = {
        "purpose": "gradient diagnosis only; no optimizer step or model modification",
        "positive_vs_hard_negative_gradient_alignment": objectness_gradient_alignment,
        "model": str(model_path),
        "subset": {
            "images": len(indices),
            "positive_images": sum(bool(len(gt_by_image[i])) for i in indices),
            "known_empty_images": sum(not bool(len(gt_by_image[i])) for i in indices),
            "positive_target_cells": int(np.sum(y[indices][..., 0] > 0.5)),
            "negative_target_cells": int(np.sum(y[indices][..., 0] <= 0.5)),
            "selection_seed": 42,
            "indices_in_sorted_training_split": indices,
        },
        "components": {},
        "notes": [
            "Loss formulas mirror the trainer: positive BCE averaged over positive images, top-64 hard-negative BCE per image, positive-cell GIoU, and positive-cell coordinate Huber.",
            "Gradient norms are evaluated at the saved weights. No gradients are applied.",
            "Norm comparisons help locate weak gradient paths but do not alone prove the root cause.",
        ],
    }

    for name, value in loss_terms.items():
        grads = tape.gradient(value, variables)
        by_var = {id(v): g for v, g in zip(variables, grads)}
        backbone_grads = [by_var[id(v)] for v in backbone_vars]
        head_grads = [by_var[id(v)] for v in head_vars]
        layer_norms = {}
        for layer in model.layers:
            layer_variables = layer.trainable_variables
            if layer_variables:
                layer_norms[layer.name] = norm([by_var.get(id(v)) for v in layer_variables])
        item: dict[str, Any] = {
            "loss_value": float(value.numpy()),
            "gradient_norm_all_trainable": norm(grads),
            "gradient_norm_shared_backbone": norm(backbone_grads),
            "gradient_norm_output_head": norm(head_grads),
            "per_layer_gradient_norm": layer_norms,
            "missing_gradient_variables": [v.name for v, g in zip(variables, grads) if g is None],
        }
        for v in head_vars:
            g = by_var[id(v)]
            if g is not None:
                item[f"output_head_gradient_stats_{v.name}"] = stats(g)
        report["components"][name] = item

    del tape
    report["gradient_norms_finite"] = all(
        np.isfinite(c["gradient_norm_all_trainable"])
        and np.isfinite(c["gradient_norm_shared_backbone"])
        and np.isfinite(c["gradient_norm_output_head"])
        for c in report["components"].values()
    )
    rendered = json.dumps(report, indent=2)
    print(rendered)
    if output_path:
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(rendered + "\n", encoding="utf-8")
        print(f"Saved report: {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
