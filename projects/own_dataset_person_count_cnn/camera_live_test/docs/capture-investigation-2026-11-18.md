# PAG7920LT live capture investigation — 2026-11-18

## Test context

- Board: ASYGN Colibry EvalboardTiny
- Sensor: PAG7920LT
- Firmware: `projects/own_dataset_person_count_cnn/camera_live_test/main.c`
- Capture mode: one physical sensor frame; `CAMIF_PATTERN_TEST = 0`
- Sensor mode reported by firmware: QQVGA, 160 × 120
- Intended ISP output: 40 × 30 grayscale
- Intended NNPA input: 1,200 bytes, `[1,30,40,1]`

## Earlier baseline test

### Camera and transfer

| Measurement | Observed value |
|---|---:|
| PAG7920LT register checks | 29/29 match |
| `CAMIF_IMG_COUNT` after capture | 1 |
| `CAMIF_IMG_CURR_PTR` | `0x0077009f` |
| `ISP_SCALER_PSB_INFO` | `0x0003b89f` |
| Decoded scaler input | 160 × 120 × 1 |
| `ISP_SCALER_SCALING_CFG` | `0x04000400` |
| `ISP_SCALER_SIZE_OUT` | `0x001e0028` (40 × 30) |
| `NNPA_DATA_LOADED` | 1,200 bytes (75 × 16-byte words) |

### Interrupts and image content

- `ISP_INT_STATUS = 0x00000028`
- `CAMIF_INT_STATUS = 0x0000003c`
- `ISP_STATUS = 0x00000000`
- ISP scaler PSB error: set (bit 3)
- ISP scaler parameter error: clear (bit 4)
- ISP frame-start: set (bit 5)
- CAMIF frame start/end: set; data FIFO ready and data FIFO overflow: set
- Decoded image: 40 × 30 grayscale, min 0, max 13, mean 8.93
- Firmware result: `CAMERA_CAPTURE_RESULT: FAIL (5)`

### ISP soft-reset diagnostic

The firmware change in commit `4c741a4b526df876e06d24fb3a5b6df5cdddcfbc` added a diagnostic ISP soft reset before capture. The test showed:

- `ISP_CTRL` after reset: `0x00000007`
- Scaler config after reset: `0x04000400`
- Output size after reset: `0x001e0028`
- The scaler PSB error returned during capture (`ISP_INT_STATUS = 0x28`).
- The reset therefore did not resolve the capture failure.

## Person-present test (latest test)

The physical PAG7920LT camera remained connected to the Colibry board. A person stood in front of the camera during this capture. The existing firmware binary was loaded into SRAM; no source or configuration changes were made for this test.

### Commands used

From `projects/own_dataset_person_count_cnn/camera_live_test`:

```bash
stty -F /dev/ttyUSB2 115200 raw -echo -ixon -ixoff cs8 -cstopb
cat /dev/ttyUSB2 > build/person_present_uart.log &
UART_PID=$!
sleep 2
sudo "$HOME/colibry-flash-env/bin/python" \
  "$HOME/colibryNPU_SDK/riscv_runtime/tools/bin_loader/colibry-sramtool.py" \
  "$PWD/build/own_count_camera_live.bin" 0x10000000 --run \
  -p "ftdi://ftdi:4232hp:5:2/1"
sleep 5
kill "$UART_PID" 2>/dev/null || true
wait "$UART_PID" 2>/dev/null || true

python3 decode_frame_log.py \
  build/person_present_uart.log \
  --output build/person_present_frame.png

grep -E 'CAMERA_CAPTURE_RESULT|ISP_INT_STATUS|CAMIF_INT_STATUS|FRAME_STATS|NNPA_DATA_LOADED|ERROR:' \
  build/person_present_uart.log
```

### Results

| Measurement | Latest result |
|---|---:|
| Decoded dimensions | 40 × 30 grayscale |
| Pixel minimum / maximum | 0 / 13 |
| Pixel mean | 7.35 |
| `CAMIF_INT_STATUS` after frame | `0x0000003c` |
| `ISP_INT_STATUS` after frame | `0x00000028` |
| `NNPA_DATA_LOADED` | 1,200 bytes (75 × 16-byte words) |
| Firmware result | `CAMERA_CAPTURE_RESULT: FAIL (5)` |

The UART log reports:

```text
ERROR: ISP reported a processing/PSB error.
CAMERA_CAPTURE_RESULT: FAIL (5)
```

The decoded image is almost entirely black, with a faint patch. The person-present test did not produce a usable image. The maximum pixel value remained 13; mean pixel value was 7.35, compared with 8.93 in the earlier baseline. Do not interpret this as a meaningful response to the person without a valid image.

## Current interpretation

**Confirmed:**

1. The physical camera frame trigger produces a completed CAMIF image event.
2. The scaler records input metadata of 160 × 120 × 1.
3. NNPA reports 1,200 input bytes loaded.
4. The scaler PSB error flag is set (`ISP_INT_STATUS = 0x28`).
5. The decoded tensor is nearly black (latest range 0–13).
6. The scaler parameter-error flag is not set.
7. Applying the ISP soft reset did not resolve the scaler PSB error.
8. Standing in front of the camera did not result in a usable decoded image.

**Not confirmed:**

1. Whether the sensor is outputting correctly exposed RAW8 pixel values.
2. Whether the CAMIF-to-ISP pixel stream has valid data throughout the frame.
3. The detailed hardware meaning/root cause of `ISP_SCALER_PSB_ERR`.
4. Whether low pixel values originate at the sensor, CAMIF, ISP path, or NNPA memory mapping.
5. Whether CAMIF data FIFO overflow is causal to the scaler error. The CPU-readable data FIFO is a separate diagnostic interface according to firmware comments.

The 1,200-byte NNPA transfer confirms byte count only; it does not prove valid image content. Do not claim that the camera pipeline or person detection is working.

## Build and SRAM-loading record

For the firmware build used in this session:

- Model size: 19,392 bytes
- Model SHA-256: `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e`
- Firmware binary SHA-256: `93188cf8b503d10c0baf123f962e821426fe9cfba0c646cca6a755191954807a`
- Build size: text 69,856; data 20; bss 12,880; total 82,756 (`0x14344`)
- SRAM loader reported JTAG IDCODE `0x249511c3`, CPU halted, SRAM write completed, and CPU started.
- Loader output explicitly reported: `External flash: NOT ACCESSED`.

## Safety constraints

- Do not program or erase external flash during this investigation.
- Continue using the SRAM-only firmware loader.
- Do not use `toflash`, `make nnpa`, `_flash_boot.bin`, or `_flash_copy.bin` for this test.
- Do not change sensor register values or scaler configuration without evidence from the SDK/hardware specification.
- Preserve the existing known-good model bitstream and its embedded header.
- Change one variable at a time and preserve before/after UART logs and decoded images.

## Next investigation

1. Identify and inspect the actual Colibry product specification and any ISP/CAMIF documentation. Do not assume the SDK PDF named `nRF52832_PS_v1.3-1117956.pdf` is the Colibry ISP specification; verify its contents first.
2. Inspect CAMIF, ISP, and NNPA status/busy/ready definitions and the SDK camera/ISP initialization sequence.
3. Establish whether the scaler PSB error is caused by stream framing/handshake, invalid input metadata, or another documented condition.
4. Establish why decoded pixel values are only 0–13 before changing sensor exposure or pixel format.
5. Make no speculative firmware changes. Change one variable at a time after the relevant behavior is documented.

## Session closeout

No fix is claimed. The camera capture remains unresolved, and person detection has not yet been validated. The latest test log is `build/person_present_uart.log`; the decoded image is `build/person_present_frame.png`. These are local build artifacts and were not committed by this documentation update.
