# Colibry Smart Video Sensor — Session Handoff
**Date:** 2026-10-23  
**Status:** End-of-day checkpoint — do not perform further board tests until this checkpoint is reviewed.

## Project objective
Build a physically validated person-detection pipeline on the ASYGN Colibry EvalboardTiny using a YOLOv8n-derived model. The currently compiled model is a backbone feature extractor only. Inference completing is not detection success; final goals include numerically correct features, a detection head/decoder, and valid person bounding boxes/counts.

Repository: https://github.com/dnagaraju271/Colibry-Smart-Video-Sensor

## Environment
- Ubuntu 24.04.5 ARM64 VM in UTM on macOS
- SDK: `/home/nagaraju/colibryNPU_SDK`
- Python: `/home/nagaraju/venvs/colibry-compiler/bin/python`
- Project: `/home/nagaraju/Colibry-Smart-Video-Sensor`
- Model assets: `/home/nagaraju/models/colibry_yolo/compression_trial/`
- FT4232H USB ID: `0403:6043`
- Latest enumerated URI prefix: `ftdi://ftdi:4232hp:3:4/`; enumeration reported interfaces `/1` to `/4`.
- SPIS auto-detection connected to interface `/2`; JTAG test passed on explicit interface `/1`.
- Use `sudo` for PyFtdi/board access as normal-user USB permissions previously failed.
- `git` and `curl` unavailable; `wget` worked.
- Do not assume the USB bus/device address or URI will remain identical after reconnection.

## Proven milestones
1. Built a quantized NNPA model artifact:
   `~/models/colibry_yolo/compression_trial/yolov8n_layer4_qdq_conv_add_int8_terminal.onnx.nnpa`
   - 82,656 bytes
   - SHA-256: `93272c47fbf505c429582611cf71e58ff127efa4c05aaff213890daf06d585ab`
   - Flash address: `0x20000`
   - User handoff reported that flash readback matched the compiled file exactly. Preserve the existing flash backup and do not reflash or erase.
2. Packed input was validated locally:
   - Original: `test_person_input_nchw_u8.bin`, 61,440 bytes, shape `[1,3,128,160]`
   - Packed: `test_person_input_nnpa_packed.bin`, 81,920 bytes
   - SHA-256: `64b5063a8127f81d0cf99f44b4a7035b6ad5e2d8f0e1afa9ab66f4bac97968a4`
   - Packing validation found 20,480 unique pixel locations, no non-zero padding, and exact reconstruction of the original tensor. Actual hardware layout is still not validated.
3. Physical input transfer and inference completed once:
   - Inference counter advanced from 2 to 3
   - status at completion: `0x00001B06`
   - firmware returned to idle message 0
   - output start word 25344; output memory address `0x30064000`
   - reported output-size register `0x03F03C13`; output-info register `0x00000000`
   - logical dimensions interpreted as `[16,20,64]`
   - saved 20,480-byte output:
     `~/models/colibry_yolo/compression_trial/nnpa_output_featuremap_raw.bin`
   - SHA-256: `94ea2f613c065f78d3c917cce77c696d5eb3898bc30acae76eb8895da57d5800`
   - The firmware raw result message `149` is one output byte, not a person count/confidence.
4. JTAG responsiveness has now been confirmed using `scripts/board/probe_jtag_idcode.py`:
   - FTDI URI `ftdi://ftdi:4232hp:3:4/1`, 400 kHz
   - returned expected IDCODE `0x249511C3`
   - result: PASS.
5. Read-only SPIS probe still fails:
   - auto-detected EvalboardTiny and connected to `ftdi://ftdi:4232hp:3:4/2`
   - every repeated read returned `0xF8000000` for NNPA status, model size, input size, output size, output start, inference counter, and SCU message
   - this is the SPIS NOP pattern and invalid readback, not valid register values.

## Current numerical-validation blocker
The ONNX comparison derived the final output quantization correctly from the final `DequantizeLinear` node:
- scale `0.04950463399`
- zero point `149`
- ONNX output shape `[1,64,16,20]`
- output should be treated as a backbone feature map, not detections.

For the captured 20,480 bytes, current comparison is extremely poor:
- NNPA dequantized min/max: `-7.3761907 / 5.2474914`
- ONNX min/max: `0 / 2.7722595`
- HWC correlation: `-0.007173`
- NCHW correlation: `0.027763`
- RMSE approximately `7.14`
- 74.941% of bytes below zero point 149
- common raw bytes: 0 occurred 15,348 times; 248 occurred 5,116 times

This does **not** establish whether the cause is memory-read format, tensor layout, model/quantization compatibility, or a different issue. Do not reuse the earlier invalid comparison that treated `0xF8000000` as a scale.

## Newly established diagnosis clues
- JTAG works; SPIS reads all return the configured NOP pattern. Hardware connection/host USB enumeration is not wholly broken.
- The SDK `spis_protocol.c` burst-read handler:
  - saves address and byte count from the burst command
  - sets DMA length to requested bytes minus one
  - disables SPIS receive interrupts
  - starts DMA and enables SPIS DMA mode
  - expects the DMA interrupt handler to invoke `spis_burst_finalize()`, which waits for TX FIFO empty/SPIS idle, disables DMA mode, and re-enables SPIS receive interrupts.
- The local firmware source `examples/50_person_detector/main.c` DMA ISR checks the done-status bit for SPIS DMA channel 3, clears that bit, and calls `spis_burst_finalize()`. The displayed code does not visibly branch on read/write error status bits. If a DMA error occurs instead of a done event, it is a plausible cause of SPIS remaining in DMA mode with receive interrupts disabled, but this has not been confirmed.
- DMA length field is 32 bits wide. The 81,920-byte size does not numerically overflow the field, but other peripheral/transport limits and completion behavior still must be verified before any large read.
- The last SPIS read-only health probe did not trigger inference, reset the CPU, or write flash.
- Do not conclude the unsuccessful output-window read caused the current SPIS fault. The user's local old copy of `read_nnpa_output_window.py` contains `print(f"Firmware message: {msg}")` with no defined `msg` in that scope, before the line that invokes `mmap_readburst()`. That old script therefore fails before starting its own large burst. The diagnostic operation described in earlier notes may have been from a different revision/tool invocation; exact causality is unknown.
- Current repository version of `read_nnpa_output_window.py` was corrected in commit `7a48a6b31a941bc0673b23b4c0699a484d7acda8`: define the sampled message and require explicit `--allow-large-burst` opt-in after DMA behavior is reviewed. The local VM file was confirmed still to be the older copy when last inspected; do not run it as-is.
- A separate direct JTAG test script was added in commit `ac8fad7a6cba119aa9f69e76a2816880e6b35277`:
  `scripts/board/probe_jtag_idcode.py`
  The downloaded local copy successfully ran and returned expected IDCODE.

## Other important files
- SPIS probe: `scripts/board/probe_nnpa_spis.py`
- output window: `scripts/board/read_nnpa_output_window.py`
- ONNX comparison: `scripts/board/compare_nnpa_featuremap.py`
- captured output: `nnpa_output_featuremap_raw.bin`
- input originals/packed: `test_person_input_nchw_u8.bin`, `test_person_input_nnpa_packed.bin`
- previous flash backup (preserve): `~/models/colibry_yolo/compression_trial/flash_backup_0x20000_128k.bin`
  - 131,072 bytes
  - SHA-256: `5500b7fc998025faede13c03686aa1eaa0edaf83d4a1ce3b90661c9e8ff087af`

## Progress / remaining work
Foundation bring-up has made meaningful progress: model compilation and byte-level flash verification, local input-packing checks, one physical inference, output capture, and a passing JTAG IDCODE test are complete.

Not complete:
1. Restore reliable SPIS/MMAP readback.
2. Determine actual DMA done/error/interrupt behavior and whether the firmware is stuck in SPIS DMA mode.
3. Confirm valid NNPA register values after recovery.
4. Determine documented output-memory representation and read it with a bounded, supported-size method.
5. Compare the correct hardware feature values against ONNX with sensible numerical agreement.
6. Resolve any input tensor, model compiler, layout, or quantization mismatch.
7. Add/compile the detector head and postprocessing to get scores/boxes.
8. Validate bounding boxes and person counts on physical-board examples.

Do not claim person detection is working yet. There is no validated bounding-box/person-count output. A percentage is not a reliable progress measure here; milestones above are more accurate.

## Resume plan (read-only first)
1. Start with the currently saved output file and this checkpoint; don't repeat input packing or the already-successful inference without a reason.
2. Ensure no serial monitor still owns a port. Re-enumerate FTDI URIs because USB bus/device numbering can change.
3. Inspect `main.c` DMA ISR plus DMA status definitions and supported peripheral constraints. Decide what evidence can be collected without reset/register writes.
4. Restore SPIS readback before another burst, inference, firmware load, CPU reset, or flash operation.
5. Keep large output-window DMA disabled until its completion behavior and maximum valid transfer size are understood.
6. Only after reliable reads are restored, inspect output layout and rerun the quantitative comparison.
7. Preserve the flash backup and never flash/erase as part of the initial recovery investigation.

## End-of-day shutdown
No more test commands tonight. The last script returned to the shell prompt and the read-only tests did not change flash or request CPU reset.
- If `picocom` is open in another terminal, close it with Ctrl-A then Ctrl-X.
- Do not run the SRAM loader, flash tools, or output-window script.
- To power down the EvalboardTiny, use the ColibryProbe's physical PWR_OFF button to remove target power; don't use a reset sequence as a substitute.
- If also powering down Ubuntu, run `sudo poweroff` inside the VM only after saving this checkpoint and closing any applications that need the files.
