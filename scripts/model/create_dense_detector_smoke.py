from pathlib import Path

import numpy as np
import tf_keras as keras
from tf_keras import layers

ROOT = Path(__file__).resolve().parents[2]
OUT = ROOT / "tests/compiler_smoke/dense_detector_baseline"
OUT.mkdir(parents=True, exist_ok=True)

bias = keras.initializers.Constant(0.01)

inputs = keras.Input(
    shape=(128, 160, 1),
    name="grayscale_input",
)

# Downsample 128 x 160 to 64 x 80.
x = layers.Conv2D(
    8, 3, strides=2, padding="same",
    activation="relu", use_bias=True,
    bias_initializer=bias, name="stem_conv",
)(inputs)

# Downsample 64 x 80 to 32 x 40.
x = layers.DepthwiseConv2D(
    3, strides=2, padding="same",
    depth_multiplier=1, activation="relu",
    use_bias=True, bias_initializer=bias,
    name="depthwise_1",
)(x)

# Preserve the 32 x 40 prediction grid.
x = layers.Conv2D(
    16, 3, strides=1, padding="same",
    activation="relu", use_bias=True,
    bias_initializer=bias, name="feature_conv",
)(x)

x = layers.DepthwiseConv2D(
    3, strides=1, padding="same",
    depth_multiplier=1, activation="relu",
    use_bias=True, bias_initializer=bias,
    name="depthwise_2",
)(x)

# Four slots per cell; five raw values per slot.
outputs = layers.Conv2D(
    20, 1, padding="same",
    activation="linear", use_bias=True,
    bias_initializer=bias, name="grid_predictions",
)(x)

model = keras.Model(inputs, outputs, name="dense_person_grid_detector")

model_path = OUT / "dense_person_grid_detector.h5"
model.save(str(model_path))

# Synthetic inputs are for conversion smoke testing only.
rng = np.random.default_rng(42)
calibration = rng.uniform(
    0.0, 1.0, size=(32, 128, 160, 1)
).astype(np.float32)

calibration_path = OUT / "calibration.npz"
np.savez_compressed(calibration_path, x_test=calibration)

print("Saved model:", model_path)
print("Saved calibration:", calibration_path)
print("Input shape:", model.input_shape)
print("Output shape:", model.output_shape)
print("Parameter count:", model.count_params())
print("Prediction slots per cell:", 4)
print("Values per slot:", 5)
