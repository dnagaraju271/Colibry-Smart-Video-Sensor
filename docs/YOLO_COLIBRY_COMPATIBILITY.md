# Checking a YOLO candidate for ColibryNPU

This workflow adds a reusable, configuration-driven check for YOLO model candidates. It is an **inspection step**, not proof that a model will compile or run on the physical Colibry board.

## What it checks

- Finds YOLO-named .onnx, .tflite, .pt, .pth, .h5, and .keras model files in configured directories.
- Uses the relevant optional runtime/parser (ONNX, TensorFlow Lite, Ultralytics, or TensorFlow/Keras) to inspect input/output tensors and operator/layer types when available.
- Compares known image input dimensions and channel count with the configured Colibry target.
- Optionally checks whether the configured NNPA compiler executable exists.
- Can save a JSON report for comparing different model candidates.

The script does not download model weights, install packages, flash hardware, or run a compiler by default.

## Run it

From the repository root, using the Python environment where the optional model-inspection packages are installed:

```bash
# List model files that the configuration can find.
python3 scripts/model/check_yolo_colibry_compat.py --list-candidates

# Inspect one exact candidate; replace the path with the real model location.
python3 scripts/model/check_yolo_colibry_compat.py \
  --model /path/to/yolov8n.onnx \
  --check-compiler \
  --json-report runs/yolo_colibry_check.json

# Attempt a compiler compatibility check and save the full log outside the repository.
python3 scripts/model/check_yolo_colibry_compat.py \
  --model /path/to/yolov8n.onnx \
  --run-compiler \
  --json-report "$HOME/models/colibry_yolo/compiler_test/yolov8n_report.json"
```

You can also select a model without changing source code:

```bash
export COLIBRY_YOLO_MODEL="$HOME/models/yolov8n.onnx"
python3 scripts/model/check_yolo_colibry_compat.py --check-compiler
```

Or add another search directory:

```bash
python3 scripts/model/check_yolo_colibry_compat.py \
  --search-root "$HOME/Downloads/models" --list-candidates
```

The default search paths and target tensor settings are in
configs/yolo_colibry_check.json. The target currently reflects the project's
grayscale image convention: width 160, height 128, one channel. Typical YOLO
exports may use RGB and a different resolution; warnings indicate a mismatch,
not a safe automatic conversion.

## Optional inspection packages

Only install what matches the format you want to inspect:

- ONNX: onnx
- TFLite: tflite-runtime or the TensorFlow installation compatible with the Colibry SDK
- Ultralytics/PyTorch weights: ultralytics
- Keras/H5: tensorflow

The checker reports when an optional inspector is missing; it does not install packages automatically.

## Compiler execution

The configuration uses the invocation verified against Colibry NNPA compiler 0.1.2:

```text
nnpa-compiler MODEL --verbose
```

Set `compiler.path`, `compiler.command`, `compiler.output_dir`, and `compiler.timeout_seconds` in the JSON configuration for your machine. The command template supports `{compiler}`, `{model}`, `{output}`, and `{root}` placeholders. The checker runs it without a shell and uses the configured output directory as its working directory, outside the Git repository by default. It saves a compiler log and records any first unsupported operator it can recognize.

Only ONNX, TFLite, and TensorFlow/Keras-style models are candidates for this CLI. The checker refuses raw `.pt/.pth` model files and asks you to export them first. Compilation is explicitly opt-in through `--run-compiler`; the script never flashes the board.

This test may fail on an unsupported layer (as standard YOLO exports can); failure is a useful compatibility result. Even a successful compile is not a substitute for testing inference on the physical board.

## Interpret the result

- **Structural inspection passed**: the selected file could be opened by its model-format tooling.
- **Input warnings**: dimensions, channels, or dynamic shapes need attention for this configured target.
- **Compatibility not confirmed**: only an accepted compiler build and verified physical-board inference can establish that.
