# Colibry Smart Video Sensor — Development Progress

**Last updated:** 2026-11-15

## 1. Project Objective

Develop an embedded AI person-detection system using the ASYGN Colibry EvalboardTiny Rev1 and Colibry Neural Processing Unit (NPU).

The intended pipeline is:

Camera → Image Preprocessing → Colibry NPU → Person Detection → Bounding Box Decoding → UART Output

## 2. Completed Work

### Dataset and Model Development

- Prepared a person-detection dataset using COCO person images.
- Converted selected images and labels to grayscale 160 × 128 format.
- Developed a compact hardware-oriented grid-detector model.
- Recovered the original fully integer-quantized INT8 TFLite detector and verified its tensor metadata directly from the binary.
- Verified the detector input shape is `[1, 128, 160, 1]`, INT8, scale approximately 1/255, zero-point -128.
- Verified the detector output shape is `[1, 32, 40, 5]`, INT8, scale approximately 0.058669433, zero-point 87.

### NNPA Model Compilation

- Verified Colibry NPU compiler version 0.2.1.
- Recompiled a copy of the recovered original TFLite detector with the Colibry compiler.
- Reproduced the older project's 14,224-byte NNPA artifact byte-for-byte.
- Verified compiled artifact SHA-256:
  `fe9897ff4694328b22fb3fc91fba9097ca353a18365c197f196011ed14be9fd2`
- Compiler reported model ID `0x3F7B`, 1,078,012 inference cycles and 17,373,440 MACs.
- Compiler reported that the model configuration and data fit Colibry memory constraints.

A different 14,224-byte NNPA file exists in the current project clone with SHA-256 `e122d3f17509ad22b6a5bb3f8ba0b4db3775bded00834c991b7f91b6eb35a717`. Its hash differs from the reproducible artifact; do not assume the files are equivalent.

### Hardware and Flash Investigation

- Identified the FTDI Quad RS232-HS bridge as USB ID `0403:6043` in Ubuntu.
- Recovered a historical guarded flash-sector deployment recipe targeting sector 2 at address `0x20000`.
- The recipe expects a 65,536-byte sector image with the 14,224-byte NNPA at the beginning and the original 51,312-byte sector tail preserved.
- Recovered the expected backup and replacement-image SHA-256 values in the artifact-recovery report.
- Inspected project board scripts and found runtime inference/diagnostic scripts, but no project-level model uploader or flash-programming script. The historical flash procedure invokes the SDK's `colibry-flashtool.py`.

**Flash-state qualification:** earlier progress notes recorded that an NNPA model had been programmed and readback-verified. The 2026-11-15 audit recovered the deployment commands and expected hashes but not the historical execution output. A new read-only sector check could not start because the SDK flash tool reported `No FTDI SPI device found`. Ubuntu sees the FTDI USB bridge, but PyFtdi currently raises a USB string-descriptor/langid error. The current contents of sector `0x20000` are therefore not independently verified in this audit. No flash erase or programming was performed during the 2026-11-15 verification attempt.

See [Detector Artifact Recovery and Board Deployment Audit](DETECTOR_ARTIFACT_RECOVERY_2026-11-15.md) for source/compiled model hashes, tensor quantization, compiler metrics, historical flash layout, FTDI diagnostics, and evidence limitations.

## 3. Firmware and Camera Investigation

- Inspected the SDK camera board initializer and camera example.
- The board initializer documentation describes requested camera dimensions as expected dimensions after the ISP.
- The implementation selects QQVGA for dimensions no greater than 160 × 120 and QVGA for dimensions up to 320 × 240; a 160 × 128 request therefore selects the QVGA sensor-format branch.
- The SDK camera example demonstrates a camera-to-NNPA data-loaded interrupt flow at 160 × 120.
- Inspected ISP HAL facilities for cropper, Bayer-to-RGB, RGB-to-grayscale, and scaler configuration.
- The exact ISP configuration for delivering this detector's 160 × 128 grayscale input has not yet been established.
- Searches of project firmware did not identify a direct reference to model ID `0x3F7B` or the recovered NNPA filename.

## 4. Current Limitations

The following work is not yet complete or independently verified:

- Reliable PyFtdi SPI access for read-only flash inspection.
- Independent verification of the current contents of flash sector `0x20000`.
- Confirmation that running firmware is configured to use the recovered detector NNPA artifact.
- Camera sensor initialization and I2C verification for the final detector configuration.
- Camera frame capture and conversion to the exact 160 × 128 grayscale input.
- On-board NPU inference using live camera frames from the sensor.
- Correct output dequantization, grid decoding, and person bounding-box validation.
- End-to-end person/no-person tests on the physical board.

Compiler acceptance and tensor metadata verification do not demonstrate detection accuracy or successful live inference on the physical board.

## 5. Next Development Steps

1. Resolve PyFtdi USB descriptor/permission access without changing flash contents.
2. Read sector `0x20000` and compare its full hash against the known backup and replacement image.
3. Confirm the active firmware's NNPA model configuration and runtime input/output sizes.
4. Establish the camera/ISP configuration that delivers 160 × 128 grayscale data.
5. Run physical-board inference using the camera input.
6. Verify output quantization and grid decoding against host-side TFLite results.
7. Demonstrate person and empty-scene tests and report actual detection results.

## 6. Hardware and Software

- Hardware: ASYGN Colibry EvalboardTiny Rev1
- Processor/accelerator: Colibry NPU
- Development environment: Ubuntu VM running in UTM
- SDK: ASYGN Colibry NPU SDK
- Compiler: Colibry NPU compiler 0.2.1; RISC-V Core-V toolchain for firmware
- Model formats: INT8 TensorFlow Lite and compiled NNPA

## 7. Project Status

The original INT8 detector has been recovered and its tensor contract verified. Its NNPA compilation is reproducible, and the resulting 14,224-byte artifact has been matched byte-for-byte to the older project's compiled artifact.

The historical flash deployment recipe is documented, but the present contents of flash sector `0x20000` remain unverified because PyFtdi cannot currently access the FTDI SPI interface. Camera-to-NNPA inference and live person detection have not yet been demonstrated on the physical board.
