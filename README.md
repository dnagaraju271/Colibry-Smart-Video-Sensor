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
