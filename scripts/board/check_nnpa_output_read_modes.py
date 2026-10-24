#!/usr/bin/env python3
"""Compare ordinary SPIS MMAP reads with an existing burst-captured output head.

Read-only diagnostic: it reads NNPA metadata and a small, bounded number of
32-bit words from the current output memory with mmap_read(). It does NOT use
mmap_readburst(), trigger inference, write registers, reset the CPU, or touch flash.
It then compares both byte orders with the corresponding prefix of the saved
raw output capture to identify a likely byte-order or burst-read issue.
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

NNPA_REG_BASE = 0x30000000
NNPAMEM_BASE = 0x30001000
NNPA_BYTES_PER_MEM_WORD = 16

REG_STATUS = NNPA_REG_BASE + 0x0C
REG_MODEL_SIZE = NNPA_REG_BASE + 0x54
REG_INPUT_SIZE = NNPA_REG_BASE + 0x58
REG_OFMAP_SIZE = NNPA_REG_BASE + 0x6C
REG_OFMAP_START = NNPA_REG_BASE + 0x74
REG_COUNTER = NNPA_REG_BASE + 0x78
SCU_CORE_MSG0_ADDR = 0x20010050

BAD_RESPONSES = {0xFFFFFFFF, 0xF8000000}


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Read output-memory prefix without DMA and compare with saved burst data."
    )
    parser.add_argument(
        "--sdk-root", type=Path, default=Path.home() / "colibryNPU_SDK",
        help="Colibry SDK root (default: ~/colibryNPU_SDK).",
    )
    parser.add_argument(
        "--raw-output", type=Path,
        default=Path.home() / "models/colibry_yolo/compression_trial/nnpa_output_featuremap_after_reload_20261024.bin",
        help="Previously captured raw output file.",
    )
    parser.add_argument(
        "--words", type=int, default=8,
        help="Number of 32-bit words to read individually (1..16; default: 8).",
    )
    parser.add_argument(
        "--port", default=None,
        help="Optional explicit SPIS FTDI URI. Omit to auto-detect the board.",
    )
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    if not 1 <= args.words <= 16:
        print("ERROR: --words must be between 1 and 16.", file=sys.stderr)
        return 2
    raw_path = args.raw_output.expanduser()
    if not raw_path.is_file():
        print(f"ERROR: saved output file not found: {raw_path}", file=sys.stderr)
        return 2

    sdk_root = args.sdk_root.expanduser()
    candidates = (sdk_root / "riscv_runtime" / "tools", sdk_root / "tools")
    for candidate in candidates:
        if (candidate / "colibry_interfaces" / "colibry_spis_api.py").is_file():
            sys.path.insert(0, str(candidate))
            break
    else:
        print(f"ERROR: could not find Colibry SPIS API under {sdk_root}", file=sys.stderr)
        return 2

    try:
        from colibry_interfaces.colibry_spis_api import Colibry_spis_api

        ll = Colibry_spis_api(freq=1_000_000, channel=2, p=args.port)
        regs = {
            "status": ll.mmap_read(REG_STATUS),
            "model_size_words": ll.mmap_read(REG_MODEL_SIZE),
            "input_size_words": ll.mmap_read(REG_INPUT_SIZE),
            "output_size": ll.mmap_read(REG_OFMAP_SIZE),
            "output_start_word": ll.mmap_read(REG_OFMAP_START),
            "inference_counter": ll.mmap_read(REG_COUNTER),
            "firmware_message": ll.mmap_read(SCU_CORE_MSG0_ADDR),
        }

        print("========== READ-ONLY OUTPUT READ-MODE DIAGNOSTIC ==========")
        print("No burst DMA, inference trigger, register write, CPU reset, or flash access.")
        for label, value in regs.items():
            print(f"{label:22s}: 0x{value:08X} ({value})")

        bad = {key: value for key, value in regs.items() if value in BAD_RESPONSES}
        if bad:
            print("RESULT: STOP — invalid sentinel response in metadata; no memory sample attempted.")
            return 3

        status = regs["status"]
        if (status & 0x1) or (status & 0x6) != 0x6:
            print(f"RESULT: STOP — NNPA status is not the expected idle/ready value (0x{status:08X}).")
            return 3

        output_start = regs["output_start_word"]
        size_reg = regs["output_size"]
        width = (size_reg & 0x3FF) + 1
        height = ((size_reg >> 10) & 0x3FF) + 1
        depth = ((size_reg >> 20) & 0x3FF) + 1
        elements = width * height * depth
        output_bytes = elements
        if output_start == 0 or output_bytes <= 0:
            print("RESULT: STOP — output address/size is not valid for sampling.")
            return 3
        if args.words * 4 > output_bytes:
            print("RESULT: STOP — requested sample exceeds decoded output length.")
            return 3

        address = NNPAMEM_BASE + output_start * NNPA_BYTES_PER_MEM_WORD
        print(f"Decoded output shape : [{height}, {width}, {depth}]")
        print(f"Output base address  : 0x{address:08X}")
        print(f"Sampling             : {args.words} individually read 32-bit words")
        words = [ll.mmap_read(address + i * 4) for i in range(args.words)]
        if any(word in BAD_RESPONSES for word in words):
            print("RESULT: INVALID — one or more direct MMAP word reads returned a sentinel.")
            print("Words: " + " ".join(f"0x{x:08X}" for x in words))
            return 3

        big_bytes = b"".join(word.to_bytes(4, "big") for word in words)
        little_bytes = b"".join(word.to_bytes(4, "little") for word in words)
        raw = raw_path.read_bytes()
        nbytes = args.words * 4
        if len(raw) < nbytes:
            print(f"ERROR: saved capture has only {len(raw)} bytes; need {nbytes}.")
            return 2
        raw_head = raw[:nbytes]

        print("Direct MMAP words    : " + " ".join(f"0x{x:08X}" for x in words))
        print(f"Direct bytes big-end : {big_bytes.hex(' ')}")
        print(f"Direct bytes little  : {little_bytes.hex(' ')}")
        print(f"Saved burst prefix   : {raw_head.hex(' ')}")
        print(f"Big-endian match     : {big_bytes == raw_head}")
        print(f"Little-endian match  : {little_bytes == raw_head}")
        if big_bytes == raw_head or little_bytes == raw_head:
            print("RESULT: DIRECT MMAP SAMPLE MATCHES THE SAVED BURST PREFIX IN ONE BYTE ORDER.")
        else:
            print("RESULT: DIRECT MMAP SAMPLE DIFFERS FROM THE SAVED BURST PREFIX.")
            print("This points toward the burst-read path/format or a changing memory window; inspect before any new burst.")
        print("==========================================================")
        return 0

    except Exception as exc:
        print(f"ERROR: output read-mode diagnostic failed: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 4


if __name__ == "__main__":
    raise SystemExit(main())
