#!/usr/bin/env python3
"""Export the trained Keras 3 CNN as a legacy tf_keras HDF5 model.

This compatibility copy is intended for Colibry NPU compiler versions whose
PTQ loader uses legacy tf_keras. The source .keras model is never modified.
Weights are copied layer-for-layer and a numerical parity check is performed.
"""
from __future__ import annotations

import argparse
import hashlib
import json
from pathlib import Path
from typing import Any

import numpy as np
import tensorflow as tf
import tf_keras


ROOT = Path(__file__).resolve().parents[3]
DEFAULT_SOURCE = (
    ROOT / "projects/own_dataset_person_count_cnn/models/host_experiment/own_count_cnn.keras"
)
DEFAULT_OUTPUT = Path.home() / "models/edgeguard_own_count/own_count_cnn_legacy.h5"
EXPECTED_INPUT = (None, 30, 40, 1)
EXPECTED_OUTPUT = (None, 3)
EXPECTED_TRAINABLE_LAYERS = ["conv2d", "class_logits"]


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def trainable_layer_names(model: Any) -> list[str]:
    return [layer.name for layer in model.layers if layer.trainable_weights]


def build_legacy_model() -> tf_keras.Model:
    """Rebuild the same layer topology using the compiler's legacy Keras API."""
    model = tf_keras.Sequential(
        [
            tf_keras.layers.InputLayer(
                input_shape=(30, 40, 1),
                name="grayscale_image",
            ),
            tf_keras.layers.Conv2D(
                filters=4,
                kernel_size=(3, 3),
                padding="same",
                activation="relu",
                name="conv2d",
            ),
            tf_keras.layers.Flatten(name="flatten"),
            tf_keras.layers.Dense(
                units=3,
                activation="linear",
                name="class_logits",
            ),
        ],
        name="edgeguard_own_count_cnn_legacy",
    )
    return model


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--source", type=Path, default=DEFAULT_SOURCE)
    parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    args = parser.parse_args()

    source = args.source.expanduser().resolve()
    output = args.output.expanduser().resolve()
    if not source.is_file():
        raise FileNotFoundError(f"Source Keras model not found: {source}")
    if output.suffix.lower() not in {".h5", ".hdf5"}:
        raise ValueError("Output must use .h5 or .hdf5 extension")

    print("TensorFlow:", tf.__version__)
    print("tf_keras:", getattr(tf_keras, "__version__", "unknown"))
    print("Source:", source)
    source_model = tf.keras.models.load_model(source, compile=False)

    if tuple(source_model.input_shape) != EXPECTED_INPUT:
        raise ValueError(
            f"Unexpected source input shape: {source_model.input_shape}; "
            f"expected {EXPECTED_INPUT}"
        )
    if tuple(source_model.output_shape) != EXPECTED_OUTPUT:
        raise ValueError(
            f"Unexpected source output shape: {source_model.output_shape}; "
            f"expected {EXPECTED_OUTPUT}"
        )
    source_layers = trainable_layer_names(source_model)
    if source_layers != EXPECTED_TRAINABLE_LAYERS:
        raise ValueError(
            f"Expected trainable layers {EXPECTED_TRAINABLE_LAYERS}, got {source_layers}"
        )

    legacy_model = build_legacy_model()
    source_weights = source_model.get_weights()
    target_weights = legacy_model.get_weights()

    if len(source_weights) != len(target_weights):
        raise ValueError(
            f"Weight array count mismatch: source={len(source_weights)}, "
            f"legacy={len(target_weights)}"
        )
    for index, (source_weight, target_weight) in enumerate(
        zip(source_weights, target_weights)
    ):
        if source_weight.shape != target_weight.shape:
            raise ValueError(
                f"Weight {index} shape mismatch: {source_weight.shape} vs "
                f"{target_weight.shape}"
            )

    legacy_model.set_weights(source_weights)
    if trainable_layer_names(legacy_model) != EXPECTED_TRAINABLE_LAYERS:
        raise RuntimeError("Legacy model does not preserve the two trainable layers")
    if legacy_model.count_params() != source_model.count_params():
        raise RuntimeError("Parameter count changed during compatibility conversion")

    # Compare both frameworks on the same deterministic test input before saving.
    rng = np.random.default_rng(20261116)
    test_input = rng.random((2, 30, 40, 1), dtype=np.float32)
    source_logits = np.asarray(source_model(test_input, training=False))
    legacy_logits = np.asarray(legacy_model(test_input, training=False))
    max_abs_diff = float(np.max(np.abs(source_logits - legacy_logits)))
    np.testing.assert_allclose(
        source_logits,
        legacy_logits,
        rtol=1e-5,
        atol=1e-5,
        err_msg="Legacy model predictions differ from source model",
    )

    output.parent.mkdir(parents=True, exist_ok=True)
    legacy_model.save(str(output), include_optimizer=False, save_format="h5")

    # Confirm the artifact can be read by the legacy loader used by the compiler.
    reloaded = tf_keras.models.load_model(str(output), compile=False)
    if tuple(reloaded.input_shape) != EXPECTED_INPUT:
        raise RuntimeError(f"Reloaded input shape changed: {reloaded.input_shape}")
    if tuple(reloaded.output_shape) != EXPECTED_OUTPUT:
        raise RuntimeError(f"Reloaded output shape changed: {reloaded.output_shape}")
    if reloaded.count_params() != source_model.count_params():
        raise RuntimeError("Reloaded parameter count differs from source")
    for index, (expected, actual) in enumerate(
        zip(legacy_model.get_weights(), reloaded.get_weights())
    ):
        np.testing.assert_array_equal(
            expected, actual, err_msg=f"Saved/reloaded weight {index} changed"
        )

    reloaded_logits = np.asarray(reloaded(test_input, training=False))
    np.testing.assert_allclose(
        source_logits,
        reloaded_logits,
        rtol=1e-5,
        atol=1e-5,
        err_msg="Reloaded HDF5 predictions differ from source model",
    )

    report = {
        "source_model": str(source),
        "source_sha256": sha256_file(source),
        "legacy_model": str(output),
        "legacy_model_sha256": sha256_file(output),
        "tensorflow_version": tf.__version__,
        "tf_keras_version": getattr(tf_keras, "__version__", "unknown"),
        "input_shape": list(reloaded.input_shape),
        "output_shape": list(reloaded.output_shape),
        "trainable_layers": trainable_layer_names(reloaded),
        "trainable_parameters": int(reloaded.count_params()),
        "source_prediction_max_absolute_difference": max_abs_diff,
        "saved_reloaded_prediction_max_absolute_difference": float(
            np.max(np.abs(source_logits - reloaded_logits))
        ),
        "source_weights_preserved": True,
        "legacy_hdf5_reload_passed": True,
    }
    report_path = output.with_suffix(".compatibility.json")
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")

    print("\n=== LEGACY COMPATIBILITY EXPORT PASSED ===")
    print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
