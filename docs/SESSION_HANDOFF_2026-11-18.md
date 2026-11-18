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
