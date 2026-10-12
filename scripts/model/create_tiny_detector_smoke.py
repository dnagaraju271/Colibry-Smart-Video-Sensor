from pathlib import Path

import numpy as np
import tf_keras as keras
from tf_keras import layers

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "tests/compiler_smoke/detector_baseline"
OUT.mkdir(parents=True, exist_ok=True)

# Use nonzero bias initializers for compiler smoke testing.
bias = keras.initializers.Constant(0.01)

inputs = keras.Input(
    shape=(128, 160, 1),
    name="grayscale_input",
)

x = layers.Conv2D(
    8, 3, strides=2, padding="same",
    activation="relu", use_bias=True,
    bias_initializer=bias, name="stem_conv",
)(inputs)

x = layers.DepthwiseConv2D(
    3, strides=2, padding="same",
    depth_multiplier=1, activation="relu",
    use_bias=True, bias_initializer=bias,
    name="depthwise_1",
)(x)

x = layers.Conv2D(
    16, 3, strides=2, padding="same",
    activation="relu", use_bias=True,
    bias_initializer=bias, name="feature_conv",
)(x)

x = layers.DepthwiseConv2D(
    3, strides=2, padding="same",
    depth_multiplier=1, activation="relu",
    use_bias=True, bias_initializer=bias,
    name="depthwise_2",
)(x)

# Five raw output values per grid cell.
# No sigmoid or bounding-box decoding is applied here.
outputs = layers.Conv2D(
    5, 1, padding="same",
    activation="linear", use_bias=True,
    bias_initializer=bias, name="grid_predictions",
)(x)

model = keras.Model(inputs, outputs, name="tiny_person_grid_detector")
model_path = OUT / "tiny_person_grid_detector.h5"
model.save(str(model_path))

# Synthetic calibration data is only for testing conversion.
# It is not real image data and cannot establish model accuracy.
rng = np.random.default_rng(42)
calibration = rng.uniform(
    0.0, 1.0, size=(32, 128, 160, 1)
).astype(np.float32)

calibration_path = OUT / "calibration.npz"
np.savez_compressed(calibration_path, x_test=calibration)

print("Saved model:", model_path)
print("Saved calibration data:", calibration_path)
print("Input shape:", model.input_shape)
print("Output shape:", model.output_shape)
print("Parameter count:", model.count_params())
