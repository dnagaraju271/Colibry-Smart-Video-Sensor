#!/usr/bin/env python3
"""Read one small, bounded chunk from the current NNPA output buffer.

This is a diagnostic readback tool, not a detector. It reads metadata through
SPIS and performs at most one output DMA burst of 32 bytes. It does not trigger
inference, write MMAP registers, reset the CPU, or write/erase external flash.

After the test, run the read-only SPIS health probe to confirm that ordinary
register reads still work. A burst read itself changes the target's peripheral
state, so this tool deliberately supports only one 32-byte burst per invocation.
"""

from __future__ import annotations

import argparse
import hashlib
import sys
from pathlib import Path

NNPA_REG_BASE = 0x30000000
NNPAMEM_BASE = 0x30001000
NNPA_BYTES_PER_MEM_WORD = 16
SCU_CORE_MSG0_ADDR = 0x20010050

REG_STATUS = NNPA_REG_BASE + 0x0C
REG_OFMAP_SIZE = NNPA_REG_BASE + 0x6C
REG_OFMAP_START = NNPA_REG_BASE + 0x74
REG_COUNTER = NNPA_REG_BASE + 0x78
BAD_RESPONSES = {0xFFFFFFFF, 0xF8000000}
MAX_BURST_BYTES = 32  # SDK SPIS burst transaction is eight 32-bit words.


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Read one 4..32-byte chunk from a completed NNPA output."
    )
    parser.add_argument(
        "--sdk-root", type=Path, default=Path.home() / "colibryNPU_SDK",
        help="SDK root (default: ~/colibryNPU_SDK).",
    )
    parser.add_argument(
        "--port", default=None,
        help="Explicit SPIS FTDI URI, e.g. ftdi://ftdi:4232hp:3:4/2.",
    )
    parser.add_argument(
        "--offset", type=int, default=0,
        help="Byte offset from the output buffer start (default: 0; multiple of 4).",
    )
    parser.add_argument(
        "--bytes", dest="byte_count", type=int, default=32,
        help="Bytes to read in this single burst (4..32; multiple of 4; default 32).",
    )
    parser.add_argument(
        "--output", type=Path,
        default=(
            Path.home() / "models/colibry_yolo/compression_trial/"
            / "nnpa_output_chunk_20261026.bin"
        ),
        help="File for the small captured chunk.",
    )
    return parser.parse_args()


def add_sdk_tools_to_path(sdk_root: Path) -> None:
    candidates = (sdk_root / "riscv_runtime" / "tools", sdk_root / "tools")
    for candidate in candidates:
        if (candidate / "colibry_interfaces" / "colibry_spis_api.py").is_file():
            sys.path.insert(0, str(candidate))
            return
    raise FileNotFoundError(f"Could not locate Colibry SPIS API under {sdk_root}")


def decode_dims(size_register: int) -> tuple[int, int, int]:
    width = (size_register & 0x3FF) + 1
    height = ((size_register >> 10) & 0x3FF) + 1
    depth = ((size_register >> 20) & 0x3FF) + 1
    return width, height, depth


def main() -> int:
    args = parse_args()
    if args.offset < 0 or args.offset % 4:
        print("ERROR: --offset must be non-negative and divisible by 4.", file=sys.stderr)
        return 2
    if not 4 <= args.byte_count <= MAX_BURST_BYTES or args.byte_count % 4:
        print("ERROR: --bytes must be 4..32 and divisible by 4.", file=sys.stderr)
        return 2

    try:
        add_sdk_tools_to_path(args.sdk_root.expanduser())
        from colibry_interfaces.colibry_spis_api import Colibry_spis_api
    except Exception as exc:
        print(f"ERROR: could not import the Colibry SPIS API: {exc}", file=sys.stderr)
        return 2

    ll = None
    try:
        ll = Colibry_spis_api(freq=1_000_000, channel=2, p=args.port)
        metadata = {
            "status": ll.mmap_read(REG_STATUS),
            "output_start_word": ll.mmap_read(REG_OFMAP_START),
            "output_size_register": ll.mmap_read(REG_OFMAP_SIZE),
            "inference_counter": ll.mmap_read(REG_COUNTER),
            "firmware_message": ll.mmap_read(SCU_CORE_MSG0_ADDR),
        }

        print("========== SINGLE BOUNDED OUTPUT READ ==========")
        print("No inference trigger, MMAP write, CPU reset, or flash write.")
        for name, value in metadata.items():
            print(f"{name:22s}: 0x{value:08X} ({value})")

        bad = {name: value for name, value in metadata.items() if value in BAD_RESPONSES}
        if bad:
            print("RESULT: STOP — invalid SPIS sentinel in metadata; no output burst attempted.")
            return 3

        status = metadata["status"]
        if (status & 0x1) or (status & 0x6) != 0x6:
            print(f"RESULT: STOP — NNPA status is not idle/ready: 0x{status:08X}.")
            return 3
        if metadata["firmware_message"] != 0:
            print("RESULT: STOP — firmware is not idle; no output burst attempted.")
            return 3

        output_start = metadata["output_start_word"]
        width, height, depth = decode_dims(metadata["output_size_register"])
        output_length = width * height * depth
        if output_start == 0 or output_length != 20 * 16 * 64:
            print(
                f"RESULT: STOP — unexpected output metadata (start={output_start}, "
                f"dims={width}x{height}x{depth})."
            )
            return 3
        if args.offset + args.byte_count > output_length:
            print("ERROR: requested chunk extends beyond the output buffer.", file=sys.stderr)
            return 2

        output_addr = NNPAMEM_BASE + output_start * NNPA_BYTES_PER_MEM_WORD + args.offset
        print(f"Output base address:   0x{(NNPAMEM_BASE + output_start * NNPA_BYTES_PER_MEM_WORD):08X}")
        print(f"Chunk address:         0x{output_addr:08X}")
        print(f"Chunk offset:          {args.offset}")
        print(f"Chunk bytes:           {args.byte_count}")
        print("Issuing one bounded burst read...")
        data = bytes(ll.mmap_readburst(output_addr, args.byte_count // 4, byte_array=True))
        if len(data) != args.byte_count:
            print(
                f"ERROR: got {len(data)} bytes, expected {args.byte_count}.",
                file=sys.stderr,
            )
            return 4

        path = args.output.expanduser()
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        print(f"Bytes saved:           {len(data)}")
        print(f"Chunk hex:             {data.hex(' ')}")
        print(f"Chunk SHA-256:         {hashlib.sha256(data).hexdigest()}")
        print(f"Saved file:            {path}")
        print("NEXT: run the read-only SPIS health probe once; do not repeat this burst yet.")
        print("================================================")
        return 0
    except Exception as exc:
        print(f"ERROR: bounded output read failed: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 4
    finally:
        if ll is not None:
            try:
                ll.cleanup()
            except Exception:
                pass


if __name__ == "__main__":
    raise SystemExit(main())
