# Session Handoff — 2026-11-15

## Resume point

Continue the main Colibry Smart Video Sensor person-detection project. The immediate next task is to restore read-only FTDI/SPI access so the existing flash sector can be verified without writing to it.

Do not repeat the detector recovery or compiler reproduction unless new evidence requires it.

## Verified detector artifacts

Recovered source model from the older local project clone:

- File: `models/colibry_person_detector_int8.tflite`
- Size: 35,280 bytes
- SHA-256: `686c65d098bedf7db36d873da4e315c5e5cf5cd45778457e98d2c12fce3c14e0`
- Input: `[1,128,160,1]`, INT8, scale `0.003921568859368563`, zero-point `-128`
- Output: `[1,32,40,5]`, INT8, scale `0.05866943299770355`, zero-point `87`

Colibry NPU compiler version 0.2.1 successfully compiled a temporary copy. Fresh output SHA-256:

`fe9897ff4694328b22fb3fc91fba9097ca353a18365c197f196011ed14be9fd2`

It exactly matches the older clone's `models/compiled/colibry_person_detector.nnpa` (14,224 bytes, model ID `0x3F7B`). Compiler report: 1,078,012 inference cycles, 17,373,440 MACs, estimated energy 58.3 µJ at estimated VDD 0.8 V.

The current clone's `models/compiled/colibry_person_detector_int8.tflite.nnpa` is also 14,224 bytes but has a different SHA-256:

`e122d3f17509ad22b6a5bb3f8ba0b4db3775bded00834c991b7f91b6eb35a717`

Do not treat these two binaries as identical or overwrite either without a separate decision.

## Historical flash recipe recovered (not re-executed)

Shell history contains a guarded 64 KiB flash-sector procedure:

- Start address: `0x20000`
- Sector number: `2`
- Sector size: 65,536 bytes
- Page size: 256 bytes
- Detector NNPA placed at sector start: 14,224 bytes
- Original tail preserved: 51,312 bytes
- Expected original backup SHA-256: `5e83c69f45d5914a775ae940cade8353d5cdb4fa605005ba22932c736999f987`
- Expected replacement-sector SHA-256: `efbdd2be348d78e98ee2f18168c36b8f2e3b94a354e6c28123242a3f4a5da00c`

The recipe checked flash ID and configuration, compared the live sector with the backup before erase, erased/programmed the sector, and verified the entire readback. Shell history preserves the procedure and expected hashes, but not the execution output. Thus, current flash contents remain unverified.

## Current FTDI/SPI blocker

Ubuntu `lsusb` successfully lists:

`Bus 005 Device 002: ID 0403:6043 Future Technology Devices International, Ltd Quad RS232-HS`

The initial `lsusb -nn` check was invalid on this older `lsusb` version; the corrected `lsusb` and `lsusb -d 0403:6043` checks confirmed the device is visible in the VM.

PyFtdi discovery:

`ValueError: The device has no langid (permission issue, no string descriptors supported or device error)`

The SDK flash utility subsequently reported:

`No FTDI SPI device found. Exiting.`

Therefore, the flash read did not start. No erase/program operation was performed during this verification attempt. Do not claim that the current flash matches either historical image.

## First action next session

Run only these read-only diagnostics in Ubuntu:

```bash
echo "===== USER AND GROUPS ====="
id

echo
echo "===== USB DEVICE PERMISSIONS ====="
ls -l /dev/bus/usb/005/002

echo
echo "===== FTDI SERIAL PORTS ====="
ls -l /dev/ttyUSB* 2>/dev/null || echo "No ttyUSB devices found"

echo
echo "===== USB DEVICE DETAILS ====="
lsusb -v -s 005:002 2>&1 | head -60
```

Use the output to distinguish Linux device permissions from USB descriptor/device issues. Do not change permissions, install packages, alter UTM settings, use `sudo` to bypass the error, or retry SPI flash access until the cause is understood.

After FTDI access is working, the next operation should be a strictly read-only 64 KiB read from address `0x20000`, followed by SHA-256 comparison with the known backup and replacement-sector hashes. Do not erase, program, or reboot as part of this verification.

## Camera and inference status

- Detector input contract is 160 × 128 grayscale.
- SDK `camera_init` documentation describes dimensions expected after the ISP.
- The inspected initializer selects QQVGA for dimensions up to 160 × 120; 160 × 128 follows the QVGA format branch.
- SDK camera example demonstrates a camera-to-NNPA data-loaded interrupt flow at 160 × 120, not proof of the exact 160 × 128 detector path.
- Exact ISP setup for 160 × 128 grayscale is not yet established.
- Live camera inference, output decoding, bounding boxes, and person/no-person physical tests remain unverified.

## Repository records

See:

- `README.md` — current overview and milestones.
- `docs/DEVELOPMENT_PROGRESS.md` — development status.
- `docs/DETECTOR_ARTIFACT_RECOVERY_2026-11-15.md` — detailed model hashes, tensor metadata, compiler results, flash recipe, USB blocker, and evidence limitations.

Keep original TFLite/NNPA files, flash backups, raw dumps, and generated binaries untouched. The audit records hashes and metadata; generated binary artifacts were intentionally not uploaded to Git.
