#!/usr/bin/env python3
"""Read-only health probe for the Colibry SPIS/MMAP link.

This script only reads MMAP addresses. It does not write to SCU registers,
trigger inference, reset the CPU, or modify flash. The 0xF8000000 NOP pattern
repeated as a result for unrelated registers is treated as invalid readback,
not as a real register value.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

NNPA_REG_BASE = 0x30000000
REGISTERS = (
    ("NNPA status", NNPA_REG_BASE + 0x0C),
    ("Model size", NNPA_REG_BASE + 0x54),
    ("Input size", NNPA_REG_BASE + 0x58),
    ("Output size", NNPA_REG_BASE + 0x6C),
    ("Output start", NNPA_REG_BASE + 0x74),
    ("Inference counter", NNPA_REG_BASE + 0x78),
    ("SCU message 0", 0x20010050),
)
SPIS_NOP_PATTERN = 0xF8000000


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Read-only Colibry SPIS register probe.")
    parser.add_argument(
        "--sdk-root", type=Path, default=Path.home() / "colibryNPU_SDK",
        help="Local ColibryNPU SDK root (default: ~/colibryNPU_SDK).",
    )
    parser.add_argument(
        "--port", default=None,
        help="Optional explicit FTDI URI, e.g. ftdi://ftdi:4232hp:3:4/2.",
    )
    parser.add_argument(
        "--repeats", type=int, default=3,
        help="How many times to read each register (default: 3).",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if args.repeats < 1 or args.repeats > 10:
        print("ERROR: --repeats must be between 1 and 10.", file=sys.stderr)
        return 2

    sdk_root = args.sdk_root.expanduser()
    candidates = (sdk_root / "riscv_runtime" / "tools", sdk_root / "tools")
    for candidate in candidates:
        if (candidate / "colibry_interfaces" / "colibry_spis_api.py").is_file():
            sys.path.insert(0, str(candidate))
            break
    else:
        print(f"ERROR: SDK SPIS API not found under {sdk_root}", file=sys.stderr)
        return 2

    try:
        from colibry_interfaces.colibry_spis_api import Colibry_spis_api
        ll = Colibry_spis_api(freq=1_000_000, channel=2, p=args.port)
        print("========== READ-ONLY SPIS HEALTH PROBE ==========")
        print("No inference, reset, or flash write will be performed.")
        all_values: dict[str, list[int]] = {}
        for label, address in REGISTERS:
            values = [ll.mmap_read(address) for _ in range(args.repeats)]
            all_values[label] = values
            formatted = ", ".join(f"0x{value:08X}" for value in values)
            print(f"{label:20s} @ 0x{address:08X}: {formatted}")

        all_nop = all(
            value == SPIS_NOP_PATTERN
            for values in all_values.values()
            for value in values
        )
        if all_nop:
            print("RESULT: INVALID READBACK — every register returned 0xF8000000.")
            print("This is the SPIS NOP pattern, not valid values for these registers.")
            print("Do not run the output-window dump until communication is restored.")
            return 3

        status_values = all_values["NNPA status"]
        if all(value == SPIS_NOP_PATTERN for value in status_values):
            print("RESULT: STATUS READ INVALID — cannot confirm NNPA readiness.")
            print("Other register results may also be unreliable.")
            return 3

        print("RESULT: Not all reads equal the NOP pattern; inspect each value above.")
        print("Compare unrelated registers across repeats; repeated 0xF8000000 values are invalid.")
        print("=================================================")
        return 0

    except Exception as exc:
        print(f"ERROR: SPIS health probe failed: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 4


if __name__ == "__main__":
    raise SystemExit(main())
