from pathlib import Path
import numpy as np
import tensorflow as tf
from PIL import Image

ROOT = Path(__file__).parent
DATA = ROOT / "demo_dataset" / "train"
OUT = ROOT / "model"
OUT.mkdir(exist_ok=True)

classes = ["no_person", "person"]
images, labels = [], []

for label, class_name in enumerate(classes):
    folder = DATA / class_name
    files = sorted(folder.glob("*"))
    if not files:
        raise RuntimeError(f"No training image found in {folder}")

    for path in files:
        try:
            image = Image.open(path).convert("L").resize((32, 32))
            images.append(np.asarray(image, dtype=np.float32)[..., None])
            labels.append(label)
        except Exception as exc:
            print(f"Skipping {path}: {exc}")

x = np.array(images, dtype=np.float32)
y = np.array(labels, dtype=np.float32)

if len(set(labels)) != 2:
    raise RuntimeError("Need at least one image in each class.")

model = tf.keras.Sequential([
    tf.keras.layers.Input(shape=(32, 32, 1)),
    tf.keras.layers.Rescaling(1.0 / 255),
    tf.keras.layers.Conv2D(4, 3, padding="same", activation="relu"),
    tf.keras.layers.Flatten(),
    tf.keras.layers.Dense(1, activation="sigmoid"),
])

model.compile(optimizer="adam", loss="binary_crossentropy",
              metrics=["accuracy"])

model.summary()
model.fit(x, y, epochs=60, batch_size=2, verbose=0)

model_path = OUT / "simple_person_cnn.keras"
model.save(model_path)

converter = tf.lite.TFLiteConverter.from_keras_model(model)
tflite_model = converter.convert()
(OUT / "simple_person_cnn.tflite").write_bytes(tflite_model)

print("\nTraining-set predictions:")
predictions = model.predict(x, verbose=0).flatten()
for path, label, prediction in zip(
        [p for c in classes for p in sorted((DATA / c).glob("*"))],
        labels, predictions):
    print(f"{path.name}: actual={classes[label]}, "
          f"predicted={'person' if prediction >= 0.5 else 'no_person'}, "
          f"score={prediction:.4f}")

print("\nSaved:", model_path)
print("Saved:", OUT / "simple_person_cnn.tflite")
