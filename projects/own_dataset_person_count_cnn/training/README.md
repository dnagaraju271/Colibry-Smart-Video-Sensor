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

## Four-class dataset and training

The additional archive is:
`datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111_4class.zip`

It contains 111 manually reviewed outdoor photographs processed to 160x120
8-bit grayscale PNG using Pillow Lanczos resizing. The manifest maps every
processed image to its source filename and manually counted visible people.

| Visible people | New images | Training handling |
|---|---:|---|
| 0 | 1 | Retained in pending folder; skipped |
| 1 | 13 | Included |
| 2 | 58 | Included |
| 3 | 25 | Included |
| 4 | 14 | Included |

The existing 71-image archive contributes 59 / 8 / 4 examples for classes
1 / 2 / 3. Combined with the new archive, the training-eligible distribution
is **72 / 66 / 29 / 14** for 1 / 2 / 3 / 4 people (**181 images total**).
The one zero-person image remains pending. There is no five-or-more class.

The 14 four-person images come from six source-time groups and include
near-consecutive frames. They are not 14 independent scenes. The current
split is image-level stratified rather than source-group-held-out, so
validation metrics may be optimistic. Use a genuinely separate scene-level
test set before claiming generalization.

### Check and train the combined four-class model

From the repository root, first inspect the archive and class distributions:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --check-only \
  --additional-archive datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111_4class.zip
```

Train into a separate four-class experiment directory so existing model
artifacts are not overwritten:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --epochs 40 \
  --additional-archive datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111_4class.zip \
  --output-dir projects/own_dataset_person_count_cnn/models/four_class_experiment
```

The updated model keeps the same 40x30x1 grayscale input and two trainable
layers (Conv2D and Dense), but the Dense layer now emits **four logits**
for `1_person`, `2_person`, `3_person`, and `4_person`. The 4-person
class is still small (14 examples); class weighting is applied, but it cannot
replace collecting more independent four-person scenes.

The four-class host model is not yet the model embedded in the existing board
firmware. After training, perform legacy HDF5 export, uint8 TFLite
quantization, NNPA compilation, then update and validate the board artifact.
The current embedded 19,392-byte NNPA remains the old three-class model.

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
