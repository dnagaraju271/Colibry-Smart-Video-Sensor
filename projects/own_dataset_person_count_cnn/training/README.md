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

## Combined 182-frame scene dataset

The combined archive is:

`datasets/mobile_160x120/OWN_Dataset_182_Scene_Frames_160x120.zip`

It combines the 71 labeled images from the existing set and all 111 new
outdoor photographs. It contains 182 160x120 grayscale PNGs:

| Visible people | Images |
|---|---:|
| 0 | 1 (retained, skipped by the four-class trainer) |
| 1 | 72 |
| 2 | 66 |
| 3 | 29 |
| 4 | 14 |

The current trainer supports exact count classes 1, 2, 3 and 4. It skips
the one 0-person frame rather than assigning it to another class.

### Folder and naming structure

Each image is stored under its own count folder, for example
`2_person/I2_001_003.png`:

- `I2` = two visible people
- `001` = source scene group 001
- `003` = third frame in the scene, regardless of the class folder

Frame numbering restarts at 001 in each scene and continues across the
class folders. Original timestamp groups are preserved, so scene lengths
vary from 1 to 15 frames. They are not padded or forced to exactly eight
frames. Within a scene the order is unsuffixed filename first, then natural
numeric suffix order `(1)`, `(2)`, ... `(10)`. This is reproducible but
does not guarantee sub-second chronology.

### Entry / exit direction fields

The ZIP includes a `manifest.csv` with entry side, exit side, movement
direction and annotation status for every frame, plus `scene_directions.csv`
and `scene_summary.csv`.

Six longer new sequences (15.08.18–15.08.23) visibly show movement in both
directions; their scene-level entry and exit sides are `LEFT_AND_RIGHT`.
Short new groups and the legacy scenes are marked `UNKNOWN` when the
available stills do not establish direction reliably. These are not per-person
tracks: no persistent person IDs, boxes, or exact trajectories are annotated.

### Check and train from the combined archive

From the repository root:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --archive datasets/mobile_160x120/OWN_Dataset_182_Scene_Frames_160x120.zip \
  --output-dir projects/own_dataset_person_count_cnn/models/four_class_scene_experiment \
  --check-only

python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --archive datasets/mobile_160x120/OWN_Dataset_182_Scene_Frames_160x120.zip \
  --output-dir projects/own_dataset_person_count_cnn/models/four_class_scene_experiment \
  --epochs 40
```

The validation split is currently image-level rather than source-scene-held-out.
Because frames from one source scene are related, validation may be optimistic;
use a held-out collection of separate scenes before claiming generalization.

The existing board firmware still embeds the historical three-class NNPA.
A four-class host model needs H5 export, uint8 TFLite quantization, NNPA
compilation and new board-side output parsing before four-class board
inference can be claimed.

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
