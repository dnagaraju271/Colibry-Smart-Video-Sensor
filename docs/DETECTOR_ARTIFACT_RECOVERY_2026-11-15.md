# Detector Artifact Recovery and Board Deployment Audit

**Audit date:** 2026-11-15  
**Target:** ASYGN Colibry EvalboardTiny Rev1  
**Purpose:** Preserve the recovered person-detector artifact provenance, compiler results, flash-deployment evidence, and current hardware-access blocker.

## Executive status

- The original 35,280-byte INT8 TFLite person-detector file was recovered from the older local project clone and its SHA-256 was recorded.
- Its actual input/output tensors and quantization metadata were read using the LiteRT interpreter.
- Colibry NPU compiler 0.2.1 successfully compiled a copy of that exact TFLite file.
- The newly compiled NNPA file reproduced the older clone's 14,224-byte NNPA artifact byte-for-byte (same SHA-256).
- A second NNPA file in the current project clone has the same reported file size but a different SHA-256; it is not byte-identical.
- The historical flash deployment recipe is recoverable from shell history. The recovered history contains the recipe and expected hashes, but not its execution output.
- A fresh flash read could not begin because the flash tool reported `No FTDI SPI device found`. Ubuntu does see the FTDI USB bridge; PyFtdi currently fails while reading USB string descriptors.
- Therefore, the current contents of the board's flash sector are **not verified by this audit**. No flash erase or programming was performed during the 2026-11-15 verification attempt.

## 1. Recovered source TFLite model

Recovered local path:

`~/Colibry-Smart-Video-Sensor-git/models/colibry_person_detector_int8.tflite`

| Property | Verified value |
|---|---|
| File size | 35,280 bytes |
| SHA-256 | `686c65d098bedf7db36d873da4e315c5e5cf5cd45778457e98d2c12fce3c14e0` |
| Input tensor | `[1, 128, 160, 1]` |
| Input tensor type | `int8` |
| Input scale | `0.003921568859368563` |
| Input zero-point | `-128` |
| Output tensor | `[1, 32, 40, 5]` |
| Output tensor type | `int8` |
| Output scale | `0.05866943299770355` |
| Output zero-point | `87` |

These values were read from the TFLite binary using `ai-edge-litert`; they are not inferred solely from the export script. The output contains 32 × 40 grid cells with five values per cell. This verifies the tensor contract, not detection accuracy.

The export script in the recovered project is `scripts/model/export_colibry_detector_tflite.py`. It expects a Keras input of `(None, 128, 160, 1)`, output `(None, 32, 40, 5)`, and requests fully integer INT8 quantization.

## 2. Reproducible NNPA compilation

Compiler invocation used on a temporary copy of the recovered TFLite model:

```bash
uvx --python 3.12 --from colibry-npu nnpa-compiler --verbose model.tflite
```

Compiler version: **0.2.1**

| Compiler report item | Result |
|---|---:|
| NNPA model configuration | 14,224 bytes |
| Model ID | `0x3F7B` |
| Total weights count reported | 13,659 |
| Inference cycles | 1,078,012 |
| MACs | 17,373,440 |
| Inference data requirement | 11,520 data words; 12 data buffers (compiler wording) |
| Estimated energy | 58.3 µJ per inference at estimated VDD 0.8 V |

The compiler reported that the NNPA model configuration and data fit Colibry memory constraints. The energy figure is a compiler estimate, not a physical measurement.

### Reproduction and artifact comparison

Freshly compiled temporary output SHA-256:

`fe9897ff4694328b22fb3fc91fba9097ca353a18365c197f196011ed14be9fd2`

This exactly matches the older clone's:

`models/compiled/colibry_person_detector.nnpa`

A different NNPA file in the current clone was also checked:

`models/compiled/colibry_person_detector_int8.tflite.nnpa`

Its SHA-256 is:

`e122d3f17509ad22b6a5bb3f8ba0b4db3775bded00834c991b7f91b6eb35a717`

Both files were reported as 14,224 bytes, but their hashes differ. Equal file sizes do not prove equivalent contents. The reproduced `fe9897...` artifact is the one tied to the recovered original TFLite through this compiler reproduction.

## 3. Historical flash deployment recipe recovered

Shell history contains a guarded recipe for a **64 KiB flash sector**:

| Item | Expected value |
|---|---|
| Flash start address | `0x20000` |
| Sector number | `2` |
| Sector size | 65,536 bytes |
| Page size | 256 bytes |
| NNPA image placed at sector start | 14,224 bytes |
| Original sector tail preserved | 51,312 bytes |
| Expected NNPA SHA-256 | `fe9897ff4694328b22fb3fc91fba9097ca353a18365c197f196011ed14be9fd2` |
| Expected backup SHA-256 | `5e83c69f45d5914a775ae940cade8353d5cdb4fa605005ba22932c736999f987` |
| Expected merged-sector image SHA-256 | `efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c` |

The recovered recipe builds the replacement sector by placing the 14,224-byte NNPA at the beginning of the backed-up 65,536-byte sector and preserving the remaining 51,312 bytes. Before writing, it was designed to check the flash ID, check `CR1NV` and `CR1V` equal `0x04`, and verify that the live sector matches the expected backup hash. It then erases sector 2, programs 256-byte pages, reads back the entire sector in 4 KiB chunks, and compares the complete readback hash with the expected merged-sector hash.

**Evidence limitation:** shell history preserves the commands and expected hashes, but not the output from the erase/program/readback session. The previous development-progress notes state that a detector NNPA was programmed and verified; this recovery audit does not independently re-confirm that historical operation or establish the sector's present contents. The read-only check attempted on 2026-11-15 did not reach the flash read stage.

A separate 128 KiB flash backup mentioned in older project handoff notes is a different artifact. Do not confuse it with the 64 KiB sector backup and replacement recipe above.

## 4. Current FTDI / SPI access state

Ubuntu `lsusb` shows:

`Bus 005 Device 002: ID 0403:6043 Future Technology Devices International, Ltd Quad RS232-HS`

Thus the FTDI USB bridge is visible to the Ubuntu VM.

However, `Ftdi.show_devices()` through PyFtdi raised:

`ValueError: The device has no langid (permission issue, no string descriptors supported or device error)`

The SDK flash utility then reported:

`No FTDI SPI device found. Exiting.`

The attempted 64 KiB flash read therefore did not start. USB device permissions and the descriptor-access issue remain to be diagnosed. Do not infer the physical flash contents from this failed attempt.

## 5. Firmware and board-path investigation

- Searches of the project firmware and board scripts did not identify a direct firmware reference to model ID `0x3F7B` or a named NNPA detector file.
- The board scripts present in `scripts/board/` are primarily NNPA diagnostics, output/feature-map reads, and runtime inference helpers.
- `run_nnpa_packed_input_inference.py` reads NNPA registers and sends an SCU message to request inference using the firmware already running. It is not a model uploader or flash programmer.
- The historical deployment used the SDK flash utility at `riscv_runtime/tools/bin_loader/colibry-flashtool.py`, outside the project's board-script directory.

A successful compiler run does not establish that the model is currently loaded, that firmware configures it correctly, or that the output is decoded as detections.

## 6. Camera input compatibility findings

The recovered detector expects one grayscale frame with tensor shape `[1, 128, 160, 1]`.

The SDK board initializer's camera dimensions are documented as dimensions expected after the ISP. The observed `camera_init` implementation selects QQVGA only when dimensions are no more than 160 × 120; 160 × 128 falls into the QVGA selection branch. The SDK camera example demonstrates a camera-to-NNPA data-loaded interrupt flow at 160 × 120, but that example does not by itself prove a working 160 × 128 ISP configuration for this detector.

The ISP HAL provides cropper, Bayer-to-RGB, RGB-to-grayscale and scaler controls, but the application examples inspected did not reveal a confirmed set of ISP programming calls for the detector's exact output dimensions. Camera capture, preprocessing, correct 160 × 128 delivery, and live person detection remain unverified on the physical board.

## 7. Next actions

1. Resolve PyFtdi USB descriptor/permission access while preserving the board state.
2. Perform a strictly read-only read of sector `0x20000` and compare its complete hash with the known backup and replacement hashes.
3. Only after identifying the current flash contents, inspect the firmware/NNPA runtime configuration.
4. Establish the camera/ISP output dimensions and grayscale format required by the verified TFLite/NNPA input.
5. Run a controlled physical-board inference and validate output dequantization and grid decoding before claiming person-detection success.

## Safety and artifact handling

- No flash erase or programming was performed during the 2026-11-15 verification attempt.
- Preserve all existing flash backups and both NNPA binaries.
- Do not replace the different current-clone NNPA file until its provenance is understood.
- Keep raw datasets, flash dumps/backups, and generated binary model artifacts out of Git unless there is an explicit, reviewed reason to version them. This audit records hashes and metadata instead of committing those binary files.
