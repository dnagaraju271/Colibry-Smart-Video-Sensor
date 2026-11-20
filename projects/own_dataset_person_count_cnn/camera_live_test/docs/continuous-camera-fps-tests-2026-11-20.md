# Continuous PAG7920LT camera FPS tests — 2026-11-20

## Purpose

Measure the physical PAG7920LT camera's continuous frame-delivery rate as observed by the Colibry CAMIF image counter.

These measurements establish that CAMIF's frame counter advances during continuous video mode. They do not establish that complete image frames were retrieved, validated, or processed by the NNPA.

## Test configuration

- Board: ASYGN Colibry EvalboardTiny
- Sensor: PAG7920LT, detected through the SDK
- Camera input: 160 x 120, RAW8, one channel
- Camera mode: `CameraPAG7920LT::startVideoMode(requested_fps)`
- Stop method: `CameraPAG7920LT::stopVideoMode()`
- Counter: `CAMIF_reg->camif_img_count`
- Counter reset: `CAMIF_reg->camif_img_count_rst.word = 1u`
- Test duration: 10 seconds for requested rates from 1 to 100 FPS; 5 seconds for 150 FPS
- After stopping: 200 ms delay before reading the registers
- ISP configuration: cropper bypass, Bayer2RGB bypass, RGB2GRAY bypass, scaler configured for 160 x 120 to 40 x 30
- Physical-camera diagnostic configuration: ISP clock source set to CAM_PCLK and CAMIF CDC FIFO bypass enabled

The local firmware source was changed from `CAMIF_PATTERN_TEST=1` to `CAMIF_PATTERN_TEST=0`. The continuous physical-camera test runs before the single-frame capture path and returns after printing its register snapshots.

The input scene was not recorded as part of this FPS sweep. No scene-specific image-quality conclusion is drawn.

The tests were performed against a working tree based on Git commit `57bb81d` (`Enable CAMIF pattern-generator isolation test`). The source changes for this test were uncommitted during the measurements.

## Measurements

| Requested FPS | Test duration | CAMIF frames | Approx. observed FPS |
|---:|---:|---:|---:|
| 1 | 10 s | 8 | 0.8 |
| 2 | 10 s | 18 | 1.8 |
| 5 | 10 s | 47 | 4.7 |
| 10 | 10 s | 97 | 9.7 |
| 20 | 10 s | 193 | 19.3 |
| 30 | 10 s | 293 | 29.3 |
| 40 | 10 s | 392 | 39.2 |
| 60 | 10 s | 585 | 58.5 |
| 100 | 10 s | 985 | 98.5 |
| 150 | 5 s | 736 | 147.2 |

Approximate observed FPS is calculated as `CAMIF frames / programmed test duration`. It is not a hardware-timestamp measurement and includes the effects of starting and stopping the camera around the delay interval.

The CAMIF image counter is 10-bit. The 150 FPS test was limited to five seconds to keep the count below its 1023 maximum. The 100 FPS, ten-second result reached 985, also close to that limit.

## Register observations

The following end-of-test snapshots were observed consistently across the FPS tests:

- `CAMIF_IMG_CURR_PTR`: `0x0077009F`
- `CAMIF_INT_STATUS`: `0x0000003C`
- `ISP_INT_STATUS`: `0x00000028`

The image pointer value corresponds to the final coordinate, X=159 and Y=119, in the configured 160 x 120 frame.

The ISP status `0x28` includes the scaler PSB error and frame-start status bits. The scaler parameter error bit is clear. The PSB error remains unresolved.

These register values are observations; they do not demonstrate that the ISP produced a valid output image.

## What the tests establish

1. The physical PAG7920LT sensor operates in continuous video mode.
2. The CAMIF frame counter advances at rates close to the requested rates over the tested range.
3. The highest tested setting was 150 requested FPS, with approximately 147.2 counted frames per second over the five-second test.

## What remains unverified

- Retrieval of a complete 160 x 120 image into a CPU-accessible memory buffer.
- Validation or visual inspection of a complete physical-camera frame.
- Successful resolution of the ISP scaler PSB error.
- Valid 40 x 30 ISP output for the physical-camera stream.
- NNPA input loading from a validated physical-camera frame.
- Person-detection inference throughput or accuracy.

The CAMIF diagnostic FIFO is not a full-frame buffer. A progressing frame counter or image pointer must not be treated as proof that complete image data is available to the CPU.

## Next investigation

Inspect the SDK DMA HAL and CAMIF trigger path to determine whether the CAMIF stream can be transferred into a complete frame buffer in SRAM. Start with a controlled 30 FPS request for that investigation; the measured 29.3 CAMIF frames per second makes it a practical initial test setting, not a validated image-transfer or inference rate.

Do not report live person detection as working until a complete physical-camera image is retrieved, inspected, and passed through the intended processing path.
