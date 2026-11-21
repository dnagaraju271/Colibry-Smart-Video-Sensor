# Own-dataset person-count CNN

## Latest standalone training collection — DATASET185 (2026-11-21)

The latest 185-image outdoor collection is stored locally as
`datasets/mobile_160x120/DATASET185_160x120_Grayscale_Scene_Organized.zip`.
It contains 160x120 8-bit grayscale PNGs arranged by visible-person count and
source scene.

| Visible people | Images | Current four-class training |
|---|---:|---|
| 0 | 5 | Preserved, skipped |
| 1 | 75 | Included |
| 2 | 63 | Included |
| 3 | 27 | Included |
| 4 | 15 | Included |

The current model trains on 180 images for exact counts 1–4. The five
zero-person samples are not part of the existing four-output classifier.
The host trainer downsamples 160x120 to the Colibry model input 40x30x1,
uses class weights, and holds out complete source scenes for validation.
It retains exactly two trainable layers (Conv2D and Dense). Run instructions
and the split details are in `training/README.md`.

**Deployment is not complete:** the embedded NNPA in the current camera
firmware is still the previous three-class model. The latest live-camera
diagnostic also failed the ISP scaler PSB check, and its captured values
were only 0–5. A new four-class NNPA must be trained/exported/compiled, and
the camera frame pipeline must produce a valid image before live predictions
can be trusted. Do not use or overwrite the old NNPA/flash backups as part
of this host-training step.


This is the dedicated project for the team's own 3-class grayscale person-count CNN and its Colibry NNPA inference demonstration.

## Project organization

- `training/` — host-side TensorFlow training and legacy Keras compatibility export.
- `model_export/` — INT8 TFLite / Colibry NNPA artifact notes and hashes.
- `board_firmware/` — EvalboardTiny firmware for SRAM-only NNPA inference tests.
- Dataset archive: `datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip`.

The separate `simple_person_cnn/` folder remains the older binary `person` / `no_person` CNN exercise. The professor assignment index remains in `experiments/professor_demo/` and links to this project for Task 2.

## Dataset currently available

| Class | Labeled images |
|---|---:|
| 1_person | 59 |
| 2_person | 8 |
| 3_person | 4 |
| **Total** | **71** |

The archive also contains three zero-person originals in `pending_for_later_originals`; those are not included in the current 3-class training. All four 3-person images come from one source group, and the earlier train/validation split shared five source groups. Current metrics are preliminary and optimistic.

## Additional outdoor collection (2026-11-20) — four-class revision

The additional archive is
`datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111_4class.zip`.
All 111 photographs were reviewed and processed to 160x120 8-bit grayscale.

| Visible people | New images | Handling |
|---|---:|---|
| 0 | 1 | Retained pending; excluded from current classifier |
| 1 | 13 | Included |
| 2 | 58 | Included |
| 3 | 25 | Included |
| 4 | 14 | Included |

Combined with the original 71 labeled images, the four-class training
distribution is **72 / 66 / 29 / 14** for 1 / 2 / 3 / 4 people (181 images).
The zero-person sample remains pending. The four-person samples span six
source-time groups and include near-consecutive frames; they are not
independent scenes.

The training script and legacy Keras exporter have been updated for four
classes. The 40x30x1 input and exactly two trainable layers are retained, with
the Dense output changed from three to four logits. See
`training/README.md` for the check-only and training commands.

**Important:** the NNPA bitstream currently embedded in the camera firmware
is still the old three-class artifact. A new four-class model must be trained,
quantized, compiled to NNPA, and verified before changing board output parsing
or claiming four-class inference. Do not use the old NNPA for 4-person
classification.

## Combined 182-frame scene dataset

The reorganized dataset combines the original **71 labeled images** and all
**111 new outdoor photographs** into one archive:

`datasets/mobile_160x120/OWN_Dataset_182_Scene_Frames_160x120.zip`

It contains 182 grayscale PNG images at 160x120, in five folders:
`0_person/`, `1_person/`, `2_person/`, `3_person/`, and `4_person/`.

| Visible people | Images | Use |
|---|---:|---|
| 0 | 1 | Preserved; excluded from the current four-output model |
| 1 | 72 | Four-class training |
| 2 | 66 | Four-class training |
| 3 | 29 | Four-class training |
| 4 | 14 | Four-class training |

The four-class training set therefore has 181 images. The 0-person photo is
retained rather than being mislabeled as a one-person sample.

### Scene and filename convention

File names use `I{count}_{scene}_{frame}.png`, with each frame stored in its
matching class folder. For example:

- `1_person/I1_001_001.png` — one person, scene 001, frame 001
- `1_person/I1_001_002.png` — one person, scene 001, frame 002
- `2_person/I2_001_003.png` — two people, scene 001, frame 003
- `3_person/I3_001_005.png` — three people, scene 001, frame 005
- `4_person/I4_001_008.png` — four people, scene 001, frame 008

The scene ID follows the original WhatsApp timestamp/source group. Frame IDs
restart at 001 for each scene and increase across all class folders in that
scene. Frames are ordered by natural numeric filename suffix: the unsuffixed
file first, then `(1)`, `(2)`, ... `(10)`. This is a reproducible ordering,
but the WhatsApp collision suffix does not prove exact capture chronology.

The 20 original source groups contain a variable number of frames (1–15).
They are kept intact; groups are not padded or arbitrarily split to make every
scene exactly eight frames. Eight is an example sequence length, not a fixed
scene-size rule.

### Entry and exit direction

The archive includes `scene_directions.csv` and per-frame direction fields in
`manifest.csv`:

- The six longer reviewed new outdoor sequences (15.08.18–15.08.23) show
  people crossing in both directions. Their scene-level entry and exit sides
  are marked `LEFT_AND_RIGHT`.
- Short new groups and all six legacy groups are marked `UNKNOWN` where the
  still sequence does not establish a reliable entry-to-exit direction.

These are scene-level observations only. The data does not contain persistent
person IDs, bounding boxes, or per-person trajectories. Exact individual
movement paths would require frame-by-frame track annotation or video.

The ZIP also includes:
- `manifest.csv` — source filename, count, scene/frame ID, class folder,
  output name, entry/exit labels, ordering basis and SHA-256
- `scene_directions.csv` — one row per source scene
- `scene_summary.csv` — per-scene frame count and class distribution

The archive was verified to contain 182 images; every PNG is 160x120, grayscale
mode L, and matches its manifest filename and SHA-256. ZIP integrity passes.

### Check and train the four-class model

The current host trainer has four output classes, but the NNPA file currently
embedded in the camera firmware is still the old three-class bitstream.

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

Do not use the old three-class NNPA bitstream to claim four-person classification.
A newly trained four-class model must be exported, quantized, compiled and
validated on the board before replacing the current artifact.

## Existing three-class model baseline (historical)

The following specifications describe the previously trained three-class model, not the pending four-class retrain.

- Input tensor: `[1, 30, 40, 1]` grayscale.
- Trainable layers: `Conv2D(4, 3x3, ReLU)` and `Dense(3)`; Flatten is a non-trainable shape operation.
- Trainable parameters: 14,443.
- Classes: `1_person`, `2_person`, `3_person`.
- Host validation: 80% image-level accuracy, but recall was 0 for both 2-person and 3-person validation examples.

This model is not yet a reliable people counter. More independently captured 2-person and 3-person examples are needed before retraining and making accuracy claims.

## Train on Ubuntu host

From the repository root:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py --check-only
```

After reviewing the dataset check, train into the project-local output directory:

```bash
python3 projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py --epochs 40
```

Default output: `projects/own_dataset_person_count_cnn/models/host_experiment/`. The previous 2026-11-16 experiment artifacts remain in the local legacy path `simple_person_cnn/model/own_count_experiment/`; the reorganization does not delete or move those untracked local files.

See `training/README.md` for the full training and export workflow.

## Colibry board inference

The Colibry NNPA runs inference; it does not train the weights. Host training is converted to uint8 TFLite and compiled to NNPA format before board use.

On 2026-11-18 the following was demonstrated on the physical EvalboardTiny:

- NNPA model loaded from the temporary SRAM application and passed its configuration-check path.
- Three 40x30x1 uint8 inputs were sent through the 8-bit input loader.
- All three NNPA inference calls completed, producing a 3-value output.
- Host INT8 TFLite and board class predictions agreed for all three tested examples.
- One-person image predicted 1_person; two-person predicted 1_person; three-person predicted 2_person. Thus only 1/3 examples matched their labels.
- Raw uint8 outputs differed slightly between host and board; max absolute difference was 11 output levels. Class argmax agreed for the three samples.

This proves the limited hardware inference path, not useful model accuracy. Exact board raw outputs and hashes are recorded in `docs/SESSION_HANDOFF_2026-11-18.md`.

### Build the SRAM-only firmware

From this directory:

```bash
cd projects/own_dataset_person_count_cnn/board_firmware
make prepare-data
make check-inputs
make all board=EVBTINY
```

To run, use the Evaluation Kit manual's Wait Start sequence and the SDK SRAM loader. The system Python in the current VM lacks PyFtdi, so invoke the loader with `~/colibry-flash-env/bin/python` as shown in `board_firmware/README.md`.

**Do not use `toflash` or `make nnpa` for this SRAM demonstration.** The app embeds the NNPA bitstream in the firmware binary and writes it to NNPA SRAM at runtime. External flash sector `0x20000` must remain unchanged.

## Flash backup status

The original 64 KiB sector backup and current sector image have been recovered and verified. The original tail (51,312 bytes) is preserved byte-for-byte after the historical detector NNPA prefix. Both backup files are local and must be retained. See the session handoff for exact paths and SHA-256 values.

## Scope

This project covers the user's own dataset, a compact two-trainable-layer CNN, and the Colibry board inference demonstration. It does not add object tracking, bounding boxes, dashboards, or unrelated features.


## Full physical board audit of current 71-image dataset (2026-11-18)

The compact CNN NNPA was executed on the physical Colibry EvalboardTiny using five SRAM-only firmware batches. All 71 labelled images were processed; all UART records were present with no duplicates or missing indices.

**Observed results on these existing development images: 68/71 correct (95.77%).**

| Actual / Predicted | 1_person | 2_person | 3_person |
|---|---:|---:|---:|
| 1_person (59) | 59 | 0 | 0 |
| 2_person (8) | 2 | 6 | 0 |
| 3_person (4) | 0 | 1 | 3 |

| Class | Precision | Recall | F1 |
|---|---:|---:|---:|
| 1_person | 96.72% | 100% | 98.33% |
| 2_person | 85.71% | 75% | 80.00% |
| 3_person | 100% | 75% | 85.71% |

All three observed errors are undercounts: two 2_person samples were called 1_person, and one 3_person sample was called 2_person.

**Critical evaluation caveat:** this is a complete inference audit on the current development dataset, not independent test accuracy or a real-world generalization result. The images were already used during model development/training and validation; the original image-level split leaks source groups, and all four 3_person images come from one source group. Class distribution is imbalanced at 59/8/4.

The board execution test confirms the compiled model executes on the Colibry NNPA for all 71 inputs. Current model capabilities remain limited to whole-image count classification for 1, 2, or 3 people. It has no 0-person or >3 output, does not produce boxes, and the test firmware does not acquire live camera frames.

Next: collect more independent 2-person and 3-person scenes, label pending zero-person originals, retrain on host, compile a new NNPA, and validate again. Live detection needs a camera-to-grayscale-40x30-to-NNPA firmware path. Do not permanently flash the new model or overwrite sector 0x20000 until that runtime and flash layout are reviewed.

Full raw UART logs and CSV `build/all_71_board_results.csv` remain on the user's Ubuntu VM under `projects/own_dataset_person_count_cnn/board_firmware/build/`.

## Live camera inference — feasibility and scope

Training the CNN on the Colibry board is not supported by this NNPA workflow. Train model weights on the Ubuntu host with TensorFlow, export/quantize to uint8 TFLite and compile to NNPA. The Colibry NNPA runs inference.

The SDK contains a camera route: `ColibryBoard::camera_init()` detects supported EIS316, HM0360 or PAG7920LT cameras, and `get_image_to_nnpa()` triggers a single camera frame and waits for the NNPA data-loaded event. `global_init()` disables sensor power by default, so camera firmware must explicitly enable the sensor rail before initialization.

The model requires 40x30 grayscale uint8 input (1200 bytes). The EvalboardTiny camera sensor path must be configured through CAMIF/ISP grayscale and scaler blocks to generate this exact input. Camera frames are normally larger (e.g. QQVGA 160x120), so 4x downscaling in both axes and exact pixel/preprocessing parity must be validated.

Current static-image batch firmware does not do live capture. Next work, pending confirmation of a connected supported camera module: first build an SRAM-only sensor-detection test; then a one-frame capture and ISP-to-40x30 verification; then integrate NNPA and continuously classify incoming frames. The output is a whole-image class (1/2/3 people), not bounding boxes, and there is no zero-person class.

Do not permanently flash the model yet; sector 0x20000 contains the prior detector bitstream and preserved original tail. Keep both flash backups intact until live SRAM inference is proven and a safe flash layout is established.

## Camera bring-up entry point

A minimal SRAM-only sensor detection probe is available at `camera_probe/`. It powers the sensor rail and invokes the ASYGN SDK auto-detection for EIS316, HM0360 and PAG7920LT. Build and SRAM-run it before implementing frame capture. No NNPA inference or flash operation occurs in this probe.

After a module is detected, configure camera frame capture and ISP grayscale/downscale to match the CNN's required 40x30 uint8 tensor (1200 bytes). The camera probe has not yet been built or run by the user; its detection result must be confirmed over UART.

## One-frame live camera NNPA test

The PAG7920LT was detected and initialized on the physical EvalboardTiny. A new SRAM-only one-frame camera-to-ISP-to-NNPA test has been added under [`camera_live_test/`](camera_live_test/README.md).

The test configures the confirmed PAG7920LT as 160x120 RAW8, enables only the ISP scaler at 4x in both axes (0x400 fixed-point), and expects a 40x30x1 uint8 NNPA input of 1200 bytes. The mono RAW path bypasses cropper, Bayer2RGB and RGB2GRAY; scaler remains enabled. The code validates NNPA IFMAP dimensions, packing metadata, ISP scaler input dimensions and ISP error flags before inference.

It embeds the verified 19,392-byte NNPA bitstream into an SRAM firmware image, triggers a single camera frame, waits with a timeout for `EVT_DATA_LOADED`, prints the exact 40x30 input tensor over UART, and runs one inference. A host script can reconstruct the frame dump as a PNG.

**This new live frame test has only been added to GitHub; it has not yet been built or run on the user's VM/board.** The first next step is pull, `make prepare-model`, `make all board=EVBTINY`, and inspect build size. It must run through the SRAM loader only; no `toflash` or flash programming. The scaler sampling output may not exactly match the host's Pillow Lanczos preprocessing, so inspect the reconstructed PNG and compare predictions empirically.

