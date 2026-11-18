# PAG7920LT live capture investigation — 2026-11-18

## Test context

- Board: ASYGN Colibry EvalboardTiny
- Sensor: PAG7920LT
- Firmware: `projects/own_dataset_person_count_cnn/camera_live_test/main.c`
- Capture mode: one physical sensor frame; `CAMIF_PATTERN_TEST = 0`
- Sensor mode reported by firmware: QQVGA, 160 × 120
- Intended ISP output: 40 × 30 grayscale
- Intended NNPA input: 1,200 bytes, `[1,30,40,1]`
- UART log: `build/physical_camera_uart.log`
- Decoded image: `build/live_camera_frame.png`

## Measured UART evidence

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
| `NNPA_INPUT_LOADER_LEVEL` | 1,200 |

The firmware reached the post-`DATA_LOADED` diagnostic path and printed the full 1,200-byte input dump.

### Interrupts

Observed after the frame:

- `ISP_INT_STATUS = 0x00000028`
- `CAMIF_INT_STATUS = 0x0000003c`
- `ISP_STATUS = 0x00000000`

Decoded ISP flags:
- Cropper PSB error: 0
- Bayer2RGB PSB error: 0
- RGB2GRAY PSB error: 0
- Scaler PSB error: 1
- Scaler parameter error: 0
- ISP frame start: 1

Decoded CAMIF flags:
- Image overflow: 0
- Image underflow: 0
- Image start: 1
- Image end: 1
- Data FIFO ready: 1
- Data FIFO overflow: 1
- CDC FIFO bypass: 1
- Data FIFO words reported: 16

The firmware returns `CAMERA_CAPTURE_RESULT: FAIL (5)` because it checks `ISP_INT_STATUS & 0x1F` and rejects any of the five processing/PSB error bits. In this run, bit 3 is set. This is not an NNPA input-loader timeout: `NNPA_DATA_LOADED` was observed.

### Image content

The frame decoder reported:
- Dimensions: 40 × 30 grayscale
- Minimum pixel: 0
- Maximum pixel: 13
- Mean pixel: 8.93

The generated image appears almost entirely black, with only a faint small region visible. These pixel statistics are consistent with the screenshot and indicate that the tensor is not currently a useful representation of the camera scene.

The firmware's `FRAME_ROW` dump reads pixels using `read_model_input_pixel()`, which maps the NNPA's packed/unflattened feature-map memory layout. `RAW_INPUT_ROW` reads the same input region sequentially. Different row byte sequences are expected because the read orders differ.

The 16 words sampled from the CPU-readable CAMIF data FIFO were all decimal 1024 (hex `0x400`). Firmware comments explicitly state that this FIFO is separate from the CAMIF-to-ISP stream and NNPA input memory. These samples must not be treated as captured image pixels.

## Current interpretation

**Confirmed:**
1. The physical camera frame trigger produced a completed CAMIF image event.
2. The scaler recorded input metadata of 160 × 120 × 1.
3. NNPA reported 1,200 input bytes loaded.
4. The scaler PSB error flag was set.
5. The decoded tensor is nearly black (0–13 pixel range).
6. The scaler parameter-error flag was not set.

**Not confirmed:**
1. Whether the sensor is actually outputting correctly exposed RAW8 pixel values.
2. Whether the CAMIF-to-ISP pixel stream has valid data throughout the frame.
3. The detailed hardware meaning/root cause of `ISP_SCALER_PSB_ERR`.
4. Whether the very low pixel values originate at the sensor, CAMIF, ISP path, or NNPA memory mapping.
5. Whether CAMIF data FIFO overflow is causal to the scaler error. The CPU-readable data FIFO is a separate diagnostic interface according to the firmware comments.

Do not claim that the camera pipeline is working correctly merely because the frame count and NNPA byte count advanced.

## Safety constraints

- Do not program or erase external flash during this investigation.
- Continue using the SRAM-only firmware loader.
- Do not use `toflash`, `make nnpa`, `_flash_boot.bin`, or `_flash_copy.bin` for this test.
- Do not change sensor register values or scaler configuration without evidence from the SDK/hardware specification.
- Preserve the existing known-good model bitstream and its embedded header.

## Reproduction / inspection commands

From `projects/own_dataset_person_count_cnn/camera_live_test`:

```bash
sed -n '85,230p' build/physical_camera_uart.log
grep -nE 'NNPA_DATA_LOADED|FRAME_STATS|RAW_INPUT_STATS|FRAME_ROW|FRAME_DUMP_SKIPPED|NNPA_INT_STATUS|ISP_INT_STATUS|CAMIF_INT_STATUS' build/physical_camera_uart.log
python3 decode_frame_log.py build/physical_camera_uart.log --output build/live_camera_frame.png
xdg-open build/live_camera_frame.png
```

Inspect the local SDK definitions:

```bash
sed -n '65,110p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_mmap.h
sed -n '230,305p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_mmap.h
sed -n '330,505p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_hal.hpp
```

## Next investigation

1. Inspect the Colibry product specification at `~/colibryNPU_SDK/components_drivers/NRF52832/nRF52832_PS_v1.3-1117956.pdf` and any available ISP/CAMIF documentation for PSB error conditions, stream protocol, and scaler input requirements.
2. Inspect CAMIF, ISP, and NNPA status/busy/ready definitions and the SDK camera/ISP initialization sequence.
3. Establish whether the scaler PSB error is caused by stream framing/handshake, invalid input metadata, or another documented condition.
4. Establish why the decoded pixel values are only 0–13 before changing sensor exposure or pixel format.
5. Change one variable at a time and preserve before/after UART logs and decoded images.

No fix is claimed by this investigation record.
