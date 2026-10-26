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


## SESSION UPDATE — 2026-10-26: Inference succeeded; output readback is unreliable

### Confirmed today
- Board selector set to JTAG; FTDI USB device enumerated as 0403:6043.
- JTAG IDCODE probe on `ftdi://ftdi:4232hp:3:4/1` at 10 kHz returned `0x249511C3`.
- Loaded the existing SDK Hello World image `examples/20_hello_world/build/demo.bin` into SRAM at `0x10000000` using the SRAM loader. The log said CPU halted, SRAM write done, starting CPU. No external flash write was performed.
- UART on `/dev/ttyUSB2` printed VDD 0.9 V, `Bitstream loading: OK`, and `System ready. Waiting for SPI input`.
- Before inference, SPIS/MMAP returned structured values: status `0x00000006`, model size 5166 words, input size 5120 words, input start word 6144, output size register `0x03F03C13`, counter 0.
- One input inference completed: count 0 -> 1, status `0x00001B06`, output start word 25344, address `0x30064000`, decoded output dimensions [16,20,64], 20,480 logical bytes. This confirms the accelerator executed one pass; it does not confirm feature accuracy or person detection.
- The saved output `nnpa_output_recovery_20261026.bin` has SHA-256 `94ea2f613c065f78d3c917cce77c696d5eb3898bc30acae76eb8895da57d5800`. It is byte-identical to `nnpa_output_cpu_mem_access_fix_20261024.bin`.
- Offline ONNX comparison is poor (HWC correlation -0.007173, RMSE about 7.151; NCHW correlation 0.027763, RMSE about 7.138). Raw bytes are dominated by 0, 248, 249, 255; only 8/20480 are in expected code range 149..205. Therefore the current capture is not a trustworthy feature map and those metrics must not be treated as final model accuracy.
- After the output burst-read, subsequent SPIS metadata probes returned `0xF8000000` for every register (the vendor API's `SPIS_NOP_OPCODE`). A read-only probe stopped before sampling output memory. Do not issue another large output burst at present.
- No flash erase/write was performed in this session.

### What is correct vs. not yet correct
- JTAG selection/path and the SRAM loader work.
- Hello World startup, bitstream check, SPIS input transfer, inference trigger, counter advance, and final NNPA status were observed to work.
- The output-read path is not validated. `mmap_readburst` uses the SDK's burst-DMA path, while the inference script currently reads the entire 20,480-byte logical output in one burst request (5,120 32-bit words split by the host API into 32-byte exchanges). The raw capture and post-read NOP sentinel are evidence of an output-transfer / SPIS-state problem, but do not by themselves prove exactly which firmware/DMA condition caused it.
- Source inspection found DMA interrupt status has separate done, read-error, and write-error bitfields. The example application's SPIS DMA handler tests only the done bit before calling `spis_burst_finalize()`. Error interrupt handling and a bounded recovery path need review before a firmware fix is chosen.
- The current model produces a [1,64,16,20] backbone feature map. It is not yet an end-to-end detector and has no validated bounding-box/person-count output.

### Controlled recovery plan
1. Use the new project option `--skip-output-read` to separate inference from the output-memory burst. This option reports inference metadata and then exits without dumping output.
2. Restore a known firmware state by SRAM-loading the existing `demo.bin` again (only if needed to recover SPIS). Keep external flash untouched.
3. After a fresh successful inference with output read skipped, attempt exactly one 32-byte output burst using `scripts/board/read_nnpa_output_chunk.py`.
4. Immediately run the read-only SPIS health probe once. If registers return normal values, test bounded chunks before any full output capture. If SPIS returns NOP again after the 32-byte burst, stop and fix/rebuild the SPIS DMA handling before another capture.
5. Only after output readback remains reliable, establish output byte format/layout and repeat the ONNX numerical comparison.
6. Do not rebuild or retrain the YOLO model yet. Preserve the current NNPA model and flash backup. Do not run flash write/erase tools.

### GitHub sync
- Working branch: `fix/safe-spis-output-readback`
- Added `--skip-output-read` to `scripts/board/run_nnpa_packed_input_inference.py`.
- Added `scripts/board/read_nnpa_output_chunk.py`, a single-burst read capped at 32 bytes.
- These changes isolate the failing operation; they do not yet claim to fix the firmware-side SPIS issue.


## SESSION UPDATE — 2026-10-26: Paused for Ubuntu sudo authentication

### Current blocker
- User reports `sudo -v` fails with “Sorry, try again” even when entering what they believe is the Ubuntu account password.
- `whoami` returns `nagaraju`.
- `setxkbmap -query` reports `layout: us`; user says their physical keyboard is QWERTY, so layout mismatch is not currently the leading theory.
- Do not continue repeated sudo attempts. No password was shared.
- The Colibry board was previously running SRAM-loaded `examples/20_hello_world/build/demo.bin`; UART showed `Bitstream loading: OK` and `System ready. Waiting for SPI input`.
- JTAG had passed; the SPIS probe after a full-output burst returned `0xF8000000` for all sampled metadata. No more board tests until authentication is resolved.
- The user's screenshot/logs indicate the GitHub safety branch files were downloaded into the local project and py_compile passed at timestamp `20261026_143647`.

### GitHub changes saved
- Branch: `fix/safe-spis-output-readback`
- Added `--skip-output-read` to `scripts/board/run_nnpa_packed_input_inference.py` so inference can be tested without issuing the large output burst.
- Added `scripts/board/read_nnpa_output_chunk.py`, which validates metadata and caps a single output burst to 32 bytes.
- Updated this handoff file with Oct 26 progress.
- Relevant commit sequence on branch includes inference-only option, bounded output-read script, and handoff update.

### Resume procedure
1. Resolve local Ubuntu authentication using the user's approved Recovery Mode path; no Colibry board or external flash changes are necessary for this.
2. Verify with `sudo -v` after reboot. Never request or record the password in chat.
3. Confirm local project code and handoff match branch `fix/safe-spis-output-readback` (pull/sync latest handoff after authentication is working if required).
4. Keep the board powered/state stable; first run read-only SPIS health probe. If it returns NOP sentinel, reload the existing `demo.bin` into SRAM using the known-good JTAG loader, verify UART readiness, and recheck SPIS.
5. Use `--skip-output-read` to isolate inference from output readback. Only then attempt one 32-byte output burst with `read_nnpa_output_chunk.py`; immediately run a read-only SPIS health check once.
6. If the short burst breaks SPIS again, stop and inspect/fix the firmware SPIS/DMA completion/error path before more output reads.
7. Do not reflash, erase, retrain, or rebuild the YOLO model at this stage.

### Data safety
- Existing external flash backup remains preserved; no flash write/erase is reported for the Oct 26 session.
- Never send passwords or reset credentials through chat.


## SESSION UPDATE — 2026-10-26 16:23: USB enumeration failure during SPIS probe
- Latest SPIS health probe failed before reading NNPA registers:
  `ValueError: The device has no langid (permission issue, no string descriptors supported or device error)`.
- Because the command presented `[sudo] password for nagaraju:` first, this run appears to have progressed past sudo authentication and then failed in PyUSB/USB descriptor enumeration. Do not reset the account password again solely because of this hardware-tool error.
- Earlier, sudo PyFtdi enumeration succeeded and listed interfaces `ftdi://ftdi:4232hp:3:4/1` through `/4`. USB bus/device numbers can change after reconnect/VM passthrough changes.
- Resume next with a read-only `lsusb | grep -i -E 'FTDI|0403'`, then explicitly select the current FTDI interface rather than auto-detection. If the USB descriptor error persists, re-check UTM USB passthrough/reconnect and device permissions before any NNPA/SPIS operation.
- Do not run inference, burst output reads, reset the CPU, or modify external flash until the interface enumeration issue is cleared.


## SESSION UPDATE — 2026-10-26 16:25: UTM usbredir protocol error
- User captured a macOS/UTM dialog: `usbredir protocol parse error for USB Device [0403:6043] at 0-2`.
- In Ubuntu, `lsusb` still sees `Bus 003 Device 004: ID 0403:6043 Future Technology Devices International, Ltd Quad RS232-HS`.
- Both an explicit SPIS probe and `sudo ... Ftdi.show_devices()` fail at USB descriptor enumeration with `ValueError: The device has no langid (permission issue, no string descriptors supported or device error)`.
- Since the host UTM USB-redirection error and PyUSB descriptor failure now co-occur, prioritize UTM/macOS USB passthrough stability. Do not interpret this as proof of an NNPA/SPIS firmware fault.
- Safe recovery: stop running board scripts; detach FTDI from VM using UTM USB toolbar; physically disconnect/reconnect the ColibryProbe USB cable (avoid hubs/adapters if possible); re-attach only the FTDI device to Ubuntu using UTM USB menu after VM is running; then run `lsusb` and PyFtdi enumeration once. If the UTM popup reappears, stop and report UTM/macOS versions and whether the FTDI is connected through a hub. Do not delete UTM settings, run kext commands, or repeatedly toggle the interface.
- Official UTM USB sharing docs: https://docs.getutm.app/guest-support/sharing/usb/ . The docs note USB sharing needs to be enabled and device attached to the VM; the USB toolbar lets users attach/detach devices.
- Project files and scripts remain backed up/synchronized. No additional inference or flash operation was done in response to this error.


## SESSION UPDATE — 2026-10-26 16:40: Ubuntu guest Caps Lock inversion
- User reports Caps Lock behavior is inverted in the Ubuntu VM: when Mac Caps Lock appears ON, text behaves lowercase; when OFF, text behaves uppercase. User's physical keyboard is QWERTY and Ubuntu reports XKB layout `us`.
- This likely matches a known Ubuntu/GNOME/Mutter Caps Lock state-inversion issue; do not assume it is a password typo.
- Next resume action: in macOS UTM Settings → Input, ensure `Caps Lock treated as a key` is OFF unless there is a specific reason to use raw key behavior. UTM documentation notes this option can leave host/guest Caps Lock states out of sync when enabled.
- Then in Ubuntu inspect `gsettings get org.gnome.desktop.input-sources xkb-options`. Apply the documented XKB workaround option `shift:both_capslock_cancel` (preserve existing options) so pressing both Shift keys enables Caps Lock and pressing one Shift disables it; use both Shift keys then one Shift to resynchronize state.
- After fixing the keyboard state, verify `sudo -v` once. Never ask for or store password in chat.
- Current separate USB blocker remains: UTM dialog `usbredir protocol parse error for USB Device [0403:6043] at 0-2`, and PyFtdi enumeration failing with `ValueError: The device has no langid`. Fix the host/VM keyboard issue first, then reconnect FTDI passthrough once and retest enumeration.


## SESSION UPDATE — 2026-10-26 16:45: Caps Lock fixed; PyFtdi still cannot enumerate descriptors
- User reports the Caps Lock inversion is now fixed.
- Latest command was `sudo -v && sudo ... Ftdi.show_devices()`; it reached the Python traceback, so sudo authentication succeeded for this invocation.
- `lsusb` sees `0403:6043` at Bus 003 Device 004, but `Ftdi.show_devices()` fails reading USB product string/LANGID with `ValueError: The device has no langid`.
- Earlier macOS/UTM popup explicitly stated `usbredir protocol parse error for USB Device [0403:6043] at 0-2`.
- Next action is now to perform the actual UTM USB detach/reconnect sequence (not run more code): ensure the Ubuntu VM has USB sharing enabled (QEMU backend), detach the FTDI entry using UTM's USB toolbar, physically disconnect the ColibryProbe cable for 5 seconds, reconnect, then reattach FTDI to Ubuntu. Run `lsusb` and `Ftdi.show_devices()` once afterward.
- If the error remains, fully shut down the Ubuntu VM, disconnect/reconnect the FTDI, restart VM, attach device; gather UTM and macOS versions before considering package or driver changes. Do not unload/macOS kernel extensions or change libusb settings without evidence.
- Hardware state/code status unchanged by the failed enumeration. Do not run SPIS or inference until PyFtdi enumerates interfaces successfully.


## SESSION UPDATE — 2026-10-26 16:50: USB descriptor issue persists after Caps Lock fix
- User reports Caps Lock behavior is fixed and wants to resume project. Do not ask for or store passwords.
- `lsusb` sees `Bus 003 Device 004: ID 0403:6043 Future Technology Devices International, Ltd Quad RS232-HS`.
- `sudo ... Ftdi.show_devices()` still fails at `usb.util.get_string` with `ValueError: The device has no langid`.
- Explicit SPIS probe on previously known URI `ftdi://ftdi:4232hp:3:4/2` also fails with the same descriptor error before register communication.
- Host UTM/macOS dialog showed: `usbredir protocol parse error for USB Device [0403:6043] at 0-2`.
- Official UTM USB docs warn QEMU USB capture does not perform a proper hardware reset; device is configured on host before being redirected to guest, and some devices may not work reliably. Repeated failed PyFtdi enumeration should not be treated as an NNPA firmware diagnostic.
- Official PyFtdi troubleshooting states no-langid on Linux may be a device permission issue or indicate that string descriptors are unsupported / inaccessible. In this case, the matching UTM usbredir dialog makes passthrough state a leading suspect, but not a proven root cause.
- Next: stop project hardware commands; close picocom, shut Ubuntu down cleanly using desktop UI or `sudo poweroff` only if sudo is working, detach FTDI from UTM, unplug ColibryProbe USB physically, wait 10 seconds, reconnect directly, boot Ubuntu, then attach FTDI once from UTM USB toolbar after the guest is up. Run `lsusb` followed by one `sudo ... Ftdi.show_devices()` check.
- If still failing, capture UTM version, macOS version, VM backend (QEMU vs Apple Virtualization), USB sharing setting, whether a hub/adapter is used, and whether the host usbredir popup reappears. Do not repeatedly toggle or reset board; do not issue SPIS/inference commands until PyFtdi enumeration is restored.
- Continue from GitHub branch `fix/safe-spis-output-readback`; output-readback isolation and 32-byte bounded diagnostic remain saved. External flash remains untouched.


## SESSION UPDATE — 2026-10-26: USB passthrough recovered
- After a clean UTM/USB disconnect-reconnect, `lsusb` reports `Bus 003 Device 002: ID 0403:6043 Future Technology Devices International, Ltd Quad RS232-HS`.
- `sudo ... Ftdi.show_devices()` now succeeds and lists:
  `ftdi://ftdi:4232hp:3:2/1`, `/2`, `/3`, `/4`.
- Previous enumeration failure (`The device has no langid`) is currently resolved. USB address changed from `3:4` to `3:2`; do not reuse old explicit URIs. Interface mapping previously validated: /1 JTAG, /2 SPIS, but verify JTAG IDCODE now before loading SRAM.
- The USB cable was physically disconnected, so target power was cycled and SRAM firmware should be considered lost. External flash remains unchanged.
- Next: run read-only JTAG IDCODE probe on `ftdi://ftdi:4232hp:3:2/1` at 10 kHz. If it passes, reload existing `examples/20_hello_world/build/demo.bin` into SRAM with the SRAM loader at `0x10000000`, then confirm UART `Bitstream loading: OK` and `System ready. Waiting for SPI input`. Determine the current UART device name after USB re-enumeration; do not assume it is still `/dev/ttyUSB2`.
- After UART startup, run one read-only SPIS health probe on `ftdi://ftdi:4232hp:3:2/2`. Only if SPIS metadata is valid, proceed to run inference with new `--skip-output-read` option; do not test a 32-byte burst until after inference-only mode and SPIS health are confirmed.


## SESSION UPDATE — 2026-10-26 17:00: JTAG verified after USB passthrough recovery
- After reconnecting FTDI, current URI is `ftdi://ftdi:4232hp:3:2/` with four interfaces.
- Read-only JTAG IDCODE at `ftdi://ftdi:4232hp:3:2/1`, 10 kHz returned `0x249511C3`; result PASS.
- Physical USB disconnect/power-cycle means SRAM firmware is considered lost; external flash is unchanged.
- Next: identify current UART device names (USB tty names may have changed) before reloading known-good SDK Hello World into SRAM. Then load `examples/20_hello_world/build/demo.bin` to `0x10000000` through JTAG interface `/1`, verify UART startup, then perform exactly one read-only SPIS check using interface `/2`.
- Do not run inference or any output burst until startup and SPIS health are confirmed.
