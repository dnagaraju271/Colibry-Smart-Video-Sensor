# Colibry Smart Video Sensor

An embedded AI project exploring person detection on the ASYGN
ColibryNPU evaluation platform.

## Project Goal

Develop a compact grayscale person-detection pipeline for an embedded
neural processing unit, targeting a 160 x 128 input resolution and
investigating the supported model operators and quantization workflow.

The intended output is person bounding boxes.

## Current Progress

- Prepared a COCO-based person-detection dataset manifest and grayscale 160 x 128 preprocessing workflow.
- Recovered the original 35,280-byte INT8 TFLite grid detector and verified its actual input/output tensor shapes and quantization metadata.
- Recompiled that exact TFLite file with Colibry NPU compiler 0.2.1 and reproduced the older project's 14,224-byte NNPA artifact byte-for-byte (model ID `0x3F7B`).
- Investigated the historical flash deployment recipe and documented its expected sector image hashes.
- Inspected the SDK camera/ISP path and the project's NNPA board diagnostics.
- Identified the current FTDI/PyFtdi access issue preventing a fresh read-only flash verification.

## Important Limitations

Host-side compilation and tensor metadata verification do not demonstrate
person-detection accuracy or successful inference on the physical ColibryNPU
board. The current contents of flash sector `0x20000` remain unverified,
and live camera-to-NNPA inference has not yet been demonstrated.

## Repository Structure

- `scripts/dataset/` — dataset preparation, validation, and audit scripts
- `dataset/coco_person_1000/manifest.csv` — selected dataset metadata
- `tests/compiler_smoke/` — compiler smoke-test reports
- `.gitignore` — excludes generated artifacts and downloaded datasets

Dataset images and generated model binaries are intentionally excluded
from version control.

## Next Milestones

1. Resolve PyFtdi USB descriptor/permission access without changing flash.
2. Read flash sector `0x20000` and compare it with the known backup/replacement hashes.
3. Confirm the active firmware's NNPA model configuration and runtime input/output sizes.
4. Establish the camera/ISP configuration that delivers the detector's 160 x 128 grayscale input.
5. Run controlled physical-board inference and validate output dequantization and grid decoding.
6. Demonstrate person and empty-scene tests using live camera frames.

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
flash. The dedicated dump script saves the complete raw feature map. The following command uses sudo for FTDI access and saves the complete
raw feature map for later comparison:

```bash
sudo "$HOME/venvs/colibry-compiler/bin/python" \
  "$HOME/Colibry-Smart-Video-Sensor/scripts/board/dump_nnpa_featuremap.py" \
  --sdk-root "$HOME/colibryNPU_SDK" \
  --input "$HOME/models/colibry_yolo/compression_trial/test_person_input_nnpa_packed.bin" \
  --output-dump "$HOME/models/colibry_yolo/compression_trial/nnpa_output_featuremap_raw.bin"
```

The script reads live NNPA input and output registers, sends the packed input,
requests one inference, and saves the complete output feature-map bytes. It
does not flash or restart the board. Use `--port ftdi://...` only when you
need to select a specific current FTDI URI. A successful inference confirms
execution, not person-detection accuracy: a feature-map capture is not a
decoded person detection. This diagnostic flow is separate from the recovered
grid-detector artifact documented below; do not treat an older YOLO feature-map
capture as validation of the recovered `0x3F7B` person detector.

See [the board inference script](scripts/board/dump_nnpa_featuremap.py).

## Diagnose NNPA output memory lanes

The flat output capture had a strong byte-stride pattern, so inspect a larger
read-only memory window at the current output address before assuming a tensor
layout. This reads memory only; it does not trigger inference, reset the CPU,
or write flash. Run it while firmware is idle:

```bash
sudo "$HOME/venvs/colibry-compiler/bin/python" \
  "$HOME/Colibry-Smart-Video-Sensor/scripts/board/read_nnpa_output_window.py" \
  --sdk-root "$HOME/colibryNPU_SDK" \
  --output "$HOME/models/colibry_yolo/compression_trial/nnpa_output_memory_window4.bin"
```

The diagnostic reads four bytes per logical output element as a hypothesis
and saves the 81,920-byte window. The byte lanes are candidates to inspect, not
a declaration of the documented output layout.

## Compare NNPA output with the ONNX reference

After saving `nnpa_output_memory_window4.bin`, compare the accelerator feature
map with ONNX Runtime using the exact same input tensor. The utility reads
scale and zero-point from the ONNX model's final `DequantizeLinear` node. It
does not connect to the board, trigger inference, or modify flash. It evaluates
four candidate byte lanes if the supplied output file is 81,920 bytes:

```bash
"$HOME/venvs/colibry-compiler/bin/python" \
  "$HOME/Colibry-Smart-Video-Sensor/scripts/board/compare_nnpa_featuremap.py" \
  --input "$HOME/models/colibry_yolo/compression_trial/test_person_input_nchw_u8.bin" \
  --raw-output "$HOME/models/colibry_yolo/compression_trial/nnpa_output_memory_window4.bin" \
  --model "$HOME/models/colibry_yolo/compression_trial/yolov8n_layer4_qdq_conv_add_int8_terminal.onnx"
```

It reports scale/zero-point, output ranges, and numerical comparison metrics
for HWC and NCHW flattening. A large discrepancy can indicate input preprocessing,
quantization, memory-ordering, or compiler/runtime differences; it is not by
itself proof of a defective accelerator. The comparison environment needs
`onnxruntime`, `onnx`, and `numpy`.

See [the comparison script](scripts/board/compare_nnpa_featuremap.py).

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

## Latest detector artifact recovery (2026-11-15)

The original 35,280-byte INT8 detector TFLite was recovered and inspected.
Its verified input is `[1,128,160,1]` INT8 (scale 1/255, zero-point -128);
its output is `[1,32,40,5]` INT8 (scale 0.058669433, zero-point 87).

Colibry NPU compiler 0.2.1 reproduced the historical 14,224-byte NNPA artifact
with model ID `0x3F7B`. The reproduced artifact SHA-256 is
`fe9897ff4694328b22fb3fc91fba9097ca353a18365c197f196011ed14be9fd2`.
A different NNPA file in the current working clone has a different hash and
must not be assumed identical.

The historical flash deployment recipe targets sector 2 at `0x20000`, but
the current flash contents have not been independently verified. Ubuntu sees
the FTDI USB bridge (`0403:6043`), while PyFtdi currently fails on USB string
descriptor access. No flash erase or programming was performed during the
2026-11-15 verification attempt.

See [the detector artifact recovery and board deployment audit](docs/DETECTOR_ARTIFACT_RECOVERY_2026-11-15.md)
and [the detailed development progress](docs/DEVELOPMENT_PROGRESS.md).
