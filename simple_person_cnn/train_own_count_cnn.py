#!/usr/bin/env python3
"""Train the EdgeGuard own-dataset 3-class, two-trainable-layer CNN.

Reads the selected OWN_Dataset ZIP directly. The archive is never extracted
or modified. Validation is an image-level stratified split and is preliminary:
related frames from the same source timestamp may cross the split.
"""
from __future__ import annotations

import argparse
import csv
import io
import json
import re
import zipfile
from collections import Counter
from pathlib import Path, PurePosixPath
from typing import Any

import numpy as np
import tensorflow as tf
from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ARCHIVE = ROOT / "datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip"
DEFAULT_OUTPUT = ROOT / "simple_person_cnn/model/own_count_experiment"
CLASS_NAMES = ["1_person", "2_person", "3_person"]
CLASS_TO_ID = {name: index for index, name in enumerate(CLASS_NAMES)}
IMAGE_WIDTH, IMAGE_HEIGHT, CHANNELS = 40, 30, 1


def source_group(source_filename: str) -> str:
    match = re.search(
        r"(WhatsApp Image \d{4}-\d{2}-\d{2} at \d{2}\.\d{2}\.\d{2})",
        source_filename,
    )
    return match.group(1) if match else source_filename


def load_archive(path: Path) -> tuple[np.ndarray, np.ndarray, list[dict[str, str]], int]:
    if not path.is_file():
        raise FileNotFoundError(f"Dataset archive not found: {path}")

    images: list[np.ndarray] = []
    labels: list[int] = []
    records: list[dict[str, str]] = []
    pending_zero_person_skipped = 0

    with zipfile.ZipFile(path) as archive:
        names = set(archive.namelist())
        manifests = [n for n in names if PurePosixPath(n).name.lower() == "manifest.csv"]
        if len(manifests) != 1:
            raise ValueError(f"Expected one manifest.csv, found {len(manifests)}")

        with archive.open(manifests[0]) as raw:
            reader = csv.DictReader(io.TextIOWrapper(raw, encoding="utf-8-sig", newline=""))
            required = {"source_filename", "visible_person_count", "output_file"}
            if not reader.fieldnames or not required.issubset(reader.fieldnames):
                raise ValueError(f"Manifest must contain columns: {sorted(required)}")

            seen_outputs: set[str] = set()
            for line_no, row in enumerate(reader, start=2):
                output_file = (row.get("output_file") or "").strip()
                source = (row.get("source_filename") or "").strip()
                if not output_file or not source:
                    raise ValueError(f"Manifest line {line_no}: missing source/output filename")
                if output_file in seen_outputs:
                    raise ValueError(f"Manifest line {line_no}: duplicate output {output_file}")
                seen_outputs.add(output_file)

                class_name = PurePosixPath(output_file).parts[0]
                count = int(row["visible_person_count"])

                # Zero-person originals are intentionally pending and must not
                # be imported into this 1/2/3-person experiment yet.
                if class_name == "pending_for_later_originals":
                    if count != 0:
                        raise ValueError(
                            f"Manifest line {line_no}: pending originals must have count 0"
                        )
                    pending_zero_person_skipped += 1
                    continue

                if class_name not in CLASS_TO_ID:
                    raise ValueError(f"Manifest line {line_no}: unsupported class {class_name}")
                if count != CLASS_TO_ID[class_name] + 1:
                    raise ValueError(
                        f"Manifest line {line_no}: count {count} conflicts with {class_name}"
                    )
                if output_file not in names:
                    raise FileNotFoundError(f"Image from manifest is missing: {output_file}")

                with Image.open(io.BytesIO(archive.read(output_file))) as image:
                    if image.size != (160, 120):
                        raise ValueError(
                            f"{output_file}: expected 160x120, got {image.size}"
                        )
                    gray = image.convert("L").resize(
                        (IMAGE_WIDTH, IMAGE_HEIGHT), Image.Resampling.LANCZOS
                    )
                    pixels = np.asarray(gray, dtype=np.float32) / 255.0

                images.append(pixels[..., None])
                labels.append(CLASS_TO_ID[class_name])
                records.append({
                    "output_file": output_file,
                    "source_filename": source,
                    "class_name": class_name,
                    "group": source_group(source),
                })

    if not images:
        raise RuntimeError("No labeled images found in the archive manifest")
    return (
        np.stack(images).astype(np.float32),
        np.asarray(labels, np.int32),
        records,
        pending_zero_person_skipped,
    )


def stratified_split(
    labels: np.ndarray, validation_fraction: float, seed: int
) -> tuple[np.ndarray, np.ndarray]:
    rng = np.random.default_rng(seed)
    train_indices: list[int] = []
    validation_indices: list[int] = []
    for class_id, class_name in enumerate(CLASS_NAMES):
        class_indices = np.flatnonzero(labels == class_id)
        if len(class_indices) < 2:
            raise ValueError(
                f"{class_name} has {len(class_indices)} images; at least two are required"
            )
        shuffled = rng.permutation(class_indices)
        val_count = max(1, int(round(len(shuffled) * validation_fraction)))
        val_count = min(val_count, len(shuffled) - 1)
        validation_indices.extend(shuffled[:val_count].tolist())
        train_indices.extend(shuffled[val_count:].tolist())
    return (
        rng.permutation(np.asarray(train_indices, dtype=np.int64)),
        rng.permutation(np.asarray(validation_indices, dtype=np.int64)),
    )


def build_model() -> tf.keras.Model:
    """Exactly two trainable layers: Conv2D and Dense (linear output logits)."""
    model = tf.keras.Sequential(
        [
            tf.keras.layers.Input(
                shape=(IMAGE_HEIGHT, IMAGE_WIDTH, CHANNELS), name="grayscale_image"
            ),
            tf.keras.layers.Conv2D(
                4, (3, 3), padding="same", activation="relu", name="conv2d"
            ),
            tf.keras.layers.Flatten(name="flatten"),
            # Linear logits keep the graph within a simple Conv/ReLU + Dense path.
            tf.keras.layers.Dense(3, activation="linear", name="class_logits"),
        ],
        name="edgeguard_own_count_cnn",
    )
    model.compile(
        optimizer=tf.keras.optimizers.Adam(learning_rate=1e-3),
        loss=tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True),
        metrics=[tf.keras.metrics.SparseCategoricalAccuracy(name="accuracy")],
    )
    return model


def distribution(labels: np.ndarray) -> dict[str, int]:
    return {
        name: int(np.sum(labels == class_id))
        for class_id, name in enumerate(CLASS_NAMES)
    }


def evaluate_classes(y_true: np.ndarray, logits: np.ndarray) -> dict[str, Any]:
    probabilities = tf.nn.softmax(logits, axis=1).numpy()
    predictions = np.argmax(probabilities, axis=1)
    matrix = np.zeros((len(CLASS_NAMES), len(CLASS_NAMES)), dtype=np.int64)
    for actual, predicted in zip(y_true, predictions):
        matrix[int(actual), int(predicted)] += 1

    per_class: dict[str, Any] = {}
    for class_id, name in enumerate(CLASS_NAMES):
        true_positive = int(matrix[class_id, class_id])
        support = int(matrix[class_id].sum())
        predicted_count = int(matrix[:, class_id].sum())
        precision = true_positive / predicted_count if predicted_count else 0.0
        recall = true_positive / support if support else 0.0
        f1 = (
            2 * precision * recall / (precision + recall)
            if precision + recall else 0.0
        )
        per_class[name] = {
            "support": support, "precision": precision, "recall": recall, "f1": f1
        }

    return {
        "accuracy": float(np.mean(predictions == y_true)),
        "confusion_matrix_rows_true_columns_predicted": matrix.tolist(),
        "per_class": per_class,
        "predictions": [CLASS_NAMES[int(i)] for i in predictions],
        "probabilities": probabilities.tolist(),
    }


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--archive", type=Path, default=DEFAULT_ARCHIVE)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--epochs", type=int, default=40)
    parser.add_argument("--batch-size", type=int, default=8)
    parser.add_argument("--validation-fraction", type=float, default=0.20)
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument("--check-only", action="store_true")
    args = parser.parse_args()

    if args.epochs < 1 or args.batch_size < 1:
        parser.error("--epochs and --batch-size must be positive")
    if not 0.10 <= args.validation_fraction <= 0.40:
        parser.error("--validation-fraction must be between 0.10 and 0.40")

    archive_path = args.archive.expanduser().resolve()
    output_dir = args.output_dir.expanduser().resolve()
    tf.keras.utils.set_random_seed(args.seed)
    x, y, records, pending_zero_person_skipped = load_archive(archive_path)
    train_idx, val_idx = stratified_split(y, args.validation_fraction, args.seed)
    x_train, y_train = x[train_idx], y[train_idx]
    x_val, y_val = x[val_idx], y[val_idx]
    train_records = [records[int(i)] for i in train_idx]
    val_records = [records[int(i)] for i in val_idx]

    train_groups = {record["group"] for record in train_records}
    val_groups = {record["group"] for record in val_records}
    overlap = sorted(train_groups & val_groups)

    model = build_model()
    trainable_layers = [
        layer.name for layer in model.layers if layer.trainable_weights
    ]
    if trainable_layers != ["conv2d", "class_logits"]:
        raise RuntimeError(f"Expected exactly Conv2D and Dense trainable layers; got {trainable_layers}")

    summary = {
        "archive": str(archive_path),
        "total_images": int(len(x)),
        "pending_zero_person_entries_skipped": int(pending_zero_person_skipped),
        "original_image_size": [160, 120],
        "model_input_shape": [IMAGE_HEIGHT, IMAGE_WIDTH, CHANNELS],
        "classes": CLASS_NAMES,
        "class_distribution": distribution(y),
        "training_images": int(len(train_idx)),
        "training_class_distribution": distribution(y_train),
        "validation_images": int(len(val_idx)),
        "validation_class_distribution": distribution(y_val),
        "source_groups_total": len(train_groups | val_groups),
        "source_groups_shared_between_splits": overlap,
        "validation_warning": (
            "Image-level stratified split; related source groups occur in both splits. "
            "Validation metrics are preliminary and may be optimistic."
        ),
        "trainable_layers": trainable_layers,
        "trainable_parameters": int(model.count_params()),
        "supported_count_range": "1, 2, or 3 visible people; no zero-person or >3 class yet",
    }
    print("\n=== DATASET AND MODEL CHECK ===")
    print(json.dumps(summary, indent=2))
    model.summary()
    if args.check_only:
        print("\nCHECK PASSED. Training was not started and no model files were written.")
        return

    counts = np.bincount(y_train, minlength=len(CLASS_NAMES))
    class_weights = {
        class_id: float(len(y_train) / (len(CLASS_NAMES) * counts[class_id]))
        for class_id in range(len(CLASS_NAMES))
    }
    print("\nClass weights:", {
        CLASS_NAMES[key]: round(value, 4) for key, value in class_weights.items()
    })

    output_dir.mkdir(parents=True, exist_ok=True)
    best_weights = output_dir / "best.weights.h5"
    callbacks = [
        tf.keras.callbacks.ModelCheckpoint(
            str(best_weights), monitor="val_loss", save_best_only=True,
            save_weights_only=True, verbose=1,
        ),
        tf.keras.callbacks.EarlyStopping(
            monitor="val_loss", patience=7, restore_best_weights=True, verbose=1,
        ),
        tf.keras.callbacks.ReduceLROnPlateau(
            monitor="val_loss", factor=0.5, patience=3, min_lr=1e-6, verbose=1,
        ),
        tf.keras.callbacks.TerminateOnNaN(),
    ]
    history = model.fit(
        x_train, y_train,
        validation_data=(x_val, y_val),
        epochs=args.epochs,
        batch_size=args.batch_size,
        shuffle=True,
        class_weight=class_weights,
        callbacks=callbacks,
        verbose=2,
    )
    if best_weights.is_file():
        model.load_weights(best_weights)

    logits = model.predict(x_val, batch_size=args.batch_size, verbose=0)
    validation_results = evaluate_classes(y_val, logits)
    train_loss, train_accuracy = model.evaluate(
        x_train, y_train, batch_size=args.batch_size, verbose=0
    )
    val_loss, val_accuracy = model.evaluate(
        x_val, y_val, batch_size=args.batch_size, verbose=0
    )

    keras_path = output_dir / "own_count_cnn.keras"
    tflite_path = output_dir / "own_count_cnn.tflite"
    model.save(keras_path)
    tflite_path.write_bytes(tf.lite.TFLiteConverter.from_keras_model(model).convert())

    report = {
        **summary,
        "class_weights": {CLASS_NAMES[key]: value for key, value in class_weights.items()},
        "epochs_completed": len(history.history.get("loss", [])),
        "train_loss": float(train_loss),
        "train_accuracy": float(train_accuracy),
        "validation_loss": float(val_loss),
        "validation_accuracy": float(val_accuracy),
        "validation_results": validation_results,
        "artifacts": {
            "keras_model": str(keras_path),
            "tflite_model": str(tflite_path),
            "tflite_bytes": tflite_path.stat().st_size,
            "best_weights": str(best_weights),
        },
        "validation_images": [record["output_file"] for record in val_records],
    }
    report_path = output_dir / "experiment_report.json"
    report_path.write_text(json.dumps(report, indent=2), encoding="utf-8")
    print("\n=== FINAL RESULTS ===")
    print(json.dumps({
        "train_accuracy": report["train_accuracy"],
        "validation_accuracy": report["validation_accuracy"],
        "validation_class_distribution": report["validation_class_distribution"],
        "confusion_matrix_rows_true_columns_predicted":
            validation_results["confusion_matrix_rows_true_columns_predicted"],
        "per_class": validation_results["per_class"],
        "keras_model": str(keras_path),
        "tflite_model": str(tflite_path),
        "report": str(report_path),
        "tflite_bytes": report["artifacts"]["tflite_bytes"],
    }, indent=2))
    print("\nNOTE: Host experiment only; this does not prove NNPA compatibility or board inference.")


if __name__ == "__main__":
    main()
