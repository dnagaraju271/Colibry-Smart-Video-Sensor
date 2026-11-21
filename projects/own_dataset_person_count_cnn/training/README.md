# Host training and compatibility export

Training runs on the Ubuntu host using TensorFlow. The Colibry NNPA is an inference accelerator; it does not train model weights.

## Current run: deduplicated 71 + 111 scene dataset (2026-11-21)

The current combined dataset is:

`datasets/mobile_160x120/OWN_Dataset_71plus111_Deduplicated_Scene_Frames_160x120.zip`

The two source collections contained 182 entries (the original 71 plus 111
new timestamped outdoor photographs). Exact pixel comparison after
160x120 grayscale normalization found that **all 71 legacy images duplicate
images already present in the newer 111-image collection**. The old 71
frames contain only three distinct grayscale pixel patterns, all of which
are already represented in the newer archive. Therefore the correct
deduplicated union contains **111 unique frames**, not 182. The 111 newer
timestamped frames are retained because they preserve the walking-scene
sequence context. The 71 old duplicates are mapped to retained files in
`duplicates_removed.csv`.

| Visible people | Unique images | Training handling |
|---|---:|---|
| 0 | 1 | Retained in `0_person`; skipped by the four-output model |
| 1 | 13 | Included |
| 2 | 58 | Included |
| 3 | 25 | Included |
| 4 | 14 | Included |
| **Total** | **111** | **110 used for four-class training** |

Four-person examples are included as a full fourth class. The existing model
has outputs for exact counts 1, 2, 3, and 4. The one empty-scene image is
preserved separately and is not treated as a one-person example.

### Resolution, folders, and scene/frame names

All images are 160x120, 8-bit grayscale PNG (mode L), stored in their
corresponding class folders: `0_person/` through `4_person/`.

The filename convention is:

`I{count}_{scene}_{frame}.png`

Frame numbers increase across **all count folders** inside a scene. Example
from the first timestamped scene:

- `2_person/I2_001_001.png`
- `1_person/I1_001_002.png`
- `2_person/I2_001_003.png`
- `1_person/I1_001_004.png`

Here `I2` means two visible people, `001` identifies the scene, and the
last number is the frame's position in the scene. This matches the requested
walking-sequence layout: each image lives in its class folder while the
frame number follows the entire scene, not just one class.

There are **14 timestamped scenes** in this deduplicated collection. Each
scene keeps its actual number of frames (1–15); scenes are not padded or
forced to exactly eight images. Within a timestamp group, the WhatsApp
filename collision suffix is sorted numerically (base filename, then
`(1)`, `(2)`, etc.). This gives a reproducible ordering but cannot recover
exact sub-second chronology.

### Entry, exit, and movement direction

`scene_directions.csv` contains one entry/exit record per scene, and the
same scene-level fields are copied into each manifest row:

- Six longer reviewed scenes (15.08.18–15.08.23): `LEFT_AND_RIGHT` entry
  and exit, `BIDIRECTIONAL` movement.
- Eight shorter scenes: `UNKNOWN`, because still images do not establish
  a defensible entry-to-exit path.

These labels describe coarse scene-level movement only. The photographs do
not provide persistent person IDs or per-person tracks, bounding boxes, or
exact individual trajectories. Do not interpret `LEFT_AND_RIGHT` as a
person-level tracking annotation.

### Check and train

The current host model retains exactly two trainable layers
(`Conv2D(4, 3x3, ReLU)` and `Dense(4)`) and downsamples 160x120 images to
the Colibry input shape 40x30x1 using Lanczos resize.

The trainer uses class weights and a source-scene-held-out
`StratifiedGroupKFold` validation split, so adjacent frames from a scene
cannot leak between training and validation. With only 14 scenes and
relatively few 3-/4-person scenes, validation remains preliminary.

First run the read-only archive/model check:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py --check-only
```

Then train into a separate directory:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py \
  --epochs 40 \
  --batch-size 8 \
  --output-dir projects/own_dataset_person_count_cnn/models/four_class_71plus111_group_split
```

Outputs are `own_count_cnn.keras`, `own_count_cnn.tflite`,
`best.weights.h5`, and `experiment_report.json`.

The camera firmware still embeds the historical three-class NNPA. Training
does not update the board model. A new four-class model must be exported,
quantized, compiled to NNPA and integrated into the firmware before the
Colibry can output four-person predictions.
 
## Earlier additional-only 111-image archive (historical input)

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

## Earlier 182-entry combined archive (superseded; contains exact duplicates)

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
