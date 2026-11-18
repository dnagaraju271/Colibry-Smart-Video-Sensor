# Session Handoff — 2026-11-18

## Project scope

Continue only Professor Frédéric AMIEL's three requested items unless explicitly instructed otherwise:

1. Demonstrate the existing simple C LED blinker on the Colibry EvalboardTiny.
2. Create and test a very small CNN with two trainable layers.
3. Collect and organize the outdoor image dataset.

Do not add unrelated features, dashboards, tracking, or broader product work.

## Working environment

- Repository: `dnagaraju271/Colibry-Smart-Video-Sensor`
- Branch: `main`
- Ubuntu VM prompt: `nagaraju@colibry-x86-vm`
- Repository directory: `~/Colibry-Smart-Video-Sensor`
- SDK root: `~/colibryNPU_SDK`
- Flash-tool Python environment: `~/colibry-flash-env`
- JTAG adapter: `ftdi://ftdi:4232hp:5:2/1`
- JTAG IDCODE observed: `0x249511c3`

## Documentation committed to GitHub

Searchable Markdown text extractions of the supplied PDFs are available under `docs/reference/`:

- `ColibryNPU_EvaluationKit_User_Manual_v0.3.md`
- `ColibryNPU_Product_Specification_v1.0.md`
- `ASYGN_Challenge_Mission_Book_Phase_2_2026-11-05.md`
- `ASYGN_Mission_Book_Phase_1_2026-10-08.md`
- `README.md`

These are text extractions, not the original PDF binaries; consult the original PDFs for figures and visual layout.

## CPU halt and recovery event

The JTAG SPIMF initialization inside `JtagSpimfInterface` calls `cpu_halt()`. An earlier flash-identification-only invocation therefore halted the CPU and stopped the physical LED blinking. No flash erase/program was run in that invocation.

The SDK implementation of `cpu_restart(boot_address=0x10000000)` was inspected. The method writes boot address `0x10000000` and issues the DMControl resume/reset sequence.

A direct API invocation using `importlib.util.spec_from_file_location` successfully connected to JTAG and sent `cpu_restart()`. The user confirmed the LED resumed blinking after that recovery.

## Read-only flash sector backup — 2026-11-18

- Flash ID check: recognized.
- Interface: JTAG/SPIMF, channel 1.
- Read address: `0x20000`.
- Sector size: `65536` bytes (64 KiB).
- Read method: `FlashAccess.read_flash_array()` in 4096-byte chunks.
- Read result: 65536 / 65536 bytes.
- Backup file on user's Ubuntu VM:
  `/home/nagaraju/models/edgeguard_own_count/flash_backups/flash_sector_0x20000_20261118.bin`
- SHA-256:
  `8bf446e0ca7a095313b956001ea3046fa328380ce15fe6cc9106eaea104bbcb8`
- The saved file was reread and its SHA-256 matched the in-memory bytes.
- This hash matches neither historical reference hash recorded in earlier handoff notes:
  - Historical original-sector backup: `5e83c69f45d5914a775ae940cade8353d5cdb4fa605005ba22932c736999f987`
  - Historical previous detector replacement sector: `efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c`
- Interpretation: the current sector differs from both historical snapshots. This alone does not establish corruption or identify the content. Preserve the backup and inspect its contents before any programming decision.
- The script's `finally` block successfully sent `cpu_restart()` with boot address `0x10000000` after the read.
- No flash erase, write, or program operation was performed during this backup.

## Critical safety notes

- Never run `colibry-flashtool.py --check_only`: inspection of this SDK version showed that this path invokes `check_flash_access()`, which writes test data and is destructive.
- Do not assume the `-i spi` direct interface works; the earlier attempt returned flash ID bytes `[255,255,255,255]`.
- JTAG flash interface initialization halts the CPU. Any future read-only JTAG/SPIMF session must plan a verified CPU restart in cleanup.
- Do not erase or rewrite sector `0x20000` until the backup contents and intended flash layout are understood.
- Do not treat NNPA compiler success as proof of physical-board inference.

## Next step

Inspect the already-saved local backup file without reconnecting to the board:

1. Reconfirm file size and SHA-256.
2. Display the first and last 256 bytes in hex.
3. Count erased-value bytes (`0xFF`) and zero bytes, and inspect the byte distribution.
4. Compare any recognizable structures or embedded model signatures against the SDK's documented flash layout.

Only use local-file read operations for this inspection. Do not read or write flash again until the local backup has been analyzed.


## Local flash-backup content inspection — 2026-11-18

The user inspected the saved 64 KiB backup locally (no board connection was used). Results:

- SHA-256 reconfirmed: `8bf446e0ca7a095313b956001ea3046fa328380ce15fe6cc9106eaea104bbcb8`.
- First 16 bytes at flash offset `0x20000`: `7B 3F 79 03 00 50 00 FF FF 0F C1 27 00 00 00 00`.
- Final 4 bytes at `0x2FFFC`: `7C 7A 83 84`.
- Entire 64 KiB sector contains 40,804 bytes of `0x00`, 24,482 bytes of `0xFF`, and 250 bytes of other values (76 unique byte values overall).
- Only 8 printable strings of length at least 4 were detected; they look non-semantic/random in this basic scan.
- This distribution is too sparse to identify the sector as a valid NNPA bitstream. It may represent real sparse contents or a read-path/transport issue; the existing binary/read routine must be checked before drawing conclusions.
- No flash modification occurred.

### Next diagnostic (local files only)
Inspect contiguous non-`00`/`FF` ranges and 256-byte block patterns in the backup, compare the backup against the local CNN `.nnpa` file (if present), and inspect the exact `read_flash_array()` implementation used for the read. Do not perform another board read or any flash write until this offline validation explains the sparse pattern.


## Offline flash-backup structure and CNN comparison — 2026-11-18

The user ran the offline inspection script against the local 65536-byte backup. Results:

- Backup SHA-256 reconfirmed: `8bf446e0ca7a095313b956001ea3046fa328380ce15fe6cc9106eaea104bbcb8`.
- Exact CNN NNPA artifact at `~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa` is 19,392 bytes, SHA-256 `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e`.
- The full NNPA binary and its first 64 bytes were both not found anywhere in the 64 KiB backup.
- Nonzero-byte ranges and 256-byte classification show an unusual repeating pattern: many 4 KiB regions contain long all-zero or all-`0xFF` runs with a small mixed block at each 4 KiB boundary. Examples include 0x21100-0x21EFF all 0xFF followed by mixed 0x21F00-0x220FF; later regions repeat similar zero/FF blocks.
- The first 16 bytes of the sector remain `7B 3F 79 03 00 50 00 FF FF 0F C1 27 00 00 00 00`.
- This pattern could be actual sparse flash contents, but the JTAG/SPIMF read transport has not been validated for 4096-byte reads. `FlashAccess.read_flash_array()` simply passes `nbyte` to `self.ll_intf.exchange(..., nbyte)`; this does not itself establish that the interface returned every requested byte faithfully.
- Therefore: the local file checksum and file length are verified, but the physical-sector read fidelity remains unverified. Do not infer flash corruption or sector structure yet.
- No flash writes/erases occurred.

### Next step — inspect read transport offline

Inspect the source implementation of `JtagSpimfInterface.exchange()` and how it services read lengths/chunks, especially any 256-byte burst limits. Do not reconnect to the board or repeat a flash read until this software path is reviewed. No programming decision should rely on this backup until its transport integrity is established.


## SDK SPIMF read transport source review — 2026-11-18

The user supplied the relevant `JtagSpimfInterface` source from `colibry-flashtool.py`:

- `SPIMF_FIFO_NBYTES = 16`.
- `exchange(tx_bytes, readlen)` sets `nbyte = len(tx_bytes) + readlen`, builds TX words from the command bytes plus zero dummy bytes, allocates RX words, and keeps chip select low during the operation.
- It sends data in 16-byte FIFO chunks, using `ceil(chunk_bytes/4)` 32-bit words, and reads the same number of words from `COLIBRY_SPIMF_RX_POP_ADDR` via `jtag_api.mmap_read()`.
- It converts received words back to bytes and returns the bytes after the TX-command prefix.
- For the sector reads used here, each transaction requested 4096 data bytes plus a 4-byte READ command, total 4100 bytes (divisible by four). The chunk accounting and final trim look internally consistent at a superficial review; no definite 4 KB size arithmetic bug identified.
- `spimf_init()` still explicitly halts the CPU and configures PLL/calibration/DIO/SPIMF; always account for CPU restart after any future JTAG SPIMF initialization.

### Remaining unknown

Need inspect `colibry_jtag_api.py` implementations of:
- `mmap_read(addr)`
- `mmap_write(addr, data, push=True)`
- any `push=False` behavior / queued write semantics
- lower-level `read_dmi` and `write_dmi` used for system-bus access

Do not reconnect to the board or reread flash while reviewing these software paths. No flash writes/erases have been performed.


## Review of pasted JTAG/SPIMF exchange implementation — 2026-11-18

The user supplied a source excerpt for `JtagSpimfInterface.exchange()` from `colibry-flashtool.py`:

- SPIMF FIFO is configured as 16 bytes (`SPIMF_FIFO_NBYTES = 16`).
- The method forms one TX byte stream containing the command header and read dummy bytes, converts it to big-endian 32-bit words, pads to a 4-byte boundary, and allocates an RX word array of corresponding size.
- It asserts chip select, repeatedly pushes up to 16 bytes (four 32-bit words), and for reads pops one RX word per TX word. Finally it releases chip select and converts RX words back to bytes, discarding bytes corresponding to the TX command header.
- For a READ transaction with a 4-byte header and a 4096-byte read, the total stream is 4100 bytes, word-aligned; the 16-byte FIFO loop handles 256 full chunks plus a final 4-byte chunk. No immediate array-length/word-rounding error is evident for the exact 4096-byte read used in backup.
- The exchange loop writes the SPIMF length register with `nbyte_tx_next * 8 - 1` on partial/final chunks and then on the terminating zero-length loop iteration before breaking. This deserves consideration but is not by itself evidence that returned bytes are wrong.
- The uploaded excerpt contains no implementation of `Colibry_jtag_api.mmap_read()` or `mmap_write()`; therefore the semantics of `push=False`, JTAG system-bus access, and RX FIFO read ordering remain unverified.
- No new board access or flash modification occurred in this review.

### Next step

Read-only source inspection of `~/colibryNPU_SDK/riscv_runtime/tools/colibry_interfaces/colibry_jtag_api.py`, focusing on full `mmap_read()` and `mmap_write()` implementations and any queue/push behavior. Do not reconnect to the board until this code review is complete.


## Review of JTAG system-bus access excerpt — 2026-11-18

The user supplied `mmap_read()` and `mmap_write()` implementations:

- `mmap_read(addr)` resets the JTAG TAP, writes DMI `sbaddress0` (0x39), reads `sbdata0` (0x3C), and returns that value.
- `mmap_write(addr, data, push=True)` resets the JTAG TAP, only rewrites `sbaddress0` if `self_curr_mmap_waddr != addr`, writes DMI `sbdata0` (0x3C), calls `jtag.sync()`, and optionally drains four bytes from the JTAG controller. It documents `push=False` as deferring effective write until next access.
- Important check: `mmap_read(RX_POP)` does change the actual system-bus address but does not update `self_curr_mmap_waddr`. In `JtagSpimfInterface.exchange()`, however, each following FIFO iteration first calls `mmap_write(SPIMF_LEN_ADDR, ...)`, then `mmap_write(SPIMF_TX_PUSH_ADDR, ...)`; both differ from the cached TX_PUSH address after a previous chunk and so should explicitly rewrite `sbaddress0` before writing TX data. Thus this particular cache detail is not, by itself, proof of incorrect writes in the shown exchange loop.
- The implementation of `read_dmi()` and `write_dmi()` is still not yet visible. These are necessary to assess the exact DMI pipeline/read completion semantics and whether RX words returned by `mmap_read()` are valid.
- No new board access or flash modification occurred.

### Next step — source-only inspection

Read the exact bodies of `read_dmi()` and `write_dmi()` (around lines 160–222 in `colibry_jtag_api.py`). Do not reconnect to the board or perform any flash operation until DMI read/write behavior is understood.


## Confirmed SDK JTAG address-cache defect and safe reader helper — 2026-11-18

The source inspection identifies a concrete defect in the SDK's `JtagSpimfInterface.exchange()` + `Colibry_jtag_api.mmap_read()/mmap_write()` interaction:

- `mmap_write(addr, ...)` caches its last requested system-bus address in `self_curr_mmap_waddr` and only writes DMI `sbaddress0` when the requested address differs from the cached value.
- `mmap_read(addr)` always writes `sbaddress0` to its requested address, e.g. SPIMF RX_POP (0x2001501C), but does not update/invalidate `self_curr_mmap_waddr`.
- During each full 16-byte SPIMF FIFO chunk, `exchange()` pushes TX words to TX_PUSH (0x20015018), then reads RX words from RX_POP (0x2001501C). For following full chunks, the loop does not rewrite SPIMF_LEN_ADDR; it calls `mmap_write(TX_PUSH_ADDR, ...)` directly.
- Because the cached value still says TX_PUSH while the actual system-bus address was changed to RX_POP by the last read, `mmap_write(TX_PUSH_ADDR, ...)` can incorrectly skip setting `sbaddress0` and push data into RX_POP rather than TX_PUSH.
- This explains why long/multi-FIFO read transactions can return invalid data after the first FIFO chunk. The earlier sector backup must be treated as forensic/untrusted, not as an accurate flash image.
- `read_dmi()` uses a READ request followed by a NOP and then captures the response; `write_dmi()` sends a WRITE request. Nothing in the pasted methods removes the confirmed stale system-bus address cache defect.
- No flash modification has occurred.

### Cache-safe reader helper committed

Added `scripts/board/read_flash_sector_jtag_cache_safe.py` to `main` (latest helper commit: `6388d3cd573ff42cf9174e0dfef812c51a6c6a90`).

The helper:
- Wraps `mmap_read()` and sets `self_curr_mmap_waddr = None` after every RX FIFO read, forcing the next `mmap_write()` to restore its requested system-bus address.
- Reads using 256-byte top-level flash transactions to keep per-transaction scope controlled.
- Calls only JEDEC READ ID (0x9F) and flash-array READ (0x03).
- Does not call flash write, register-write, erase, check-only, or programming methods.
- In its finally block attempts `cpu_restart()` using boot address `0x10000000`. It reports a failure and does not report a successful exit if CPU restart fails.
- Has been committed but is not yet locally pulled, syntax-checked, or tested on the hardware.

### Next action

User must pull the updated `main` and run `python -m py_compile` locally first. Only after syntax validation should the cache-safe helper be run using explicit `--sdk-root` and `--output` paths (because `sudo` changes `Path.home()`). The helper will halt CPU while initializing SPIMF and restart it at cleanup; expect the LED to pause temporarily. After the corrected read, compare sector checksum, block patterns, and embedded NNPA signature. Do not flash/program anything yet.


## Local repository update and syntax verification — 2026-11-18

- User safely entered the repository at `/home/nagaraju/Colibry-Smart-Video-Sensor`.
- `git fetch origin main` advanced the remote-tracking ref from `8f58562` to `1c32346`.
- Confirmed remote `main` did not contain path collisions with the three untracked local paths:
  - `datasets/mobile_160x120/processed/`
  - `main`
  - `simple_person_cnn/model/own_count_experiment/`
- Confirmed local `HEAD` was an ancestor of `origin/main`.
- `git merge --ff-only origin/main` succeeded and brought in the reference documents, handoff and cache-safe flash reader.
- `python3 -m py_compile scripts/board/read_flash_sector_jtag_cache_safe.py` passed.
- All three untracked paths remained untouched and present after the update; none were staged, committed, stashed, moved, or deleted.
- Next step: use the cache-safe helper first for a small 256-byte read at 0x20000 to validate the multi-FIFO read path, saving under a new unique filename. Inspect that short output before considering a complete 64 KiB backup. The helper has not yet been hardware-tested.


## Cache-safe reader pull and syntax check — 2026-11-18

- User safely ran the collision and fast-forward checks.
- Local `HEAD` was an ancestor of `origin/main`, no remote collisions existed for the three untracked local paths.
- `git merge --ff-only origin/main` succeeded.
- `python3 -m py_compile scripts/board/read_flash_sector_jtag_cache_safe.py` succeeded with `SYNTAX CHECK PASSED`.
- The untracked local paths remain untouched:
  - `datasets/mobile_160x120/processed/`
  - `main`
  - `simple_person_cnn/model/own_count_experiment/`
- Next action: run the cache-safe reader for a **256-byte probe only** at flash address 0x20000, save to a new output filename, and inspect the probe result. Do not yet run the full 64 KiB read. The helper automatically restarts CPU in finally; LED may pause during JTAG/SPIMF initialization.


## Cache-safe 256-byte flash probe — 2026-11-18

User ran the new helper from the updated local repository:

- JTAG connection succeeded; IDCODE `0x249511c3`.
- CPU halt occurred during SPIMF initialization as expected.
- Flash identification succeeded.
- Read completed: address `0x020000`, size 256 bytes.
- Output file: `/home/nagaraju/models/edgeguard_own_count/flash_backups/cache_safe_probe_256_0x20000.bin`
- SHA-256: `cee24b3a287eacef223741627cd8e36d4a169055e30271afbf05fa310998f81c`
- CPU restart command sent successfully with boot address `0x10000000`.
- This is the first hardware run of the cache-safe helper. The 256-byte content has not yet been inspected, so do not infer correctness solely from length/checksum. Next inspect the probe bytes locally and compare them to offsets 0x20000–0x200FF in the earlier (untrusted) backup. Do not yet read the full sector.
- No flash erase/program operations were called.


## Cache-safe 256-byte probe content inspection — 2026-11-18

The user inspected `cache_safe_probe_256_0x20000.bin` locally.

- Probe SHA-256: `cee24b3a287eacef223741627cd8e36d4a169055e30271afbf05fa310998f81c`.
- Distribution: 97 zero bytes, 4 `0xFF` bytes, 155 other values.
- The bytes contain varied instruction-like/binary data beyond the 16-byte header; this is materially different from the earlier untrusted dump, where most of the corresponding 256-byte range was zero.
- Comparison against the first 256 bytes of the earlier (untrusted) 64 KiB backup: not identical; 149 byte positions differ.
- Both reads report the same leading 12-byte prefix `7B 3F 79 03 00 50 00 FF FF 0F C1 27`; the prior read then produced mostly zero padding where the cache-safe read returns varied data.
- This is consistent with the confirmed stale `self_curr_mmap_waddr` defect affecting multi-FIFO reads. The corrected probe looks plausible, but we have not yet verified the whole sector or confirmed exact model placement.
- Helper CPU cleanup printed `CPU restart command sent (boot address 0x10000000)`.
- No flash write/erase occurred.

### Next action

Run the cache-safe helper over the full 64 KiB sector at 0x20000, using 256-byte top-level read transactions and a new output path. Preserve both earlier files unchanged. Inspect the full corrected backup SHA-256 and byte/block distribution, then compare against the known CNN `.nnpa` artifact. Do not program or erase flash.


## Full cache-safe sector backup and historical-hash match — 2026-11-18

The user successfully ran the corrected cache-safe JTAG/SPIMF helper over the full 64 KiB region.

- JTAG IDCODE: 0x249511c3.
- Flash ID: recognized.
- Range: address 0x20000, size 65536 bytes.
- Top-level chunks: 256 bytes each, with FIFO read-cache invalidation after every SPIMF RX_POP read.
- Backup path: /home/nagaraju/models/edgeguard_own_count/flash_backups/flash_sector_0x20000_cache_safe_20261118.bin.
- Helper verified file size and in-memory-vs-saved SHA-256.
- SHA-256: efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c.
- This exactly matches the previously recorded historical hash for the previous detector replacement sector. That historical image was documented as a 14,224-byte earlier detector NNPA bitstream plus preserved original tail to fill the 64 KiB sector.
- Therefore, current sector 2 at 0x20000 is strongly identified as that historical previous detector replacement image, not as the new own-count CNN NNPA file. The CNN artifact remains 19,392 bytes with SHA-256 ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e.
- CPU restart command was sent by the helper with boot address 0x10000000.
- No flash erase, write, or program was performed.

### State and next action

- Keep all three local backups: original cache-bugged read, cache-safe 256-byte probe, and corrected cache-safe 64 KiB image.
- The corrected 64 KiB file is the trustworthy sector backup candidate and matches the historical previous detector replacement hash.
- Before deciding whether/how to replace the sector with the new CNN artifact, recover and verify the original pre-replacement sector tail and precise historical flash layout. Historical notes indicate the prior operation preserved a 51,312-byte tail, but the original sector backup hash is different and the original backup binary is not known to be present locally.
- Do not overwrite the current sector until that original tail and replacement procedure are fully understood.
- The current CNN NNPA file must not be flashed until a separate, safe, documented model/firmware loading path is established and the professor's requested simple CNN board inference workflow is prepared.

## Original sector backup recovered locally — 2026-11-18

The user searched existing backup files under ~/models, ~/Colibry-Smart-Video-Sensor-git, and ~/Colibry-Smart-Video-Sensor. An original 64 KiB sector backup was found:

- Path: /home/nagaraju/Colibry-Smart-Video-Sensor-git/models/compiled/backup_flash_sector_0x20000.bin
- Size: 65536 bytes.
- SHA-256: 5e83c69f45d5914a775ae940cade8353d5cdb4fa605005ba22932c736999f987.
- Exact hash match to historical original-sector backup hash.
- This confirms the original sector backup is present locally. The 128 KiB historical inventory candidate was not listed in the current file search output; its current presence remains unconfirmed.

### Next verification

Compare the original sector backup and cache-safe current sector image locally, with no board access:
- Compare current sector bytes 0..14223 to the historical old detector NNPA artifact (if its file is still present).
- Compare bytes 14224..65535 of current sector with the same range of original backup to verify preservation of the 51,312-byte tail.
- Reconstruct in memory: old NNPA bytes + original backup[14224:] and verify this reconstructed sector SHA-256 equals efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c.
- Do not write or upload raw backups to GitHub; retain them locally and record only hashes/metadata.

## Original sector reconstruction fully verified — 2026-11-18

The user ran a local-file-only comparison between the recovered original 64 KiB sector backup, the corrected cache-safe current 64 KiB sector read, and the historical detector NNPA file.

- Original sector: /home/nagaraju/Colibry-Smart-Video-Sensor-git/models/compiled/backup_flash_sector_0x20000.bin
- Original size: 65536 bytes.
- Original SHA-256: 5e83c69f45d5914a775ae940cade8353d5cdb4fa605005ba22932c736999f987 (exact historical match).
- Current cache-safe sector: /home/nagaraju/models/edgeguard_own_count/flash_backups/flash_sector_0x20000_cache_safe_20261118.bin
- Current size: 65536 bytes.
- Current SHA-256: efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c (exact historical replacement-sector match).
- Historical detector NNPA: /home/nagaraju/Colibry-Smart-Video-Sensor-git/models/compiled/colibry_person_detector.nnpa
- NNPA size: 14224 bytes.
- NNPA SHA-256: fe9897ff4694328b22fb3fc91fba9097ca353a18365c197f196011ed14be9fd2 (exact historical compiled artifact match).
- Current sector first 14,224 bytes exactly equal the historical detector NNPA.
- Current sector bytes 14,224..65,535 (51,312 bytes) exactly equal the same range from the original backup.
- Reconstructing historical NNPA + original backup tail produced SHA-256 efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c and was byte-for-byte identical to the corrected cache-safe current-sector read.
- User script ended with ALL ORIGINAL DATA AND TAIL CHECKS PASSED.

Conclusion: the original 64 KiB sector data is recovered and verified locally; the current sector image is exactly the expected historical composition, with the previous detector NNPA replacing only the first 14,224 bytes while the original 51,312-byte tail is preserved. This does not mean the new own-count CNN has been flashed or that it is ready for deployment.

No external flash write, erase, or programming operation was performed during recovery/verification. Preserve both original and current sector backup files. Do not flash the new CNN as part of this recovery task.

## Own dataset and CNN deployment plan — 2026-11-18

### Dataset already available

- Canonical archive: datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip.
- Images are 160x120 grayscale and the existing trainer resizes to 40x30 grayscale input.
- Previously verified training class distribution: 1_person=59, 2_person=8, 3_person=4 (71 labeled images). The manifest also contains pending zero-person originals intentionally skipped; there is no trained zero-person class in this experiment.
- This dataset is highly imbalanced, especially for 2-person and 3-person cases. Do not treat aggregate accuracy alone as sufficient validation.

### Existing model and prior training

- Training script: simple_person_cnn/train_own_count_cnn.py.
- Architecture: exactly two trainable layers: Conv2D(4 filters, 3x3, ReLU) and Dense(3 linear logits), with Flatten between them; 40x30x1 input and 3 classes.
- The script supports --check-only, which checks dataset integrity, split, architecture, and class distributions without starting training or writing model files.
- Previously run host training used 56 training and 15 validation images; early stopped at epoch 21 with best epoch 14. Train accuracy was 1.0 and validation accuracy 0.8, but the confusion matrix [[12,0,0],[2,0,0],[0,1,0]] shows the model failed to identify 2-person and 3-person validation examples. This model is not yet a reliable people counter.
- Legacy Keras exporter: simple_person_cnn/export_legacy_keras_for_colibry.py; it copies the same weights into legacy tf_keras and checks numerical parity.
- A full-INT8 TFLite and 19,392-byte NNPA compile artifact were already produced earlier. NNPA file: ~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa; the previously recorded SHA-256 is ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e.
- The Colibry board does not train neural-network weights. Training is performed on the Ubuntu host using the user's dataset; Colibry NNPA runs inference using a compiled NNPA bitstream loaded by suitable firmware.

### Hardware/deployment status

- Board's current flash sector 2 at 0x20000 is proven byte-for-byte to contain the old 14,224-byte person-detector NNPA followed by the original 51,312-byte flash tail.
- The new own-count CNN is not flashed and has not been verified running on board.
- Before any flash change, inspect SDK reference firmware/sample paths for loading a model into NNPA SRAM and running a small CNN inference test. Keep existing original/current sector backups intact. Avoid direct sector replacement until firmware loading path and all data/layout requirements are established.

### Next safe step

Run the existing trainer with --check-only on the canonical archive and inspect the existing report/model artifact hashes. Preserve the current generated experiment directory. If retraining is required, use a new timestamped output directory to avoid overwriting previous weights/reports. After host-side metrics are acceptable, proceed to NNPA inference firmware preparation and physical-board inference; do not describe this as on-board training.

## Own dataset preflight completed — 2026-11-18

User ran the training script with --check-only. Results:

- Canonical archive: /home/nagaraju/Colibry-Smart-Video-Sensor/datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip.
- Archive size: 725 KiB.
- Archive SHA-256: c43161f7ed98bab89b997d986396b77a554c22f6b5821b3a01807d010b2777c7.
- Processed image directory contains 71 images with counts 1_person=59, 2_person=8, 3_person=4.
- Archive contains 71 labeled images; three pending zero-person originals were skipped by the trainer.
- Model input: 30x40x1 grayscale.
- Train/validation split: 56 / 15; train class counts 47/6/3 and validation counts 12/2/1.
- There are 6 source groups, but 5 source groups occur in both train and validation splits. The image-level split therefore leaks related images across partitions and cannot support a reliable generalization estimate.
- Exactly two trainable layers confirmed: Conv2D and Dense. Total parameters 14,443.
- `--check-only` passed; it did not start training or write model files.
- TensorFlow CUDA warnings indicate GPU not available and CPU execution is expected; they did not make the preflight fail.

Existing artifacts found and checksummed (all from 2026-11-16):

- Keras source model: `simple_person_cnn/model/own_count_experiment/own_count_cnn.keras`, 193 KiB, SHA-256 `5c96d2ce428d66e6cbf6fc335a1f19850a90a27c8b6a6a09b7d99abba6443654`.
- TFLite export: same experiment directory `own_count_cnn.tflite`, 60 KiB, SHA-256 `d3544ca026fa739ea9844022d104d983704b05e3ebbe1063fb0784d8baafbe54`.
- Experiment report: `experiment_report.json`, SHA-256 `0473cccd9aca0a4ec85b0355d20c3bedbaf16f623e6706a59df8bc45d73a3a84`.
- Legacy Keras H5: `~/models/edgeguard_own_count/own_count_cnn_legacy.h5`, 72 KiB, SHA-256 `17692bcccaf677a009e6808d76aa8fc9fd7dbd47180aa539afd9788e81bbfc42`.
- PTQ TFLite: `~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite`, SHA-256 `df8d84655a0c1142aa556120920587ce256f26bd14a0c0c5a8c598bfbfe9a45a`.
- Compiled CNN NNPA: `~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa`, 19,392 bytes, SHA-256 `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e`.

Existing model should not be retrained/overwritten until current report metrics and per-source-group class distributions are inspected. Prior evaluation documented 80% image-level validation accuracy but confusion matrix [[12,0,0],[2,0,0],[0,1,0]], showing the model failed on validation examples from classes 2_person and 3_person.

Important clarification: model training is performed on the Ubuntu host using TensorFlow and the user's data; Colibry NNPA is used to execute inference, not to train weights. The compiled CNN artifact has not been flashed and has not been run on the physical board.

### Next local read-only diagnostic

Inspect `experiment_report.json`, group counts per source timestamp, and whether `datasets/mobile_160x120/processed/` is just an extracted copy of the 71 archive images or contains additional examples. Do not retrain or change model/flash files yet. Consider collecting more 2-person and 3-person images before any new run.

## Dataset source-group audit — 2026-11-18

User inspected the previous training report, source groups, and extracted directory.

- `datasets/mobile_160x120/processed/` contains 71 images and all 71 match byte-for-byte the same relative paths in the canonical ZIP. It is an extracted copy, not additional data.
- The canonical ZIP manifest has 74 entries including 3 `pending_for_later_originals` zero-person originals, which are not included in the current 3-class trainer.
- Per source group:
  - 2026-11-16 11.33.31: 4 x 3_person and 1 x 2_person (5 total).
  - 2026-11-16 11.33.32: 2 x 1_person and 2 x 2_person (4 total).
  - 2026-11-16 11.33.33: 14 x 1_person and 5 x 2_person (19 total).
  - 2026-11-16 11.33.34: 20 x 1_person.
  - 2026-11-16 11.33.35: 16 x 1_person.
  - 2026-11-16 11.33.36: 7 x 1_person.
- All 4 `3_person` images are from one source group only (11.33.31). It is impossible to create an independent group-held-out validation split for `3_person` while also training on `3_person` with these data. Validation of that class cannot currently assess generalization across independent source groups.
- `2_person` examples occur in 3 source groups but total only 8; `1_person` occurs in 5 groups but is majority class.
- Prior model metrics: 21 epochs; train accuracy 1.0; image-level validation accuracy ~0.80; validation confusion matrix [[12,0,0],[2,0,0],[0,1,0]], so no 2_person or 3_person validation example was correctly classified. Do not claim this is a reliable people counter.
- The image-level train/validation split leaks 5 of 6 source groups and related images; metrics are likely optimistic.
- The TensorFlow `--check-only` preflight passed and did not write artifacts.
- Existing trained model, TFLite, legacy H5, PTQ TFLite, and NNPA artifact have verified local hashes in the preceding section. The NNPA has not been flashed and has not run on the physical board.

### Decision / next step

For the professor's small-CNN proof-of-work, use the existing model only as an experimental candidate while clearly reporting the weak per-class validation. Do not retrain or overwrite the existing artifacts blindly. First inspect the SDK sample firmware that loads an NNPA bitstream into NNPA SRAM and performs one inference; determine the minimum firmware path for this exact `[1,30,40,1]` uint8 input and three logits. Board training is not supported by the NNPA; host training is required. Do not modify sector 0x20000 during this investigation.

## SRAM-only own-count CNN board inference scaffold committed — 2026-11-18

After inspecting SDK examples, a temporary SRAM-only inference app was added under `experiments/professor_demo/task2_small_cnn/`.

Files:
- `README.md` — host training vs board inference, SRAM-only run procedure and validation limitations.
- `Makefile` — SDK build configuration for `EVBTINY`; explicit `prepare-data` step.
- `generate_board_data.py` — embeds the verified NNPA bitstream and three selected own-dataset images into a generated C header.
- `main.c` — initializes ColibryBoard, copies the embedded NNPA bytes directly into NNPA SRAM using CPU memory access, calls `check_nnpa_bitstream()` and `nnpa_allocate_buffers()`, loads 1200 bytes per image through the NNPA 8-bit input loader, triggers inference, and prints output shape/raw values/predicted class.
- `.gitignore` — excludes generated `build/` C header.

Verified design facts from SDK sources:
- `ColibryBoard::copy_bitstream_to_mem()` copies from external flash to NNPAMEM via CPU memory access and 32-bit writes. The new app uses the same destination and CPU memory access pattern, but sources words from the embedded SRAM firmware image instead of external flash.
- SDK `30_mlperf_vww` example uses `input_loader_cfg`, `INPUT_LOADER_RST_MASK`, `input_loader_data`, and `RUN_START_MASK` for 8-bit input loading and inference.
- `NNPA_NBYTE_PER_MEM_WORD` is 16; the own CNN input tensor is 40x30x1 uint8 = 1200 bytes (expected 75 NNPA memory words).
- `nnpa_hal.hpp` confirms `rd_ofmap_size.fields.ofmap_width`, `.ofmap_height`, `.ofmap_depth` are dimensions minus one.
- Generator pins the exact own-count NNPA artifact size (19392 bytes) and SHA-256 `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e` before embedding it.
- Three sample images selected from the existing train/validation set: `1_person/I1__008__120x160.png`, `2_person/I2__007__120x160.png`, `3_person/I3__004__120x160.png`. They use the same grayscale Lanczos 160x120 to 40x30 preprocessing as the Python trainer.
- The app tests three logits in class order 1_person, 2_person, 3_person and prints raw uint8 logits. It compares the predicted class to the sample folder label; this is only a hardware execution test, not an accuracy claim.

Important:
- The scaffold is committed but is NOT yet syntax-checked by the local RISC-V toolchain, compiled, loaded into SRAM, or tested on hardware.
- The app embeds the NNPA binary into its firmware image and does not need to program external flash. The documented run is `make all tosram board=EVBTINY`, after the manual's Wait Start sequence. Never use `toflash` or `make nnpa` for this test.
- Preserve original sector backup and current sector backup. Sector 0x20000 remains unchanged.
- The local repository must be updated from origin/main before building; ensure existing untracked dataset and model experiment directories remain untouched.

### Next action

Safely fast-forward the local working clone to current origin/main (first fetch and confirm local HEAD is ancestor and no untracked path collisions), then enter `experiments/professor_demo/task2_small_cnn/`, run `make prepare-data`, `make check-inputs`, and a build-only `make all board=EVBTINY`. Do not execute the `tosram` run until the build succeeds. After build success, prepare Wait Start mode and execute the SRAM test. Keep all flash files unchanged.

## Own-count CNN firmware build completed — 2026-11-18

User updated local repository from origin/main (1c32346 -> 494e950) using a fast-forward. Local untracked paths remained present and untouched:
- `datasets/mobile_160x120/processed/`
- `main`
- `simple_person_cnn/model/own_count_experiment/`

`python3 -m py_compile experiments/professor_demo/task2_small_cnn/generate_board_data.py` passed.

`make prepare-data` completed successfully. Generated `build/own_count_board_data.h` using:
- Verified NNPA file `~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa`, 19,392 bytes, SHA-256 `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e`.
- Dataset archive `datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip`.
- Input shape `[30,40,1]` uint8; 1200 bytes each.
- Three samples: `1_person/I1__008__120x160.png`, `2_person/I2__007__120x160.png`, `3_person/I3__004__120x160.png`.

`make check-inputs` passed.

`make all board=EVBTINY` completed successfully: C and C++ compilation, link, objcopy and flash-image generation succeeded. Reported ELF size:
- text 49,744 bytes
- data 20 bytes
- bss 12,512 bytes
- total reported 62,276 bytes (0xF344).

The build output created both SRAM binary and flash-oriented variants (including `own_count_cnn_sram_demo.bin`, `_flash_boot.bin`, `_flash_copy.bin`). These are build artifacts only; user did not run `tosram`, `toflash`, `make nnpa`, or any flash operation. No physical-board inference has yet been performed.

### Next safe step

Inspect the SDK's exact `tosram` target recipe, linker memory layout, and the generated ELF/map section addresses/size before loading. Confirm image/stack/BSS fit the SRAM region. Do not program external flash. Once confirmed, use `make all tosram board=EVBTINY` following the Evaluation Kit Wait Start sequence, then observe UART `/dev/ttyUSB2` at 115200 baud. The SRAM test pauses/changes the currently running LED firmware until the board is reset or booted back into the prior program; make sure the user understands this. Keep the verified sector 0x20000 and original backup files untouched.

## SRAM memory-layout inspection for own-count CNN firmware — 2026-11-18

User inspected generated ELF `build/own_count_cnn_sram_demo/own_count_cnn_sram_demo`.

- Linker SRAM region: ORIGIN 0x10000000, LENGTH 0x00020000 (128 KiB).
- ELF section addresses: `.vectors` at 0x10000000; `.text` at 0x10000138; `.rodata` at 0x10005D50; `.bss` at 0x1000D000 size 0x348; `.stack` at 0x1000D348 size 0x2000.
- The used region through stack top is approximately 0x1000F348, leaving about 0x10CB8 (68,792 bytes) below SRAM end 0x10020000.
- `riscv32-corev-elf-size` reported total 62,276 bytes (0xF344), consistent with the ELF section sizes.
- The user attempted `build/own_count_cnn_sram_demo.map`, but this file was not found there. The ELF section inspection nevertheless provides sufficient initial evidence for fit; no need to block on a map file.
- Runtime makefile source showed `tosram` invokes `colibry-sramtool.py <TARGET_BUILD_DIR>/<TARGET_NAME>.bin 0x10000000 --run`, while `toflash` invokes flash tool at address 0x0. User has not run either target.
- No SRAM load, physical inference, or flash write has occurred.

### Next safety check

Inspect `colibry-sramtool.py` source (the SRAM loader) and actual built `.bin` size/output path. Confirm it only writes the specified SRAM address and starts CPU, with no flash commands. Only after reviewing loader source should the SRAM test be run. The expected `.bin` is `build/own_count_cnn_sram_demo.bin` and is distinct from the ELF at `build/own_count_cnn_sram_demo/own_count_cnn_sram_demo`.

## SRAM loader source reviewed — 2026-11-18

User supplied the full `colibry-sramtool.py` source excerpt and artifact output.

- Loader path: `~/colibryNPU_SDK/riscv_runtime/tools/bin_loader/colibry-sramtool.py` (156 lines).
- Loader selects JTAG interface `Colibry_jtag_api(freq=2E6, channel=1)` by default.
- For a supplied address in `0x10000000..0x1001FFFF` (Colibry 128 KiB SRAM), it calls `cpu_halt()` and `bin_copy()` to that address.
- `bin_copy()` writes system-memory words through the JTAG access layer. The loader does not call flash-read/write/erase functions or the external flash access interface.
- With `--run`, the loader writes PLL control register 0x200100C0=0x10E1, SCU HF clock config register 0x20010000=2, calls `cpu_restart()` (default boot address 0x10000000), then writes SCU core control register 0x20010030=1. These are clock/core-control register writes; no flash operations are present in the shown loader source.
- SRAM application binary: `build/own_count_cnn_sram_demo.bin`, size shown as 49 KiB, SHA-256 `5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`.
- Exact runtime target in `runtime.mk`: `python3 $(SRAM_LOADER) $(TARGET_BUILD_DIR)/$(TARGET_NAME).bin 0x10000000 --run`.
- User has not yet run the SRAM loader. No physical-board inference has occurred and external flash remains untouched.

### Before invoking SRAM loader

Perform a local environment/serial preflight: confirm Python used for loader has `numpy` and `pyftdi`, check current USB device permissions and availability of a UART terminal (`picocom`, `minicom`, or `screen`), and confirm `/dev/ttyUSB2` is the ColibryProbe UART. Then open UART at 115200 baud before invoking loader. JTAG loader will halt current CPU; user LED blink will stop while SRAM test app runs. Firmware does a brief LED pulse after NNPA load then will keep LED off while it prints three test inferences and then loops forever. A power cycle/standard boot will return to flash-resident firmware, without changing external flash.

## SRAM-loader runtime dependency and UART preflight — 2026-11-18

User ran the dependency/device preflight:

- `~/colibry-flash-env/bin/python` has NumPy 2.5.4 and PyFtdi 0.57.2; JTAG modules import successfully.
- System `/usr/bin/python3` cannot import `pyftdi` (ModuleNotFoundError).
- `/dev/ttyUSB2` exists with owner root and group dialout, mode crw-rw----; manual identifies this as the UART VCOM at 115200 baud.
- No `picocom`, `minicom`, or `screen` executable is installed.
- SRAM firmware binary exists at `experiments/professor_demo/task2_small_cnn/build/own_count_cnn_sram_demo.bin`, 49 KiB, SHA-256 `5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`.

Do not use `make tosram` as written, because runtime.mk hardcodes `python3` and the system Python lacks PyFtdi. Instead, execute the exact SDK SRAM loader directly with `~/colibry-flash-env/bin/python`, passing the built binary, SRAM address 0x10000000, `--run`, and JTAG URL `ftdi://ftdi:4232hp:5:2/1`.

Before the loader, configure `/dev/ttyUSB2` using `stty -F ... 115200 raw -echo` and start `cat /dev/ttyUSB2` into a build log. Check that the capture process remains alive, then run the SRAM loader. After it returns, capture several additional seconds, terminate only the local `cat` process, and print the log. The board app should output model configuration and three sample predictions. It pulses the user LED after model configuration, then leaves it off while it loops.

No serial-terminal installation is needed. The direct SRAM loader writes only SRAM address 0x10000000 and starts the CPU; the inspected script contains no external flash write/erase path. The app itself embeds the NNPA bytes in its firmware image and copies them into NNPA SRAM. Do not use `toflash` or `make nnpa`.

## SRAM loader and UART preflight — 2026-11-18

User verified:
- `~/colibry-flash-env/bin/python` has NumPy 2.5.4 and PyFtdi 0.57.2; JTAG modules import successfully.
- System `python3` does not have PyFtdi. Therefore do not use the stock Makefile `tosram` target unchanged, because runtime.mk invokes system `python3`.
- `/dev/ttyUSB2` exists as root:dialout, mode 660. Evaluation manual identifies this as the UART at 115200 baud.
- No `picocom`, `minicom`, or `screen` executable is installed. UART capture can be performed using `stty` and `timeout cat` without installing anything.
- SRAM application binary path: `experiments/professor_demo/task2_small_cnn/build/own_count_cnn_sram_demo.bin`, size about 49 KiB, SHA-256 `5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`.

### Next step

Capture UART output to a log before launching the SRAM loader directly with `~/colibry-flash-env/bin/python` and the JTAG port `ftdi://ftdi:4232hp:5:2/1`. This is equivalent to the SDK `tosram` command: `<binary> 0x10000000 --run`. Do not invoke `make toflash` or `make nnpa`. Expect existing firmware LED blink to stop while CPU is halted; the loaded CNN app performs test and then loops. A power cycle/normal boot returns to flash-resident firmware.

## First SRAM loader invocation — UART capture expired — 2026-11-18

User ran the SDK SRAM loader through `~/colibry-flash-env/bin/python` with binary `build/own_count_cnn_sram_demo.bin`, address 0x10000000, `--run`, JTAG port channel 1.

Observed loader output:
- JTAG IDCODE 0x249511c3.
- CPU halted.
- `Writing to SRAM... Done`.
- `Starting CPU`.
- Loader exit code 0.

UART capture was started with `timeout 25s cat /dev/ttyUSB2`. Shell reported `[1]+ Exit 124 timeout 25s cat ...`; UART log was empty. Because the capture window timed out while the loader invocation was running, this does not establish whether firmware produced output or NNPA inference passed/failed. The loader success only proves that the SRAM loader accepted and copied the binary and issued CPU start sequence; it does not prove app execution.

### Next action

Retry the same SRAM-only load with a directly managed background `cat /dev/ttyUSB2` process (no 25 s timeout), start capture before invoking loader, wait after it returns, then stop only the cat process and inspect log. Reloading the same binary is safe for external flash; it reuses volatile SRAM and does not touch sector 0x20000. Continue not to run `toflash` or `make nnpa`.

## First successful physical-board NNPA inference — 2026-11-18

The user ran the SRAM-only CNN application. The SRAM loader returned exit code 0:
- JTAG IDCODE 0x249511c3.
- CPU halted, 49 KiB SRAM firmware copied, and loader printed Starting CPU.
- UART was captured continuously at 115200 baud through /dev/ttyUSB2.
- Firmware printed VDD=0.9V and own dataset CNN test banner.
- Embedded model size 19,392 bytes; NNPA model ID 0x723E.
- NNPA config-check routine printed `CRC_TRACE_PRE model=1212 status=0x00000106 int=0x00000007 trig=0x00000000 control=0x00000000 mask=0x00000002`, then `CRC_TRACE_POST status=0x00000105 int=0x00000007 trig=0x00000000 control=0x00000000 config_ok=0 event_end=1`, then reported `Bitstream loading: OK` and proceeded. Note the printed `config_ok=0` alongside success is internally inconsistent with a simple reading of the check code; investigate the volatile status register/read timing or log formatting before asserting exact CRC status value. Application proceeded to inference.
- Input size register 75 NNPA 16-byte words, exactly 1200 bytes; input tensor [1,30,40,1] uint8.
- Output start word 4096; output shape [1,1,3].
- Output quantization metadata: output scaling register 0x00000625; output offset register 0x0000F100.

Per-sample board raw outputs / argmax:
- `1_person/I1__008__120x160.png`, expected 1_person: raw uint8 [224,133,46], predicted 1_person — match.
- `2_person/I2__007__120x160.png`, expected 2_person: raw uint8 [223,166,26], predicted 1_person — mismatch.
- `3_person/I3__004__120x160.png`, expected 3_person: raw uint8 [146,185,78], predicted 2_person — mismatch.
- All three inference calls completed and app printed `ALL THREE NNPA INFERENCE CALLS COMPLETED`.
- External flash was not accessed or modified by the app.

Conclusion: first physical NNPA inference execution succeeded for three inputs and output shape. Only one of three sample labels matched. This is a hardware execution pass, not an accuracy pass. Do not state that the board people counter works reliably.

### Required validation before further training/deployment decisions

Compare these same 3 inputs and exact preprocessing (Pillow grayscale + Lanczos resize 160x120 to 40x30, uint8 input bytes) using the host INT8 TFLite file `~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite`. Print host raw uint8 outputs and argmax. Compare to board raw logits above. Previous image-level report matrix showed 3_person predicted as 1_person, whereas board produced 2_person on that exact named sample, so quantized host-vs-board parity must be established. If exact parity holds, proceed to data/model review; if not, inspect the NNPA output read/index/quantization and input pipeline before retraining.

Board app finishes in an infinite loop, with user LED turned off after initial brief pulse. A normal power cycle/boot returns to flash-resident application; no flash writes were performed.

## Host-vs-board parity attempt — input shape bug in diagnostic script — 2026-11-18

User ran the host TFLite comparison diagnostic. The correct PTQ TFLite artifact was loaded:
- Size: 17,312 bytes.
- SHA-256: `df8d84655a0c1142aa556120920587ce256f26bd14a0c0c5a8c598bfbfe9a45a`.
- Input tensor: `[1,30,40,1]`, dtype uint8, quantization `(0.003921568859368563, 0)`.
- Output tensor: `[1,3]`, dtype uint8, quantization `(0.024000564590096474, 241)`.
- The script failed at `set_tensor()` with `Dimension mismatch. Got 2 but expected 4` because the diagnostic helper's uint8 branch returned the 2-D Pillow array without adding batch/channel axes.
- This is a bug in the comparison script only; model load/tensor metadata succeeded. TensorFlow CUDA and XNNPACK informational messages are not the failure.

### Correct next step

Rerun host comparison using `tensor = image[np.newaxis, ..., np.newaxis]` for this model, since uint8 quantization scale is exactly 1/255 and zero point 0, so grayscale bytes can be passed unchanged. Compare output uint8 triplets / argmax to the board values `[224,133,46]`, `[223,166,26]`, `[146,185,78]` for the same exact filenames and Pillow Lanczos preprocessing. Do not change or retrain model yet; no new board operation required.

## Host INT8 TFLite versus physical NNPA output comparison — 2026-11-18

The user ran host TFLite inference on the same three images used by the SRAM board test, using Pillow grayscale + Lanczos resize to 40x30 and uint8 NHWC input [1,30,40,1].

Host model verification:
- PTQ TFLite size: 17,312 bytes.
- SHA-256: `df8d84655a0c1142aa556120920587ce256f26bd14a0c0c5a8c598bfbfe9a45a`.
- Input uint8 quantization: scale 0.003921568859368563 (~1/255), zero point 0.
- Output uint8 quantization: scale 0.024000564590096474, zero point 241.

Host vs board values:
- 1_person sample: host [213,126,43], board [224,133,46]. Absolute differences [11,7,3]. Both predict 1_person.
- 2_person sample: host [212,157,25], board [223,166,26]. Absolute differences [11,9,1]. Both predict 1_person (mismatch to expected 2_person).
- 3_person sample: host [138,176,74], board [146,185,78]. Absolute differences [8,9,4]. Both predict 2_person (mismatch to expected 3_person).
- All three class argmax predictions agree host vs board. Maximum raw uint8 difference is 11 output levels, equivalent to about 0.264 after multiplying by the output scale 0.02400056.
- Raw outputs are not bit-exact. The data supports class-decision agreement on these three inputs and modest numerical differences, but this is not a full numerical parity guarantee or accuracy validation.

Conclusion: SRAM board execution and host INT8 TFLite class decisions agree for the three sampled inputs. Hardware path is confirmed functional at this limited sample level. Model quality remains inadequate for 2_person and 3_person. The current data contains only 8 two-person and 4 three-person examples, with all four three-person images belonging to a single source group. Collect more independent examples (especially 2_person and 3_person) before retraining and any robust counting claims.

Board raw-output test did not modify external flash. The board app loops indefinitely after completing the three samples; a normal power cycle/boot returns to the flash-resident firmware.

## Inference parity and GitHub project reorganization — 2026-11-18

### Host vs board comparison

On the same 3 images, host INT8 TFLite and Colibry NNPA produced matching class argmax predictions:
- 1-person sample: host [213,126,43], board [224,133,46], both 1_person.
- 2-person sample: host [212,157,25], board [223,166,26], both 1_person.
- 3-person sample: host [138,176,74], board [146,185,78], both 2_person.
- Raw uint8 outputs differed by max 11 levels; output quantization scale is 0.02400056459, hence max dequantized-scale difference about 0.264. Exact raw output parity is not established.
- All three class decisions match. Only one of three true labels was correctly classified. Model quality is the limiting factor, not a mismatch in these three class decisions.

### GitHub structure reorganized

Own-dataset CNN code was separated from the professor assignment index. New structure:

- `projects/own_dataset_person_count_cnn/README.md` — project overview, dataset/model status, hardware test status.
- `projects/own_dataset_person_count_cnn/training/train_own_count_cnn.py` — host training entry point. ROOT path updated to repository root; default output is `projects/own_dataset_person_count_cnn/models/host_experiment/`.
- `projects/own_dataset_person_count_cnn/training/export_legacy_keras_for_colibry.py` — legacy tf_keras exporter. Default source points to the new project-local host experiment output; legacy H5 output remains `~/models/edgeguard_own_count/own_count_cnn_legacy.h5`.
- `projects/own_dataset_person_count_cnn/training/README.md` — host training and compatibility export guide.
- `projects/own_dataset_person_count_cnn/model_export/README.md` — PTQ TFLite and NNPA hashes/metadata.
- `projects/own_dataset_person_count_cnn/board_firmware/` — `main.c`, `Makefile`, `generate_board_data.py`, `.gitignore`, and detailed README.
- `experiments/professor_demo/README.md` is now an assignment index. Task 2 links to the dedicated project.
- `simple_person_cnn/README.md` now clearly identifies that directory as the older separate binary person/no_person experiment. Own count CNN training scripts have moved out of it.

Old paths deleted after destination files were created:
- `experiments/professor_demo/task2_small_cnn/*`
- `simple_person_cnn/train_own_count_cnn.py`
- `simple_person_cnn/export_legacy_keras_for_colibry.py`

GitHub operations only changed tracked repository files. The user's local untracked dataset images, local old model experiment outputs under `simple_person_cnn/model/own_count_experiment/`, external compiled models under `~/models/edgeguard_own_count/`, original flash backup, and current-sector backup were not modified by these GitHub operations.

### Next local action

Safely fetch origin/main and fast-forward the Ubuntu working tree after confirming it is an ancestor and that untracked paths do not conflict. Then verify the new trainer syntax/check-only and new board firmware build under `projects/own_dataset_person_count_cnn/board_firmware/`. Because previous board firmware was tested, do not immediately rerun loader; the board's SRAM-resident app may still be running. A normal power cycle returns to flash firmware; no external flash write is needed.

## Ubuntu fast-forward and rebuild from reorganized CNN project — 2026-11-18

User updated the Ubuntu working tree from origin/main successfully:
- Local HEAD advanced from 494e950 to da26176 using `git merge --ff-only origin/main`.
- Pre-existing untracked paths remained in status before update: `datasets/mobile_160x120/processed/`, `main`, and `simple_person_cnn/model/own_count_experiment/`.
- Python syntax checks passed for new project training script, legacy exporter, and board data generator.
- New board project path: `projects/own_dataset_person_count_cnn/board_firmware/`.
- `make prepare-data` succeeded and verified the 19,392-byte NNPA SHA-256 `ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e`; generated the board-data header from the canonical dataset archive containing three 1200-byte sample inputs.
- `make check-inputs` succeeded.
- `make all board=EVBTINY` completed C/C++ compilation, linker, objcopy and SDK image generation. Reported size: text 49,744; data 20; bss 12,512; total 62,276 bytes (0xF344).
- New SDK outputs include SRAM binary `build/own_count_cnn_sram_demo.bin` and flash-oriented variants. These are generated artifacts only. User did not run `tosram`, `toflash`, `make nnpa`, or any flash loader in this rebuild.
- No external flash programming occurred.

### Current next step

The same source firmware had already passed physical board inference before reorganization; the new-location build compiles the same tracked main.c and generator. If a final artifact equivalence check is desired, compare the new `.bin` SHA-256 to the old binary SHA-256 `5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`. Because the build path changed, debug-path-dependent ELF details may differ, but the binary should be compared before claiming identical bytecode. Do not rerun the board unless needed. Preserve local untracked image/model files and flash backups.

## Reorganized firmware binary identity confirmed — 2026-11-18

User compared the SRAM firmware binary rebuilt from `projects/own_dataset_person_count_cnn/board_firmware/` to the previously board-tested binary.

- New binary SHA-256: `5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`.
- Previously tested binary SHA-256: `5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`.
- Result: EXACT MATCH. The newly organized project builds byte-identical firmware to the firmware that already ran successfully on the physical Colibry board.
- Therefore no need to rerun or reload the board solely due to folder reorganization.
- The first physical board inference results remain applicable to the new project path: all 3 NNPA calls completed, host and board argmax agreed for the three samples, but only 1/3 labels were correctly classified.
- No flash operations occurred.

## Clarification: host training, board evaluation, and persistent deployment — 2026-11-18

The user asked whether the existing images can be used to train/test on the board and flash it for people detection.

Confirmed distinctions:
- The Colibry NNPA is an inference accelerator; train weights on the Ubuntu host using TensorFlow. Do not describe this as on-board training.
- The 3-class CNN has been executed on the physical board using an SRAM-only test application and 3 static images. Host and board argmax decisions matched for all three, but only 1/3 labels matched. Raw uint8 outputs differed by <=11 levels.
- A larger board evaluation using all 71 images is possible, but the current firmware embeds only 3 samples. Embedding all 71 at once would add 81,600 bytes of input payload; total SRAM binary could exceed the 128 KiB SRAM limit. Prefer batches of roughly 8-10 images per SRAM firmware build or implement host-to-board UART streaming if justified. Do not assume all 71 fit in one firmware.
- Dataset has 59 1_person, 8 2_person, 4 3_person images; all 4 3_person images belong to one source group. The previous train/validation split leaked source groups. Full-dataset board evaluation would include images already used for training and should be reported as a dataset inference audit, not independent accuracy/generalization.
- Current model is a whole-image count classifier with 3 outputs: `1_person`, `2_person`, `3_person`. It has no zero-person class, no >3 class and no bounding-box/localization output. It cannot yet be described as a general live person detector.
- The current SRAM demo uses three static sample images embedded in the application; it does not capture live camera frames.
- The existing external flash sector 0x20000 currently contains historical detector NNPA bytes (14,224 bytes) followed by original tail. The new own-count NNPA is 19,392 bytes. Replacing the persistent model requires a carefully reconstructed sector image and confirmed boot firmware/runtime that loads the new bitstream; persistent firmware also needs a camera capture/preprocessing/inference path for live detection.
- Do not flash or erase sector 0x20000 yet. Original/current 64 KiB backups remain preserved.

### Recommended next steps

1. Improve dataset breadth, particularly independently captured 2_person and 3_person scenes; confirm/label the 3 pending zero-person originals separately before deciding whether to extend the class set.
2. Review/adjust host model and evaluate with a source-group-aware protocol where feasible. With all 3_person examples in one group, independent 3_person generalization is not measurable until additional groups are collected.
3. Run a batch-based board evaluation using SRAM-only firmware and compare board outputs with host INT8 TFLite. Use a small number of images per batch to remain within 128 KiB SRAM.
4. Only after model behavior and camera inference runtime are verified should we consider a persistent flash deployment.