# Synthetic Model Compiler Smoke Test

Date: 2026-10-12
Compiler: Colibry NPU compiler 0.1.2
TensorFlow: 2.18.1

## Model
- Input: 160 x 128 x 1 grayscale
- Output: 5 numeric values
- Parameters: 381
- Purpose: compiler compatibility test only
- Training: not performed

## Results
- INT8 post-training quantization: passed
- TFLite supported-layer constraint check: passed
- NNPA output generated: yes
- NNPA output file size: 688 bytes
- Reported inference cycles: 208064
- Reported estimated energy: 11.3 uJ per inference
- Reported Colibry memory constraints: passed

## Limitations
- Synthetic, randomly initialized model; not a trained detector.
- Five outputs have not been trained or validated as bounding boxes.
- Physical Colibry NPU inference has not been tested.
- Compiler-reported estimates are not measured hardware performance.
