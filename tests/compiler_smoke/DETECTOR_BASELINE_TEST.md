# Tiny Detector Baseline — Compiler Smoke Test

**Date:** 2026-10-12  
**Compiler:** Colibry NPU compiler 0.1.2  
**TensorFlow:** 2.18.1

## Purpose

Test whether a small detector-shaped convolutional network can be
post-training quantized and accepted by the Colibry NNPA compiler.

## Model

- Input: 160 x 128 x 1 grayscale
- Output: 8 x 10 x 5 prediction grid
- Operators: Conv2D and DepthwiseConv2D
- Calibration: 32 synthetic random float32 inputs
- Quantization: full-int8 PTQ

## Compiler Results

- TFLite constraint check: passed
- NNPA file generation: reported successful
- Compiler-reported MACs: 905,360
- Compiler-reported cycles: 56,540
- Configuration: 1,904 bytes (119 words)
- Inference data: 2,601 words, 3 buffers
- Estimated energy: 3.1 uJ/inference at VDD=0.8 V
- Compiler reports configuration and data fit its memory constraints

## Limitations

This is an untrained synthetic smoke-test model. Synthetic calibration
data does not establish detection accuracy.

The compiler log mentions VWW person-classification evaluation, but the
available output does not establish valid bounding-box evaluation or
detection accuracy.

Inference on the physical ColibryNPU board has not been tested.

## Generated Artifact

tests/compiler_smoke/detector_baseline/quantized/tiny_person_grid_detector_int8.tflite.nnpa

Generated model and calibration artifacts remain excluded from Git.
