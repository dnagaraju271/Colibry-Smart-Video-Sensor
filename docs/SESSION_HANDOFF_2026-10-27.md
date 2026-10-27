# Colibry Smart Video Sensor — Session Handoff
**Date:** 2026-10-27 (end-of-day checkpoint)  
**Purpose:** Resume tomorrow from today's state without restarting the investigation from zero.

## User's target
Achieve **live person detection with YOLO on the physical ASYGN Colibry EvalboardTiny**. The camera is connected directly to the EvalboardTiny. Ultimately the system must produce valid person detections (bounding boxes, confidence scores, and count), not merely report that NNPA inference ran.

## User's explicit instruction for next session
- Start by connecting/reconnecting to the physical board and establish the board's current state.
- Continue from this checkpoint, today's terminal logs, existing scripts, and backups. **Do not repeat all diagnostics from the beginning** or ask again for details already recorded.
- Do not make speculative SPIS changes or run repeated register probes without a specific reason.
- Preserve current files and flash backup. No flash erase/write.
- User is exhausted after approximately 12 hours of debugging. Keep tomorrow's steps small, purposeful, and one at a time.

## Environment and paths (same as before)
- Ubuntu 24.04.5 ARM64 VM in UTM on macOS.
- SDK: `/home/nagaraju/colibryNPU_SDK`
- Project: `/home/nagaraju/Colibry-Smart-Video-Sensor`
- Python environment: `/home/nagaraju/venvs/colibry-compiler/bin/python`
- Model assets: `/home/nagaraju/models/colibry_yolo/compression_trial/`
- GitHub repo: https://github.com/dnagaraju271/Colibry-Smart-Video-Sensor
- The FTDI URI can change after USB reconnection; previously seen JTAG interface `ftdi://ftdi:4232hp:3:4/1` and SPIS interface `ftdi://ftdi:4232hp:3:4/2`. Treat these as historical examples, not guaranteed current addresses.
- Use the already-established project paths. Do not create a second copy of the project.

## Confirmed prior milestones — do not repeat without a reason
1. JTAG IDCODE probe succeeded previously: `0x249511C3`.
2. SRAM loading of the existing Hello World firmware `examples/20_hello_world/build/demo.bin` succeeded previously; UART on `/dev/ttyUSB2` showed `VDD 0.9 V`, `Bitstream loading: OK`, and `System ready. Waiting for SPI input`.
3. One host-packed image inference completed previously: inference counter advanced, firmware returned to idle, and the output region was reported at `0x30064000`. This proves one inference trigger/execution path worked at that time, **not** that output features were correct or person detection worked.
4. Known packed test input already exists:
   `~/models/colibry_yolo/compression_trial/test_person_input_nnpa_packed.bin`
   Size 81,920 bytes (160x128, 4 bytes/pixel). Packing was locally validated earlier. Do not repack it.
5. Existing output captures and comparison scripts already exist. A captured output was byte-identical across two runs, but numerical comparison with ONNX was extremely poor; treat the hardware output readback/format as untrusted.
6. Previous flash backup exists and must be preserved:
   `~/models/colibry_yolo/compression_trial/flash_backup_0x20000_128k.bin`
   Size 131,072 bytes; SHA-256 `5500b7fc998025faede13c03686aa1eaa0edaf83d4a1ce3b90661c9e8ff087af`.
7. Do not run the old local `read_nnpa_output_window.py` blindly; a previous local copy had an undefined `msg` variable before the burst call. The repository revision was corrected with explicit large-burst opt-in.

## Important model limitation already established
The current compiled artifact:
`~/models/colibry_yolo/compression_trial/yolov8n_layer4_qdq_conv_add_int8_terminal.onnx.nnpa`
is documented as a **YOLOv8n-derived backbone feature extractor**, not a complete YOLO detector. It outputs a feature map shaped `[1,64,16,20]`; it does not by itself generate person boxes, confidence scores, or counts. A complete detection head/decoder and correct preprocessing/output interpretation are still needed.

Previous model artifact details:
- Size: 82,656 bytes
- SHA-256: `93272c47fbf505c429582611cf71e58ff127efa4c05aaff213890daf06d585ab`
- Flash address was recorded as `0x20000`; readback previously matched the compiled file exactly.
- Do not reflash or erase as a routine diagnostic.

## Today's (2026-10-27) SPIS source inspection
User already ran:
```bash
cd ~/colibryNPU_SDK
grep -Rni --include='*.h' --include='*.hpp' \
  'SPIS_BUSY_MASK\|spis_busy\|spis_dma_mode' \
  components_drivers colibry_drivers riscv_runtime
```
Findings:
- `colibry_drivers/SPIS/includes/spis_hal.hpp`: `spis_dma_mode_get/set`; `spis_busy_get` indicates an in-progress transaction.
- `colibry_drivers/SPIS/includes/spis_mmap.h`: `SPIS_DMA_MODE_MASK = 0x00001000`; `SPIS_BUSY_MASK = 0x80000000`.
User also searched:
```bash
grep -Rni --include='*.c' --include='*.h' \
  'SPIS_BURST_RD_INIT_OPCODE\|SPIS_BURST_WR_INIT_OPCODE' .
```
Findings:
- Active firmware handlers are in `components_drivers/SPIS_PROTOCOL/spis_protocol.c`; opcode definitions in `spis_protocol.h`.
- Host equivalents are in `riscv_runtime/tools/colibry_interfaces/colibry_spis_api.py` (same burst opcodes, used for read/write commands). This establishes opcode agreement only; it does not identify the actual SPIS failure cause.

Source details already reviewed:
- SPIS burst read/write handler waits on `SPIS_BUSY_MASK`, sets up DMA, and switches SPIS into DMA mode.
- `spis_burst_finalize()` waits for TX FIFO empty / SPIS idle with a timeout, then clears DMA mode and re-enables RX IRQ.
- `examples/50_person_detector/main.c` uses SPIS DMA channel 3 and image receive DMA channel 1. Its deferred finalization path checks done/read-error/write-error masks and services pending finalization in the main loop.
- DMA status fields for channel 3: done bit 3, read-error bit 11, write-error bit 19.
- These are clues for source review, **not proof of root cause**. Do not blindly change SPIS again.

## Existing relevant scripts and data
- `scripts/board/run_nnpa_packed_input_inference.py` — host-prepared packed frame -> SPIS -> inference; supports `--skip-output-read` in the repository version.
- `scripts/board/read_nnpa_output_chunk.py` — bounded 32-byte output read; use only after deciding it is appropriate for the current board state.
- `scripts/board/probe_nnpa_spis.py` — SPIS health/register probe.
- `scripts/board/probe_jtag_idcode.py` — previously passed JTAG test.
- `scripts/board/compare_nnpa_featuremap.py` — offline ONNX comparison.
- `scripts/board/dump_nnpa_featuremap.py`, `scripts/board/check_nnpa_output_read_modes.py`, `scripts/board/read_nnpa_output_window.py`.
- Test input: `test_person_input_nnpa_packed.bin`.
- Previous output captures include `nnpa_output_recovery_20261026.bin`, `nnpa_output_spis_timeout_test_20261025.bin`, and `nnpa_output_skip_read_20261026.bin` (confirm exact local presence before using).
- Previous logs and the current repository source should be reviewed before asking the user to rerun anything.

## Camera-to-detection path: next actual engineering task
The user's camera is directly connected to the EvalboardTiny. Do **not** assume this means the camera pipeline is implemented or unsupported. Inspect existing capture-related source and firmware first, especially:
- `examples/50_person_detector/main.c`
- `components_drivers/EIS316/` (image sensor controller driver seen in the SDK tree)
- existing `image_start`, `image_received`, and `IMGRX_DMA_CHANNEL` references
Determine from the existing code whether the camera produces frames in memory, what format/resolution is available, and how a frame could feed the NNPA input buffer. Do not ask the user to re-explain the camera connection.

## Tomorrow's first steps — start here, not at the beginning
1. Connect/power the EvalboardTiny and make sure the serial terminal is not already holding the UART port.
2. Do only the minimal connection check needed to identify the currently enumerated board/FTDI interfaces and observe the existing firmware's UART state. USB bus/device numbers may have changed; do not assume the old URI.
3. Before any reset, SRAM load, firmware rebuild, inference, SPIS burst, or flash operation, use today's logs and current UART result to decide what state the board is in. Do not repeat the old JTAG and input-packing work just because this is a new chat.
4. Review the current `main.c` image capture path and the EIS316 driver alongside the current model graph/artifact. Establish two separate questions:
   - Is a live camera frame actually being captured and placed in accessible memory?
   - Does the current NNPA model include a detector head, or is it only the backbone feature extractor?
5. Make one evidence-based change at a time. Keep flash untouched and preserve all known backups.
6. Only after board state and capture path are established, decide whether to restore reliable SPIS readback or prioritize the camera-to-NNPA data path. Avoid large output reads until the SPIS DMA path is understood.
7. The eventual acceptance test is a real person in front of the attached camera leading to a valid person detection (box + confidence + count), repeatedly and on the physical board.

## Current status — honest checkpoint
- **Physical board state at the end of today's chat: not freshly confirmed.**
- **Live camera frames reaching NNPA: not yet demonstrated.**
- **Full YOLO detection / valid boxes and counts: not yet demonstrated.**
- **SPIS readback reliability and feature-map numerical correctness: unresolved.**
- Existing code, logs, input, output captures, and flash backup should be used to continue from this exact point.
- No claim that live person detection is working should be made yet.

## End-of-day instruction
Stop debugging for tonight. Do not run more commands now. Keep the VM/project and files in their existing paths. Resume tomorrow with the board connection/state and the existing camera/model source review described above.


## Additional local backups recorded in today's work
These files were seen in the local SDK during the 2026-10-27 SPIS review:
- `components_drivers/SPIS_PROTOCOL/spis_protocol.c.pre_spis_recovery_20261026_180505`
- `examples/50_person_detector/main.c.before_deferred_spis_finalize_20261027.c`
- `examples/50_person_detector/main.c.before_spis_deferred_finalize_20261027.c`

Keep these backup files intact. Tomorrow, compare the active source against them before changing firmware. The user reviewed a SPIS protocol diff today; don't discard or replace that work without inspecting the actual diff and current working tree first.


## Final user request (end of day)
The user is exhausted and explicitly asked to stop for tonight and save the work. Tomorrow, connect to the board first and resume from this checkpoint using the same paths and today's logs/backups; do not restart the investigation or repeat diagnostics from scratch. Do not claim that uncommitted local files or every terminal log were pushed to GitHub unless verified. This handoff is the persistent resume point.
