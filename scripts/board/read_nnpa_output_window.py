#!/usr/bin/env python3
"""Read a diagnostic memory window around the most recent NNPA output.

This is a read-only SPIS utility. It does not trigger inference, reset the CPU,
or write flash. The saved window is four bytes per logical output element as a
diagnostic hypothesis, not an assertion that this is the official tensor format.
"""

from __future__ import annotations

import argparse
import hashlib
import sys
import time
from pathlib import Path

NNPA_REG_BASE = 0x30000000
NNPAMEM_BASE = 0x30001000
NNPAMEM_BYTES = 640 * 1024
SCU_CORE_MSG0_ADDR = 0x20010050

REG_STATUS = 0x0C
REG_OFMAP_SIZE = 0x6C
REG_OFMAP_START_MADDR = 0x74
REG_INFERENCE_COUNTER = 0x78

NNPA_BYTES_PER_MEM_WORD = 16


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Read an extended memory window around the latest NNPA output."
    )
    parser.add_argument(
        "--sdk-root",
        type=Path,
        default=Path.home() / "colibryNPU_SDK",
        help="Local SDK root (default: ~/colibryNPU_SDK).",
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=(
            Path.home()
            / "models/colibry_yolo/compression_trial/"
            / "nnpa_output_memory_window4.bin"
        ),
        help="Destination for the raw diagnostic memory window.",
    )
    parser.add_argument(
        "--port",
        default=None,
        help="Optional explicit PyFTDI URI, e.g. ftdi://ftdi:4232hp:3:4/2.",
    )
    return parser.parse_args()


def decode_dims(register_value: int) -> tuple[int, int, int]:
    width = (register_value & 0x3FF) + 1
    height = ((register_value >> 10) & 0x3FF) + 1
    depth = ((register_value >> 20) & 0x3FF) + 1
    return width, height, depth


def main() -> int:
    args = parse_args()
    sdk_root = args.sdk_root.expanduser()
    candidates = (sdk_root / "riscv_runtime" / "tools", sdk_root / "tools")
    for candidate in candidates:
        if (candidate / "colibry_interfaces" / "colibry_spis_api.py").is_file():
            sys.path.insert(0, str(candidate))
            break
    else:
        print(
            f"ERROR: could not find colibry_interfaces under {sdk_root}",
            file=sys.stderr,
        )
        return 2

    try:
        from colibry_interfaces.colibry_spis_api import Colibry_spis_api
        ll = Colibry_spis_api(freq=1_000_000, channel=2, p=args.port)

        # Read the accelerator registers first and reject the known SPIS NOP
        # sentinel if it appears where a register value should be. The SCU message
        # read has been observed to sporadically return 0xF8000000 (SPIS NOP),
        # so sample it repeatedly instead of treating that sentinel as real data.
        status = ll.mmap_read(NNPA_REG_BASE + REG_STATUS)
        counter = ll.mmap_read(NNPA_REG_BASE + REG_INFERENCE_COUNTER)
        ofmap_start = ll.mmap_read(NNPA_REG_BASE + REG_OFMAP_START_MADDR)
        size_reg = ll.mmap_read(NNPA_REG_BASE + REG_OFMAP_SIZE)

        msg_samples = []
        for _ in range(4):
            msg_samples.append(ll.mmap_read(SCU_CORE_MSG0_ADDR))
            time.sleep(0.02)

        valid_messages = [value for value in msg_samples if value in (0, 5, 10)]
        print("========== READ-ONLY NNPA REGISTER SNAPSHOT ==========")
        print(f"Status samples:          0x{status:08X}")
        print(f"Inference counter:       {counter}")
        print(f"Output start word:       {ofmap_start}")
        print(f"Output size register:    0x{size_reg:08X}")
        print("SCU message samples:     " + ", ".join(f"0x{x:08X}" for x in msg_samples))
        print("======================================================")

        if (status & 0x1) != 0:
            raise RuntimeError(
                f"NNPA status indicates busy (0x{status:08X}); refusing readback."
            )
        if (status & 0x6) != 0x6:
            raise RuntimeError(
                f"NNPA status does not show expected ready bits (0x{status:08X}); "
                "register read may be invalid, refusing readback."
            )
        if counter > 1_000_000:
            raise RuntimeError(
                f"Inference counter looks invalid ({counter}); refusing readback."
            )
        if not ofmap_start or ofmap_start > 40959:
            raise RuntimeError(
                f"Output start address looks invalid ({ofmap_start}); refusing readback."
            )
        if valid_messages and valid_messages[-1] in (5, 10):
            raise RuntimeError(
                f"Latest valid firmware message is {valid_messages[-1]}, not idle; "
                "refusing readback."
            )
        if not valid_messages:
            print(
                "WARNING: SCU message reads returned no recognizable value "
                "(possibly SPIS NOP echo); proceeding based on validated, idle NNPA status."
            )

        width, height, depth = decode_dims(size_reg)
        if (width, height, depth) != (20, 16, 64):
            raise RuntimeError(
                f"Unexpected output dimensions {width}x{height}x{depth}; "
                "refusing the four-lane diagnostic."
            )
        elements = width * height * depth
        window_bytes = elements * 4
        addr = NNPAMEM_BASE + ofmap_start * NNPA_BYTES_PER_MEM_WORD
        end_addr = addr + window_bytes
        memory_end = NNPAMEM_BASE + NNPAMEM_BYTES
        if end_addr > memory_end:
            raise RuntimeError(
                f"Diagnostic window 0x{addr:08X}..0x{end_addr:08X} "
                f"exceeds NNPA memory end 0x{memory_end:08X}."
            )

        print("========== READ-ONLY NNPA OUTPUT WINDOW ==========")
        print(f"Inference counter:      {counter}")
        print(f"NNPA status:            0x{status:08X}")
        print(f"Firmware message:       {msg}")
        print(f"Output start word:      {ofmap_start}")
        print(f"Output size register:   0x{size_reg:08X}")
        print(f"Decoded shape:          [{height}, {width}, {depth}]")
        print(f"Logical elements:       {elements}")
        print(f"Diagnostic window size: {window_bytes} bytes (4 bytes/element hypothesis)")
        print(f"Address range:          0x{addr:08X}..0x{end_addr - 1:08X}")
        print("No inference trigger, reset, or flash write will be performed.")
        print("Reading diagnostic window...")
        values = ll.mmap_readburst(addr, window_bytes // 4, byte_array=True)
        data = bytes(values)
        if len(data) != window_bytes:
            raise RuntimeError(
                f"Read {len(data)} bytes, expected {window_bytes}."
            )

        path = args.output.expanduser()
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_bytes(data)
        digest = hashlib.sha256(data).hexdigest()
        print(f"Bytes read:             {len(data)}")
        print(f"SHA-256:                {digest}")
        print(f"Saved file:             {path}")
        print("==================================================")
        return 0
    except Exception as exc:
        print(f"ERROR: output window read failed: {type(exc).__name__}: {exc}", file=sys.stderr)
        return 3


if __name__ == "__main__":
    raise SystemExit(main())
