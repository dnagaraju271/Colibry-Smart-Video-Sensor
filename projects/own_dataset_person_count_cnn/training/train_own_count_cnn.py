#!/usr/bin/env python3
"""Train the own-dataset 4-class, two-trainable-layer CNN.

Reads the selected OWN_Dataset ZIP directly. The archive is never extracted
or modified. Validation is stratified by source scene so adjacent frames from
one timestamp group cannot cross between training and validation.
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
from sklearn.model_selection import StratifiedGroupKFold

ROOT = Path(__file__).resolve().parents[3]
DEFAULT_ARCHIVE = ROOT / "datasets/mobile_160x120/OWN_Dataset_71plus111_Deduplicated_Scene_Frames_160x120.zip"
DEFAULT_OUTPUT = ROOT / "projects/own_dataset_person_count_cnn/models/four_class_71plus111_group_split"
CLASS_NAMES = ["1_person", "2_person", "3_person", "4_person"]
CLASS_TO_ID = {name: index for index, name in enumerate(CLASS_NAMES)}
IMAGE_WIDTH, IMAGE_HEIGHT, CHANNELS = 40, 30, 1


def source_group(source_filename: str) -> str:
    match = re.search(
        r"(WhatsApp Image \d{4}-\d{2}-\d{2} at \d{2}\.\d{2}\.\d{2})",
        source_filename,
    )
    return match.group(1) if match else source_filename


def load_archive(path: Path) -> tuple[np.ndarray, np.ndarray, list[dict[str, str]], int, int]:
    if not path.is_file():
        raise FileNotFoundError(f"Dataset archive not found: {path}")

    images: list[np.ndarray] = []
    labels: list[int] = []
    records: list[dict[str, str]] = []
    pending_zero_person_skipped = 0
    pending_over_3_person_skipped = 0

    with zipfile.ZipFile(path) as archive:
        names = set(archive.namelist())
        manifests = [n for n in names if PurePosixPath(n).name.lower() == "manifest.csv"]
        if len(manifests) != 1:
            raise ValueError(f"Expected one manifest.csv, found {len(manifests)}")

        with archive.open(manifests[0]) as raw:
            reader = csv.DictReader(io.TextIOWrapper(raw, encoding="utf-8-sig", newline=""))
            required = {"source_filename", "visible_person_count"}
            if not reader.fieldnames or not required.issubset(reader.fieldnames):
                raise ValueError(f"Manifest must contain columns: {sorted(required)}")
            output_fields = ("output_file", "output_path", "output_filename")
            if not any(field in reader.fieldnames for field in output_fields):
                raise ValueError(
                    "Manifest must contain one image path column: "
                    + ", ".join(output_fields)
                )

            seen_outputs: set[str] = set()
            for line_no, row in enumerate(reader, start=2):
                output_file = next(
                    ((row.get(field) or "").strip() for field in output_fields
                     if (row.get(field) or "").strip()),
                    "",
                )
                source = (row.get("source_filename") or "").strip()
                if not output_file or not source:
                    raise ValueError(f"Manifest line {line_no}: missing source/output filename")
                if output_file in seen_outputs:
                    raise ValueError(f"Manifest line {line_no}: duplicate output {output_file}")
                seen_outputs.add(output_file)

                class_name = PurePosixPath(output_file).parts[0]
                count = int(row["visible_person_count"])

                # Zero-person originals are retained in the archive but the
                # current 1/2/3/4 classifier does not train on a zero-person class.
                if class_name in {"pending_for_later_originals", "0_person"}:
                    if count != 0:
                        raise ValueError(
                            f"Manifest line {line_no}: pending originals must have count 0"
                        )
                    pending_zero_person_skipped += 1
                    continue

                # Preserve more-than-four-person originals without folding
                # them into the exact-four class.
                if class_name in {"pending_over_3_originals", "pending_over_4_originals"}:
                    if count <= 4:
                        raise ValueError(
                            f"Manifest line {line_no}: over-4 pending entry must have count > 4"
                        )
                    pending_over_3_person_skipped += 1
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
                    "group": (row.get("scene_id") or "").strip() or source_group(source),
                })

    if not images:
        raise RuntimeError("No labeled images found in the archive manifest")
    return (
        np.stack(images).astype(np.float32),
        np.asarray(labels, np.int32),
        records,
        pending_zero_person_skipped,
        pending_over_3_person_skipped,
    )


def stratified_group_split(
    labels: np.ndarray,
    groups: np.ndarray,
    validation_fraction: float,
    seed: int,
) -> tuple[np.ndarray, np.ndarray]:
    """Hold out complete source scenes while keeping all four classes represented."""
    unique_groups = np.unique(groups)
    if len(unique_groups) < 2:
        raise ValueError("At least two source scenes are required for group validation")

    total_counts = np.bincount(labels, minlength=len(CLASS_NAMES))
    n_splits = max(2, int(round(1.0 / validation_fraction)))
    n_splits = min(n_splits, len(unique_groups))
    splitter = StratifiedGroupKFold(
        n_splits=n_splits,
        shuffle=True,
        random_state=seed,
    )

    candidates: list[tuple[float, np.ndarray, np.ndarray]] = []
    for train_indices, validation_indices in splitter.split(
        np.zeros((len(labels), 1), dtype=np.uint8), labels, groups
    ):
        train_counts = np.bincount(labels[train_indices], minlength=len(CLASS_NAMES))
        validation_counts = np.bincount(
            labels[validation_indices], minlength=len(CLASS_NAMES)
        )
        # A valid fold must preserve every class in both subsets.
        if np.any(train_counts == 0) or np.any(validation_counts == 0):
            continue

        actual_validation_fraction = len(validation_indices) / len(labels)
        per_class_validation_fractions = validation_counts / total_counts
        score = (
            abs(actual_validation_fraction - validation_fraction)
            + float(np.mean(
                np.abs(per_class_validation_fractions - validation_fraction)
            ))
        )
        candidates.append((score, train_indices, validation_indices))

    if not candidates:
        raise ValueError(
            "Could not form a source-scene-held-out validation split containing "
            "all four classes in both train and validation. Collect more scenes "
            "for each class or adjust --validation-fraction."
        )

    _, train_indices, validation_indices = min(
        candidates, key=lambda item: item[0]
    )
    if set(groups[train_indices]) & set(groups[validation_indices]):
        raise RuntimeError("Source-scene leakage detected between train and validation")
    return (
        np.asarray(train_indices, dtype=np.int64),
        np.asarray(validation_indices, dtype=np.int64),
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
            tf.keras.layers.Dense(4, activation="linear", name="class_logits"),
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
    parser.add_argument(
        "--additional-archive", type=Path, action="append", default=[],
        help="Additional dataset ZIP; may be repeated to combine labeled collections.",
    )
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

    archive_paths = [
        args.archive.expanduser().resolve(),
        *(path.expanduser().resolve() for path in args.additional_archive),
    ]
    output_dir = args.output_dir.expanduser().resolve()
    tf.keras.utils.set_random_seed(args.seed)

    loaded = [load_archive(path) for path in archive_paths]
    x = np.concatenate([part[0] for part in loaded], axis=0)
    y = np.concatenate([part[1] for part in loaded], axis=0)
    records: list[dict[str, str]] = []
    for archive_path, part in zip(archive_paths, loaded):
        for record in part[2]:
            record = dict(record)
            record["archive"] = str(archive_path)
            # Keep source-group IDs unique when multiple archives are combined.
            record["group"] = f"{archive_path.stem}:{record['group']}"
            records.append(record)
    pending_zero_person_skipped = sum(part[3] for part in loaded)
    pending_over_3_person_skipped = sum(part[4] for part in loaded)
    source_groups = np.asarray([record["group"] for record in records], dtype=str)
    train_idx, val_idx = stratified_group_split(
        y, source_groups, args.validation_fraction, args.seed
    )
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
        "archives": [str(path) for path in archive_paths],
        "total_training_eligible_images": int(len(x)),
        "pending_zero_person_entries_skipped": int(pending_zero_person_skipped),
        "pending_over_3_person_entries_skipped": int(pending_over_3_person_skipped),
        "original_image_size": [160, 120],
        "model_input_shape": [IMAGE_HEIGHT, IMAGE_WIDTH, CHANNELS],
        "classes": CLASS_NAMES,
        "class_distribution": distribution(y),
        "training_images": int(len(train_idx)),
        "training_class_distribution": distribution(y_train),
        "validation_images": int(len(val_idx)),
        "validation_class_distribution": distribution(y_val),
        "source_groups_total": len(train_groups | val_groups),
        "split_method": "StratifiedGroupKFold; complete source scenes held out",
        "train_source_groups": sorted(train_groups),
        "validation_source_groups": sorted(val_groups),
        "source_groups_shared_between_splits": overlap,
        "validation_warning": (
            "Complete source scenes are held out, preventing adjacent frames from "
            "crossing the split. Metrics are still preliminary because the dataset "
            "has a limited number of source scenes and relatively few 3- and 4-person scenes."
        ),
        "trainable_layers": trainable_layers,
        "trainable_parameters": int(model.count_params()),
        "supported_count_range": "1, 2, 3, or 4 visible people; zero-person and >4 examples are retained in pending folders and skipped",
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
