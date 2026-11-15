# Colibry Smart Video Sensor — Development Progress

## 1. Project Objective

Develop an embedded AI person-detection system using the
ASYGN Colibry EvalboardTiny Rev1 and Colibry Neural Processing Unit (NPU).

The intended pipeline is:

Camera → Image Preprocessing → Colibry NPU → Person Detection
→ Bounding Box Decoding → UART Output

## 2. Completed Work

### Dataset and Model Development
- Prepared a person-detection dataset using COCO person images.
- Developed a compact person-detection model for the Colibry target.
- Trained and exported an INT8 TensorFlow Lite model.
- Prepared the model for Colibry NNPA compilation.

### NNPA Model Compilation
- Compiled the INT8 model into the Colibry NNPA format.
- Generated the model artifact:
  `test/colibry_person_detector_int8.tflite.nnpa`
- The compiled model size is 14,224 bytes.
- Model ID: 0x3F7B.

### Hardware and Flash Preparation
- Connected the ASYGN Colibry EvalboardTiny Rev1.
- Identified the FT4232HP USB interface.
- Verified the four USB serial interfaces in the Ubuntu AMD64 VM.
- Created a backup of flash sector 0x20000.
- Programmed the compiled NNPA model into flash sector 0x20000.
- Read back the sector and verified its contents against the expected image.

### Firmware Investigation
- Inspected the Colibry SDK examples:
  - 10_uart_printf_blink
  - 20_hello_world
  - 30_mlperf_vww
  - 40_video_numbers_ble
  - 50_person_detector
- Identified the SDK person-detector example as a starting point
  for firmware integration.
- Investigated UART communication at 115200 baud.
- Confirmed that the UART interface can be opened from the host VM.

## 3. Current Limitations

The following work is not yet complete:

- Reliable JTAG communication and SRAM firmware loading.
- Integration of the trained NNPA model into the target firmware.
- Camera sensor initialization and I2C verification.
- Camera frame capture and image preprocessing.
- On-board NPU inference using live camera frames.
- Correct decoding of person predictions and bounding boxes.
- End-to-end person/no-person testing on the physical board.

The UART connection has been opened successfully, but the latest
30-second capture did not receive firmware output.

The camera-to-NPU person-detection pipeline has not yet been
validated on the physical board.

## 4. Next Development Steps

1. Adapt the SDK person-detector example for the trained NNPA model.
2. Resolve the JTAG communication issue.
3. Build firmware for EVBTINY.
4. Load and execute the firmware from SRAM for initial testing.
5. Initialize the camera sensor and verify image capture.
6. Preprocess camera frames for the model input.
7. Execute inference on the Colibry NPU.
8. Decode person detections and bounding boxes.
9. Send inference results through UART.
10. Demonstrate detection with a person in front of the camera
    and an empty-scene test.

## 5. Hardware and Software

- Hardware: ASYGN Colibry EvalboardTiny Rev1
- Processor/accelerator: Colibry NPU
- Development environment: Ubuntu 24.04 AMD64 VM
- SDK: ASYGN Colibry NPU SDK
- Compiler: RISC-V Core-V GCC toolchain
- Model format: INT8 TFLite and compiled NNPA

## 6. Project Status

Model preparation and NNPA compilation have been completed.
The NNPA model has been programmed into flash and verified.

The project is currently in the firmware integration and hardware
validation phase. The final objective is a working real-time
camera-to-NPU person-detection demonstration.
