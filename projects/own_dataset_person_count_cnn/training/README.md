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
