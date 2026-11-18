# Colibry EvalboardTiny SRAM inference firmware

This directory contains board firmware for evaluating the team's own 3-class CNN on the Colibry NNPA. The model is trained on the Ubuntu host; the NNPA executes inference only.

## Current model limits

- Whole-image classes: `1_person`, `2_person`, `3_person`.
- No `0_person` class, no `>3` class, and no bounding-box output.
- Dataset: 59 one-person, 8 two-person, 4 three-person images. All 4 three-person images originate from one source group.
- The existing model misclassified two of the three initial physical-board samples. Testing all 71 images is an inference audit, not an independent accuracy test, because these images were used in model development/training.

## Firmware files

- `main.c` embeds the NNPA model and up to 16 preprocessed images, copies NNPA into NNPA SRAM, runs each image and prints machine-readable `RESULT` records.
- `generate_board_data.py` verifies the NNPA size/SHA-256 and selects images from the ZIP manifest.
- `Makefile` builds for `EVBTINY`; pass `BATCH_START` and `BATCH_SIZE` to select a batch.

## Build one batch

```bash
make prepare-data BATCH_START=0 BATCH_SIZE=16
make check-inputs
make all board=EVBTINY
```

`BATCH_START` is a zero-based offset into the 71 labelled images. `BATCH_SIZE` must be 1..16. Three pending zero-person images are skipped.

Five batches cover the archive:

| Batch | Start | Size |
|---|---:|---:|
| 0 | 0 | 16 |
| 1 | 16 | 16 |
| 2 | 32 | 16 |
| 3 | 48 | 16 |
| 4 | 64 | 16 (generator takes remaining 7) |

Before generating the next batch, save the previous binary under a distinct name, such as `build/batches/batch_00.bin`. Rebuild after each `make prepare-data` call.

## Run a batch from SRAM only

Use the Evaluation Kit Wait Start sequence: hold PWR_OFF, hold the EvalboardTiny User Button, release PWR_OFF, then release the User Button.

Capture UART `/dev/ttyUSB2` at 115200 baud before invoking the loader:

```bash
stty -F /dev/ttyUSB2 115200 raw -echo -ixon -ixoff cs8 -cstopb -parenb
cat /dev/ttyUSB2 > build/batch_00_uart.log &
UART_PID=$!
sleep 2
sudo "$HOME/colibry-flash-env/bin/python" \
  "$HOME/colibryNPU_SDK/riscv_runtime/tools/bin_loader/colibry-sramtool.py" \
  "$PWD/build/batches/batch_00.bin" 0x10000000 --run \
  -p "ftdi://ftdi:4232hp:5:2/1"
sleep 30
kill "$UART_PID" 2>/dev/null || true
wait "$UART_PID" 2>/dev/null || true
```

Each `RESULT,index,expected,predicted_id,logit0,logit1,logit2` record contains a dataset index, the true manifest label, a predicted class ID (0=1_person, 1=2_person, 2=3_person), and raw uint8 logits.

**Do not use `toflash`, `make nnpa`, or the flash programming tool for this evaluation.** The SRAM loader writes firmware only to volatile SRAM at `0x10000000`; the firmware copies NNPA into NNPA SRAM. External flash sector `0x20000` remains unchanged.

## Initial hardware run

The first 3-image board run loaded model ID `0x723E`, accepted 1200-byte input tensors, and completed all three inference calls. Host and board argmax predictions agreed for all three images, but only one true label was classified correctly.

The full 71-image run measures outputs from the current model on the existing dataset. It does not demonstrate independent generalization.

## Persistent deployment and live camera

The present SRAM demo does not capture camera frames. Flashing only the NNPA file will not create a live detector. A persistent application requires bootable firmware that configures the camera, preprocesses frames to the trained 40x30 grayscale input, loads the NNPA model from a verified flash location, and runs inference.

Do not replace sector `0x20000` yet. It currently contains the historical 14,224-byte detector NNPA followed by the preserved 51,312-byte original tail. The new NNPA is 19,392 bytes and overlaps a larger portion of that tail. Preserve both verified backups while a flash layout and camera pipeline are designed.

