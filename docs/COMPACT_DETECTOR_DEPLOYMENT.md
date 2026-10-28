# Compact Colibry detector: train, export, and host-test

This is the intended path for the compact grayscale grid detector. It is separate
from the current YOLOv8-derived backbone feature-map experiment.

## 1. Validate and train

Use a processed dataset with images/{train,val} and labels/{train,val}
directories. Each image must be 160x128 pixels and each label must be a YOLO
text file with class 0 representing a person. Include empty label files for
known-empty images.

From the repository root, using the environment with TensorFlow 2.18.x,
tf_keras, NumPy, and Pillow:

~~~bash
python scripts/model/train_colibry_person_detector.py \
  --dataset "$HOME/datasets/coco_person_hardneg/processed" \
  --output-dir "$HOME/models/colibry_person_detector" \
  --check-only
~~~

If the contract check passes, repeat without --check-only and review
training_report.json before using the model.

## 2. Export fully-int8 TFLite

The exporter uses training images as representative data and writes the quantized
model plus a JSON report containing input/output shapes and quantization metadata.
It never accesses or flashes the board.

~~~bash
python scripts/model/export_colibry_detector_tflite.py \
  --model "$HOME/models/colibry_person_detector/colibry_person_detector.keras" \
  --dataset "$HOME/datasets/coco_person_hardneg/processed" \
  --output "$HOME/models/colibry_person_detector/colibry_person_detector_int8.tflite"
~~~

The exporter requires fully integer built-in TFLite operators. If conversion fails,
keep the full error; do not silently relax the integer-only requirement.

## 3. Validate a real image on the host

~~~bash
python scripts/model/run_colibry_tflite_detector.py \
  --model "$HOME/models/colibry_person_detector/colibry_person_detector_int8.tflite" \
  --image /path/to/test_person.jpg \
  --output-image "$HOME/models/colibry_person_detector/test_detection.png" \
  --output-json "$HOME/models/colibry_person_detector/test_detection.json"
~~~

The script applies the input quantization parameters from the TFLite model, converts
its int8 grid output back to real-valued logits, decodes boxes, applies NMS, and
writes an annotated image plus JSON detections. Review both positive person images
and known-empty images. A trained model can still have false positives or misses.

## 4. Only then test compiler and board integration

Run scripts/model/check_yolo_colibry_compat.py against the generated TFLite
model before invoking the explicitly configured compiler command. Compiler
acceptance does not prove physical-board inference. The current firmware and NNPA
artifact used in the YOLOv8 experiment produce a [1,64,16,20] backbone feature map,
not the [1,32,40,5] grid detector output expected by the compact model. Do not
interpret one model's output using the other's decoder.

The TFLite host test is a necessary intermediate checkpoint, not proof that the
physical Colibry EvalboardTiny is detecting people. Board deployment still needs
a compatible compiled artifact, correct input/output memory handling, and camera
frame integration.
