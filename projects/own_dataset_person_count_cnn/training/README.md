# Host training and compatibility export

Training runs on the Ubuntu host using TensorFlow. The Colibry NNPA is an inference accelerator; it does not train model weights.

## Train

From the repository root:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py --check-only
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py --epochs 40
```

Defaults:
- Dataset: `datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip`.
- Output: `projects/own_dataset_person_count_cnn/models/host_experiment/`.
- Input: 160x120 grayscale Lanczos resize to 40x30; float32 scaled to [0,1].
- Classes: `1_person`, `2_person`, `3_person`.
- Exactly two trainable layers: Conv2D and Dense.

The validation split is image-level stratified, not source-group held out. The six source groups overlap between train/validation; in fact all four 3-person images originate from one group. Treat validation metrics as preliminary only.

## Add the 20 November 2026 outdoor collection

The additional archive is:
`datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111.zip`

It contains 111 manually counted outdoor photographs processed to 160x120
8-bit grayscale PNG using Pillow Lanczos resizing. The collection includes:

| Visible people | Images | Training handling |
|---|---:|---|
| 1 person | 13 | Included |
| 2 people | 58 | Included |
| 3 people | 25 | Included |
| 0 people | 1 | Retained in pending folder; skipped |
| 4 people | 14 | Retained in pending folder; skipped |

Only the existing model classes 1/2/3 are used for training. The model has no
zero-person or four-person class. Labels are recorded in the archive manifest.
These are near-consecutive frames from 14 source-time groups, not 111
independent scenes; image-level validation can be optimistic.

To check the combined original 71-image archive and the new collection:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --check-only \
  --additional-archive datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111.zip
```

To train on the combined eligible images:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --epochs 40 \
  --additional-archive datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111.zip
```

With the existing 71 labeled images, this adds 96 eligible examples and gives
a combined class distribution of 72 one-person, 66 two-person, and 29
three-person images. The one zero-person and fourteen four-person examples are
preserved but excluded from this three-class run.

## Legacy Keras export

Use the compiler-compatible exporter after training:

```bash
~/colibry-cnn-env/bin/python projects/own_dataset_person_count_cnn/training/export_legacy_keras_for_colibry.py
```

Default source model: `projects/own_dataset_person_count_cnn/models/host_experiment/own_count_cnn.keras`.
Default H5 output: `~/models/edgeguard_own_count/own_count_cnn_legacy.h5`.

The exporter verifies the same two trainable layers, parameter count, saved/reloaded weights, and prediction parity.

## Historical experiment

The previous trained run (2026-11-16) and untracked output files are still present on the user's VM under `simple_person_cnn/model/own_count_experiment/`. The previous NNPA compiled artifact remains at `~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa`. Reorganization does not delete or overwrite these files.
