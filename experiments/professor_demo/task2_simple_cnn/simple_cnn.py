#!/usr/bin/env python3
"""Train, evaluate, and export the professor's independent minimal CNN."""

from __future__ import annotations

import argparse
from pathlib import Path

import numpy as np
import tensorflow as tf
from tensorflow.keras import layers, models


SCRIPT_DIR = Path(__file__).resolve().parent
DEFAULT_DATA_DIR = SCRIPT_DIR.parent / "task3_dataset" / "images"
MODEL_DIR = SCRIPT_DIR / "model"
MODEL_PATH = MODEL_DIR / "professor_simple_cnn.keras"
TFLITE_PATH = MODEL_DIR / "professor_simple_cnn.tflite"

IMAGE_SIZE = (32, 32)  # height, width
CLASS_NAMES = ["no_person", "person"]
BATCH_SIZE = 16


def build_model() -> tf.keras.Model:
    """Build a binary classifier with exactly two trainable layers."""
    model = models.Sequential(
        [
            layers.Input(shape=(32, 32, 1), name="grayscale_image"),
            layers.Rescaling(1.0 / 255.0, name="normalize"),
            layers.Conv2D(
                filters=4,
                kernel_size=(3, 3),
                padding="same",
                activation="relu",
                name="conv2d",
            ),
            layers.Flatten(name="flatten"),
            layers.Dense(1, activation="sigmoid", name="person_probability"),
        ],
        name="professor_simple_cnn",
    )
    model.compile(
        optimizer="adam",
        loss="binary_crossentropy",
        metrics=["accuracy"],
    )

    trainable_layers = [layer.name for layer in model.layers if layer.trainable_weights]
    if trainable_layers != ["conv2d", "person_probability"]:
        raise RuntimeError(
            f"Expected exactly Conv2D and Dense to be trainable; got {trainable_layers}"
        )
    return model


def count_images(split_dir: Path) -> int:
    extensions = {".jpg", ".jpeg", ".png", ".bmp"}
    return sum(
        1
        for class_name in CLASS_NAMES
        for path in (split_dir / class_name).glob("*")
        if path.is_file() and path.suffix.lower() in extensions
    )


def load_split(directory: Path, *, shuffle: bool) -> tf.data.Dataset:
    return tf.keras.utils.image_dataset_from_directory(
        directory,
        labels="inferred",
        label_mode="binary",
        class_names=CLASS_NAMES,
        color_mode="grayscale",
        batch_size=BATCH_SIZE,
        image_size=IMAGE_SIZE,
        shuffle=shuffle,
        seed=42,
    )


def train(data_dir: Path, epochs: int) -> None:
    train_dir = data_dir / "train"
    validation_dir = data_dir / "validation"

    for split_dir in (train_dir, validation_dir):
        for class_name in CLASS_NAMES:
            class_dir = split_dir / class_name
            if not class_dir.is_dir():
                raise FileNotFoundError(
                    f"Missing dataset folder: {class_dir}\n"
                    "See task3_dataset/README.md for the required structure."
                )

    for split_dir in (train_dir, validation_dir):
        for class_name in CLASS_NAMES:
            if not any(
                path.is_file() and path.suffix.lower() in {".jpg", ".jpeg", ".png", ".bmp"}
                for path in (split_dir / class_name).glob("*")
            ):
                raise RuntimeError(
                    f"No supported images found in {split_dir / class_name}"
                )

    tf.keras.utils.set_random_seed(42)
    train_ds = load_split(train_dir, shuffle=True)
    validation_ds = load_split(validation_dir, shuffle=False)

    model = build_model()
    model.summary()
    print(
        "\nTrainable layers:",
        [layer.name for layer in model.layers if layer.trainable_weights],
    )
    print(
        "Trainable parameters:",
        sum(int(np.prod(weight.shape)) for weight in model.trainable_weights),
    )

    model.fit(
        train_ds,
        validation_data=validation_ds,
        epochs=epochs,
    )

    loss, accuracy = model.evaluate(validation_ds, verbose=0)
    print(f"Validation loss: {loss:.4f}")
    print(f"Validation accuracy: {accuracy:.4f}")

    MODEL_DIR.mkdir(parents=True, exist_ok=True)
    model.save(MODEL_PATH)

    converter = tf.lite.TFLiteConverter.from_keras_model(model)
    TFLITE_PATH.write_bytes(converter.convert())

    print(f"Saved Keras model: {MODEL_PATH}")
    print(f"Saved TensorFlow Lite model: {TFLITE_PATH}")


def predict(image_path: Path) -> None:
    if not MODEL_PATH.is_file():
        raise FileNotFoundError(
            f"Trained model not found: {MODEL_PATH}. Train the CNN first."
        )
    if not image_path.is_file():
        raise FileNotFoundError(f"Image not found: {image_path}")

    model = tf.keras.models.load_model(MODEL_PATH)
    image = tf.keras.utils.load_img(
        image_path,
        color_mode="grayscale",
        target_size=IMAGE_SIZE,
    )
    image_array = tf.keras.utils.img_to_array(image)
    image_array = np.expand_dims(image_array, axis=0)

    probability = float(model.predict(image_array, verbose=0)[0][0])
    label = "person" if probability >= 0.5 else "no_person"
    print(f"Prediction: {label}")
    print(f"Person probability: {probability:.4f}")


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Train or test the professor's independent minimal CNN."
    )
    parser.add_argument(
        "--data-dir",
        type=Path,
        default=DEFAULT_DATA_DIR,
        help=f"Task 3 dataset root (default: {DEFAULT_DATA_DIR})",
    )
    parser.add_argument("--epochs", type=int, default=5)
    parser.add_argument(
        "--predict",
        type=Path,
        help="Classify one image using the saved Keras model",
    )
    args = parser.parse_args()

    if args.predict is not None:
        predict(args.predict)
    else:
        if args.epochs < 1:
            parser.error("--epochs must be at least 1")
        train(args.data_dir, args.epochs)


if __name__ == "__main__":
    main()
