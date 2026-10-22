# Colibry Smart Video Sensor

An embedded AI project exploring person detection on the ASYGN
ColibryNPU evaluation platform.

## Project Goal

Develop a compact grayscale person-detection pipeline for an embedded
neural processing unit, targeting a 160 x 128 input resolution and
investigating the supported model operators and quantization workflow.

The intended output is person bounding boxes.

## Current Progress

- Prepared a COCO-based person-detection dataset manifest.
- Converted selected images and labels to grayscale 160 x 128 format.
- Audited dataset labels and reviewed selected empty-label cases.
- Tested the Colibry compiler with the SDK's reference VWW model.
- Ran a synthetic model conversion and compiler smoke test.
- Configured Git version control and GitHub.

## Important Limitations

The synthetic model is not a trained person detector.
Compiler acceptance does not demonstrate detection accuracy or successful
inference on the physical ColibryNPU board.

Physical-board inference and camera integration remain to be tested.

## Repository Structure

- `scripts/dataset/` — dataset preparation, validation, and audit scripts
- `dataset/coco_person_1000/manifest.csv` — selected dataset metadata
- `tests/compiler_smoke/` — compiler smoke-test reports
- `.gitignore` — excludes generated artifacts and downloaded datasets

Dataset images and generated model binaries are intentionally excluded
from version control.

## Next Milestones

1. Confirm which detector architectures and output operations the compiler supports.
2. Build and train a small person detector compatible with those constraints.
3. Quantize and compile the trained model.
4. Evaluate accuracy and model size.
5. Test inference on the physical evaluation board.
6. Integrate camera input and bounding-box visualization.

## Technology

- Python
- OpenCV
- TensorFlow / TensorFlow Lite
- COCO dataset annotations
- ASYGN ColibryNPU compiler and SDK

## Status

Research and prototyping. This project has not yet demonstrated
person detection on the physical evaluation board.

## Train the compact Colibry person detector

The training entry point targets **160 pixels wide × 128 pixels high** in
grayscale. TensorFlow uses channels-last shape `(N, 128, 160, 1)`; this is the
same image size, with height listed before width. The training architecture is a
compact hardware-oriented grid detector, not the original YOLOv8 graph. Cached
YOLOv8 detections can optionally supplement labels on images already annotated
with people; known-empty images remain negative.

Review [the person detector training guide](docs/COLIBRY_PERSON_TRAINING.md)
for dataset format, the pre-training validation command, model outputs, and the
limits of the validation metrics. Dataset images, teacher caches, and generated
weights remain outside Git history.

## Run a packed input on the Colibry board

With the reference firmware already running and waiting for an input, send the
prepacked 160 x 128 input over SPIS without resetting the board or rewriting
flash:

```bash
"$HOME/venvs/colibry-compiler/bin/python" \
  "$HOME/Colibry-Smart-Video-Sensor/scripts/board/run_nnpa_packed_input_inference.py" \
  --sdk-root "$HOME/colibryNPU_SDK" \
  --input "$HOME/models/colibry_yolo/compression_trial/test_person_input_nnpa_packed.bin"
```

The script reads the NNPA input address and size from the live registers, sends
the packed input, requests one inference, and reports the inference counter,
status, and firmware result message. It does not flash or restart the board.
Use `--port ftdi://...` only when you need to select a specific current FTDI
URI. A successful inference confirms execution, not person-detection accuracy:
the current YOLO backbone produces a feature map, and detection decoding is a
separate step.

See [the board inference script](scripts/board/run_nnpa_packed_input_inference.py).

## Check a YOLO candidate for Colibry

Use the configuration-driven compatibility inspector to examine a model's
input/output shapes and operator types before attempting NNPA compilation:

```bash
python3 scripts/model/check_yolo_colibry_compat.py --list-candidates
python3 scripts/model/check_yolo_colibry_compat.py --model /path/to/yolov8n.onnx --check-compiler

# Run the compiler check too; the log and artifacts go outside the repository.
python3 scripts/model/check_yolo_colibry_compat.py \
  --model /path/to/yolov8n.onnx \
  --run-compiler \
  --json-report "$HOME/models/colibry_yolo/compiler_test/yolov8n_report.json"
```

The model can be selected with `--model` or the `COLIBRY_YOLO_MODEL`
environment variable; no source-code changes are needed for routine model
selection. Settings are in `configs/yolo_colibry_check.json`. See
[`docs/YOLO_COLIBRY_COMPATIBILITY.md`](docs/YOLO_COLIBRY_COMPATIBILITY.md)
for supported file formats, optional inspection packages, reports, and the
explicit compiler-run safety gate.

A successful structural inspection does not prove NNPA compatibility or
physical-board inference. The checker does not flash the board.
