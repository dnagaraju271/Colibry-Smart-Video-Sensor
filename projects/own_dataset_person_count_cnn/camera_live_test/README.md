# PAG7920LT live camera capture and inference

## Current blocker — 2026-11-21

The latest physical-board UART run did **not** produce a valid camera frame or
a trustworthy prediction:

- `NNPA_DATA_LOADED: 1200 bytes` was reported, but this alone does not prove
  that the input pixels are valid.
- `ISP_INT_STATUS: 0x00000028`, with `ISP_SCALER_PSB_ERR=1`.
- `CAMIF_INT_STATUS: 0x0000003c`, with `CAMIF_DATA_FIFO_OVERFLOW=1`.
- The reconstructed 40x30 frame had pixel values only from 0 to 5.
- Firmware ended with `CAMERA_CAPTURE_RESULT: FAIL (5)`.

Do not bypass the ISP error check or treat this frame as a valid test image.
Resolve the camera/ISP stream issue and visually verify a frame before
enabling live inference.

The firmware source in this directory also still embeds the historical
three-class NNPA and has `CLASS_COUNT=3`. It cannot report the new
four-class 1–4 person model. The host model must first be trained, exported
and compiled to a new NNPA; then the firmware model header, class count and
output parser must be updated together. Preserve the existing NNPA and flash
backups; do not program external flash for this SRAM test.


## Current firmware mode — 2026-11-20

The current `main.c` is configured for the physical PAG7920LT camera
(`CAMIF_PATTERN_TEST=0`). It runs a continuous video-mode frame-counter
test at 150 requested FPS for five seconds, prints CAMIF and ISP register
snapshots, and returns.

**This current firmware mode does not execute the single-frame capture,
NNPA input-loading, or inference path described below.** The frame-rate
measurements prove CAMIF frame-counter progression only. They do not prove
that a complete image was retrieved or that the ISP scaler produced valid
image data.

See [continuous camera FPS tests](docs/continuous-camera-fps-tests-2026-11-20.md)
for the measured results and remaining limitations.


This SRAM-only test initializes the confirmed PAG7920LT camera, scales a frame through the Colibry ISP, loads the own-count CNN from an embedded NNPA bitstream, and runs one live inference.

## Verified hardware configuration

- PAG7920LT sensor detected by the ASYGN SDK.
- PAG7920LT SDK QQVGA mode: 160x120 pixels.
- CAMIF: RAW, 8-bit, one channel.
- ISP: cropper bypass; Bayer2RGB bypass; RGB2GRAY bypass (monochrome sensor); scaler enabled.
- Scaler input: 160x120x1; scaler output: 40x30x1 with exact 4x X/Y scaling (`0x400` 4.8 fixed point).
- NNPA input: `[1,30,40,1]` uint8, 1200 bytes.
- Firmware validates the NNPA input metadata: width 40, height 30, depth 1, unflattened, packing level 3.
- After a frame arrives, it validates ISP scaler input metadata is 160x120 with one channel and checks ISP PSB error flags before using the frame.

The Colibry product specification documents CAMIF-to-ISP Pixel Streaming Bus, monochrome RAW mode, ISP scaler configuration and PSB transfer to the NNPA input loader.

## Build

From this directory:

```bash
make prepare-model
make all board=EVBTINY
riscv32-corev-elf-size build/own_count_camera_live/own_count_camera_live
sha256sum build/own_count_camera_live.bin
```

The generator pins NNPA size to 19,392 bytes and SHA-256 to `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e`.

## Run from SRAM only

Use the Evaluation Kit Wait Start sequence if required. Capture UART `/dev/ttyUSB2` at 115200 baud and load only the `.bin` SRAM image:

```bash
stty -F /dev/ttyUSB2 115200 raw -echo -ixon -ixoff cs8 -cstopb -parenb
cat /dev/ttyUSB2 > build/live_camera_uart.log &
UART_PID=$!
sleep 2
sudo "$HOME/colibry-flash-env/bin/python" \
  "$HOME/colibryNPU_SDK/riscv_runtime/tools/bin_loader/colibry-sramtool.py" \
  "$PWD/build/own_count_camera_live.bin" 0x10000000 --run \
  -p "ftdi://ftdi:4232hp:5:2/1"
sleep 5
kill "$UART_PID" 2>/dev/null || true
wait "$UART_PID" 2>/dev/null || true
cat build/live_camera_uart.log
```

Do not use `toflash`, `make nnpa`, `_flash_boot.bin` or `_flash_copy.bin`. The app embeds the model and copies it into NNPA SRAM. It does not use external flash APIs.

## Captured frame dump

The firmware emits 30 `FRAME_ROW,yy,<80 hex chars>` lines representing the 40x30 grayscale tensor reconstructed from the NNPA input memory layout. The mapping is checked against the model's IFMAP metadata before reading. Decode it on Ubuntu:

```bash
python3 decode_frame_log.py build/live_camera_uart.log --output build/live_camera_frame.png
```

Open `build/live_camera_frame.png` to visually inspect the exact tensor sent to the NNPA.

## Expected success markers

- `CAMERA_FRAME_CAPTURE: SUCCESS`
- `NNPA_DATA_LOADED: 1200 bytes`
- `ISP_SCALER_INPUT: 160 x 120, 1 channel(s)`
- `ISP_SCALER_OUTPUT: 40 x 30`
- `LIVE_FRAME_INFERENCE: SUCCESS`
- `PREDICTED_PEOPLE_COUNT_CLASS: 1_person` / `2_person` / `3_person`

The predicted class should be compared to the actual scene. The current model has no 0-person or >3-person class and has known undercount errors. This is a first live engineering test, not a claim of reliable real-world detection.


## Engineering diagnostics and reference notes

The maintained diagnostic notebook is in [`docs/`](docs/README.md):

- [ISP register and signal reference](docs/isp-register-reference.md)
- [PAG7920LT capture investigation — 2026-11-18](docs/capture-investigation-2026-11-18.md)

These records separate measured evidence from hypotheses and preserve the current known failure state. Update them when new UART logs, SDK findings, or controlled tests become available. Do not treat the current camera test as passing until the scaler PSB error is resolved and the decoded image is validated.
