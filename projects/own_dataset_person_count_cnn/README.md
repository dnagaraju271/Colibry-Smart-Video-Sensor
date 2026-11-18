# Own-dataset person-count CNN

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

## Model

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
