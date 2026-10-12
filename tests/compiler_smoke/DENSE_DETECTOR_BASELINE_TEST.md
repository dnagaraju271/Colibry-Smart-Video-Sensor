# Dense Detector Baseline — Compiler Smoke Test

**Date:** 2026-10-12
**Compiler:** Colibry NPU compiler 0.1.2
**TensorFlow:** 2.18.1

## Purpose

Check whether a small detector-shaped convolutional network can be
quantized and compiled for the Colibry NPU.

## Architecture

- Input: 160 x 128 x 1 grayscale
- Output: 32 x 40 x 20
- Prediction slots: 4 per cell
- Values per slot: 5
- Parameters before quantization: 1,828
- Operators: Conv2D and DepthwiseConv2D

## Quantization

- Method: post-training quantization (PTQ)
- Calibration samples: 32 synthetic random inputs
- Input and output inference types: INT8

## Compiler Results

- TFLite constraints check: passed
- NNPA file generation: successful
- Reported MACs: 2,647,040
- Reported cycles: 167,744
- Configuration size: 2,208 bytes
- Inference data: 2,601 words, 3 buffers
- Estimated energy: 9.1 uJ per inference at VDD=0.8 V
- Compiler reports configuration and data fit its memory constraints

## Limitations

This model is untrained. Synthetic calibration does not establish
person-detection accuracy.

The compiler log does not provide a verified bounding-box detection
accuracy result. Physical-board inference remains untested.

## Generated Artifact

tests/compiler_smoke/dense_detector_baseline/quantized/dense_person_grid_detector_int8.tflite.nnpa

Generated model and calibration artifacts are excluded from Git.
