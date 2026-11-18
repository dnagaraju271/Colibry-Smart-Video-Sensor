#!/usr/bin/env python3
"""Read a flash region through JTAG/SPIMF while invalidating the SDK mmap cache.

The SDK's JtagSpimfInterface.exchange() interleaves mmap_write(TX_PUSH) and
mmap_read(RX_POP). mmap_read() changes the system-bus address but does not
update Colibry_jtag_api.self_curr_mmap_waddr. On full 16-byte FIFO iterations,
exchange() does not rewrite SPIMF_LEN_ADDR between chunks; the next TX_PUSH
write can therefore skip restoring sbaddress0 and target RX_POP instead.

This helper wraps mmap_read() to invalidate that cached address after every
read. It only calls JEDEC READ ID (0x9F) and flash-array READ (0x03).
It never calls any flash write, register-write, erase, or check-only method.

WARNING: JtagSpimfInterface initialization halts the Colibry CPU to configure
SPIMF. The finally block sends cpu_restart() using boot address 0x10000000.
"""

from __future__ import annotations

import argparse
import hashlib
import importlib.util
import sys
from pathlib import Path
from typing import Any


def load_flash_tool(sdk_root: Path) -> Any:
    tools_dir = sdk_root / "riscv_runtime" / "tools"
    flash_tool = tools_dir / "bin_loader" / "colibry-flashtool.py"

    if not flash_tool.is_file():
        raise FileNotFoundError(f"Flash-tool source not found: {flash_tool}")

    sys.path.insert(0, str(tools_dir))
    spec = importlib.util.spec_from_file_location(
        "colibry_flashtool_cache_safe", str(flash_tool)
    )
    if spec is None or spec.loader is None:
        raise RuntimeError(f"Cannot load flash-tool module: {flash_tool}")

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Read flash using JTAG/SPIMF with mmap cache invalidation."
    )
    parser.add_argument(
        "--sdk-root",
        type=Path,
        default=Path.home() / "colibryNPU_SDK",
    )
    parser.add_argument(
        "--port",
        default="ftdi://ftdi:4232hp:5:2/1",
        help="PyFtdi JTAG interface URL",
    )
    parser.add_argument("--frequency", type=int, default=400000)
    parser.add_argument("--address", type=lambda x: int(x, 0), default=0x20000)
    parser.add_argument("--size", type=lambda x: int(x, 0), default=65536)
    parser.add_argument("--chunk-size", type=int, default=256)
    parser.add_argument(
        "--output",
        type=Path,
        default=Path.home()
        / "models/edgeguard_own_count/flash_backups"
        / "flash_sector_0x20000_cache_safe_20261118.bin",
    )
    args = parser.parse_args()

    if args.address < 0 or args.address > 0xFFFFFF:
        parser.error("--address must fit the 24-bit flash address")
    if args.size <= 0 or args.address + args.size > 0x1000000:
        parser.error("--size/address range must fit the 24-bit flash array")
    if args.chunk_size < 4 or args.chunk_size % 4:
        parser.error("--chunk-size must be a positive multiple of 4 bytes")

    reader = None
    api = None
    restart_ok = False
    try:
        module = load_flash_tool(args.sdk_root)

        # Keep the partially initialized object so that a failure during
        # SPIMF setup can still be followed by an attempted CPU restart.
        reader = module.JtagSpimfInterface.__new__(
            module.JtagSpimfInterface
        )
        module.JtagSpimfInterface.__init__(
            reader,
            freq=args.frequency,
            channel=1,
            p=args.port,
        )
        api = reader.jtag_api

        # Critical workaround: mmap_read() sets sbaddress0 to RX_POP_ADDR but
        # leaves self_curr_mmap_waddr unchanged. Invalidate the cache even if
        # the read raises, so the next mmap_write() must restore its address.
        original_mmap_read = api.mmap_read

        def mmap_read_with_cache_invalidation(addr: int) -> int:
            try:
                return original_mmap_read(addr)
            finally:
                api.self_curr_mmap_waddr = None

        api.mmap_read = mmap_read_with_cache_invalidation

        flash = module.FlashAccess(reader)

        print("===== READ-ONLY FLASH IDENTIFICATION =====")
        if flash.check_flash_id():
            raise RuntimeError("Flash ID check failed; refusing to read array")

        print("\n===== READ-ONLY FLASH ARRAY READ =====")
        result = bytearray()
        for offset in range(0, args.size, args.chunk_size):
            count = min(args.chunk_size, args.size - offset)
            block = bytes(
                flash.read_flash_array(args.address + offset, count)
            )
            if len(block) != count:
                raise RuntimeError(
                    f"Short read at 0x{args.address + offset:X}: "
                    f"expected {count}, received {len(block)}"
                )
            result.extend(block)
            print(f"Read {len(result)} / {args.size} bytes")

        if len(result) != args.size:
            raise RuntimeError("Read length verification failed")

        args.output.parent.mkdir(parents=True, exist_ok=True)
        args.output.write_bytes(result)

        saved = args.output.read_bytes()
        digest = hashlib.sha256(result).hexdigest()
        saved_digest = hashlib.sha256(saved).hexdigest()
        if saved_digest != digest or len(saved) != args.size:
            raise RuntimeError("Backup file size or SHA-256 verification failed")

        print("\n===== BACKUP VERIFIED =====")
        print(f"Address:     0x{args.address:06X}")
        print(f"Size:        {len(saved)} bytes")
        print(f"Output:      {args.output}")
        print(f"SHA-256:     {digest}")
        print("Read method: cache-invalidated RX FIFO reads; 256-byte transactions")

    finally:
        print("\n===== CPU RESTART CLEANUP =====")
        if api is None and reader is not None:
            api = getattr(reader, "jtag_api", None)

        if api is not None:
            try:
                api.cpu_restart()
                restart_ok = True
                print("CPU restart command sent (boot address 0x10000000).")
            except Exception as exc:  # report cleanup failure explicitly
                print(f"CRITICAL: CPU restart failed: {exc!r}")

            try:
                api.jtag.close()
            except Exception as exc:
                print(f"JTAG close warning: {exc!r}")
        else:
            print("CRITICAL: JTAG API unavailable for automatic CPU restart.")

        if not restart_ok:
            print("Check the board before attempting any further operation.")

    if not restart_ok:
        raise RuntimeError("Backup finished or failed, but CPU restart was not confirmed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
