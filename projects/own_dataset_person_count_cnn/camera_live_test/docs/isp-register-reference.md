# Colibry ISP register and signal reference

**Purpose:** Keep a source-grounded reference for debugging the PAG7920LT camera pipeline.

**SDK root used in the current VM:** `~/colibryNPU_SDK`

**Primary source files (not vendored here):**
- `colibry_drivers/ISP/includes/isp_mmap.h`
- `colibry_drivers/ISP/includes/isp_hal.hpp`

The SDK files are authoritative for the exact register layout and HAL comments. Re-check the local SDK version before relying on these notes after an SDK upgrade.

## ISP interrupt status

The SDK defines these bits in `ISP_INT_STATUS`:

| Bit | Mask | SDK field | Meaning documented by SDK |
|---:|---:|---|---|
| 0 | `0x00000001` | `isp_cropper_psb_err` | Cropper PSB error |
| 1 | `0x00000002` | `isp_b2rgb_psb_err` | Bayer2RGB PSB error |
| 2 | `0x00000004` | `isp_rgb2gray_psb_err` | RGB2GRAY PSB error |
| 3 | `0x00000008` | `isp_scaler_psb_err` | Scaler PSB error |
| 4 | `0x00000010` | `isp_scaler_param_err` | Scaler parameter error |
| 5 | `0x00000020` | `isp_frame_start` | ISP frame-start event |

The HAL comments say the PSB error flags rise when the corresponding event occurs. They do **not** specify the detailed root cause or recovery procedure. Do not infer a specific electrical/timing cause from the flag alone.

## Scaler input metadata

`ISP_SCALER_PSB_INFO` records metadata for the last frame received by the scaler:

- Bits [10:0]: input width minus 1
- Bits [21:11]: input height minus 1
- Bits [23:22]: input channel count minus 1

The current firmware decodes these fields in `psb_width()`, `psb_height()`, and `psb_channels()`.

For the current capture, `0x0003b89f` decodes as:
- Width: 160
- Height: 120
- Channels: 1

This confirms the scaler recorded those input dimensions. It does not, by itself, prove that all pixels were valid or that the output frame was correct.

## Scaler configuration

The SDK HAL documents X and Y scaling values as 4.8 fixed-point factors. The image width/height is divided by the factor; the documented minimum is 1.0, encoded as `0x100`.

Current firmware configuration:
- `ISP_SCALER_SCALING_X = 0x400` → factor 4.0
- `ISP_SCALER_SCALING_Y = 0x400` → factor 4.0
- `ISP_SCALER_SIZE_OUT_X = 40`
- `ISP_SCALER_SIZE_OUT_Y = 30`

With 160 × 120 input and 4× downscaling, the requested output is 40 × 30. The measured register values are consistent with that configuration. This does not establish that the scaler PSB transfer completed without error.

## Current firmware ISP control setup

In `camera_live_test/main.c`, `configure_isp_for_pag7920_mono()`:
- bypasses cropper,
- bypasses Bayer2RGB,
- bypasses RGB2GRAY,
- sets 4× X/Y scaling,
- sets output size to 40 × 30,
- clears ISP interrupt flags with `0x3F`.

The comments in firmware assume the PAG7920LT is supplying monochrome RAW8. Verify the sensor/board configuration against the SDK and hardware documentation before changing that assumption.

## Useful local inspection commands

Run from the Ubuntu VM:

```bash
sed -n '65,110p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_mmap.h
sed -n '230,305p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_mmap.h
sed -n '330,445p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_hal.hpp
sed -n '450,505p' ~/colibryNPU_SDK/colibry_drivers/ISP/includes/isp_hal.hpp
```

Search all SDK sources for additional implementation details:

```bash
grep -RInE --include='*.h' --include='*.hpp' --include='*.c' --include='*.cpp' \
  'isp_scaler|SCALER_PSB|scaler_size_out|scaler_scaling' \
  ~/colibryNPU_SDK/colibry_drivers/ISP
```

## Important distinction

- `ISP_SCALER_PSB_ERR` is a bus/event error flag.
- `ISP_SCALER_PARAM_ERR` is a separate scaler-parameter error flag.

A zero parameter-error flag does not explain or clear a PSB error. The current SDK comments identify the flags but do not explain the detailed PSB failure mechanism. Consult the applicable Colibry hardware/product specification or vendor documentation before proposing register changes.
