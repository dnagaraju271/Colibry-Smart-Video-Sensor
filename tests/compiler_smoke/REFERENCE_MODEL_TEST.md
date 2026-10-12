# ColibryNPU Reference Model Compilation Test

Date: 2026-10-11

## Model
- Source: ASYGN SDK MLPerf Visual Wake Words example
- Model type: Person / non-person image classification
- Input: 96 x 96 x 3 RGB
- Output classes: 2

## Compiler environment
- ColibryNPU compiler: 0.1.2
- TensorFlow: 2.18.1
- Python: 3.12.15

## Result
- SavedModel loaded and processed by the compiler.
- Compiler model constraints check passed.
- Compiler reported configuration and data fit Colibry memory constraints.
- Generated NNPA file: models/reference/vww_trained_model_qat.nnpa
- File size: approximately 212 KB.

## Limitations
- Physical-board inference has not been tested.
- This model classifies images; it does not produce person bounding boxes.
- It is a reference model, not the project's final COCO person detector.
