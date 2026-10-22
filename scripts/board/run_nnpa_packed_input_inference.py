#!/usr/bin/env python3
"""Run one packed 160x128x4 input through the currently loaded Colibry NNPA model.

This sends an input buffer to the running firmware over SPIS and requests one
inference. It does not reset the board, flash the model, or claim to perform
person detection. The current YOLO backbone produces a feature map, not boxes.
"""

from __future__ import annotations

import argparse
import io
import sys
import time
from pathlib import Path


NNPA_REG_BASE = 0x30000000
NNPAMEM_BASE = 0x30001000
SCU_CORE_MSG0_ADDR = 0x20010050

REG_STATUS = 0x0C
REG_INPUT_START_MADDR = 0x18
REG_MODEL_SIZE = 0x54
REG_INPUT_SIZE = 0x58
REG_INFERENCE_COUNTER = 0x78
REG_OFMAP_START_MADDR = 0x74

MSG_WAIT_INFERENCE = 5
MSG_RUNNING = 10
EXPECTED_INPUT_SHAPE = (1, 3, 128, 160)
EXPECTED_PACKED_BYTES = 128 * 160 * 4


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Send a packed image to the running Colibry NNPA firmware."
    )
    parser.add_argument(
        "--sdk-root",
        type=Path,
        default=Path.home() / "colibryNPU_SDK",
        help="Local ColibryNPU SDK root (default: ~/colibryNPU_SDK).",
    )
    parser.add_argument(
        "--input",
        type=Path,
        default=(
            Path.home()
            / "models/colibry_yolo/compression_trial/"
            / "test_person_input_nnpa_packed.bin"
        ),
        help="Packed 81,920-byte input file.",
    )
    parser.add_argument(
        "--port",
        default=None,
        help=(
            "Optional explicit PyFTDI URI, e.g. "
            "ftdi://ftdi:4232hp:3:4/2. By default, detect the FTDI device."
        ),
    )
    parser.add_argument(
        "--timeout",
        type=float,
        default=15.0,
        help="Inference timeout in seconds (default: 15).",
    )
    return parser.parse_args()


def add_sdk_tools_to_path(sdk_root: Path) -> None:
    candidates = (
        sdk_root / "riscv_runtime" / "tools",
        sdk_root / "tools",
    )
    for candidate in candidates:
        if (candidate / "colibry_interfaces" / "colibry_spis_api.py").is_file():
            sys.path.insert(0, str(candidate))
            return
    raise FileNotFoundError(
        "Could not locate colibry_interfaces/colibry_spis_api.py under "
        f"{sdk_root}. Check --sdk-root."
    )


def main() -> int:
    args = parse_args()
    input_path = args.input.expanduser()
    if not input_path.is_file():
        print(f"ERROR: input file not found: {input_path}", file=sys.stderr)
        return 2

    packed_data = input_path.read_bytes()
    if len(packed_data) != EXPECTED_PACKED_BYTES:
        print(
            f"ERROR: expected {EXPECTED_PACKED_BYTES} input bytes, "
            f"found {len(packed_data)} in {input_path}",
            file=sys.stderr,
        )
        return 2

    try:
        add_sdk_tools_to_path(args.sdk_root.expanduser())
        from colibry_interfaces.colibry_spis_api import Colibry_spis_api
    except Exception as exc:
        print(f"ERROR: could not import the Colibry SPIS API: {exc}", file=sys.stderr)
        return 2

    ll = None
    try:
        # SPIS uses FTDI channel 2 at 1 MHz, matching the previously successful
        # board test. Passing port=None lets the SDK discover the current USB URI.
        ll = Colibry_spis_api(freq=1_000_000, channel=2, p=args.port)

        model_words = ll.mmap_read(NNPA_REG_BASE + REG_MODEL_SIZE)
        input_words = ll.mmap_read(NNPA_REG_BASE + REG_INPUT_SIZE)
        input_start = ll.mmap_read(NNPA_REG_BASE + REG_INPUT_START_MADDR)
        status_before = ll.mmap_read(NNPA_REG_BASE + REG_STATUS)
        counter_before = ll.mmap_read(NNPA_REG_BASE + REG_INFERENCE_COUNTER)
        msg_before = ll.mmap_read(SCU_CORE_MSG0_ADDR)

        input_addr = NNPAMEM_BASE + input_start * 16
        expected_bytes = input_words * 16

        print("========== COLIBRY NNPA REAL-INPUT TEST ==========")
        print(f"Input file:           {input_path}")
        print(f"Input bytes:          {len(packed_data)}")
        print(f"NNPA model size:      {model_words} 16-byte words")
        print(f"NNPA input size:      {input_words} 16-byte words")
        print(f"Input start word:     {input_start}")
        print(f"Input memory address: 0x{input_addr:08X}")
        print(f"Status before:        0x{status_before:08X}")
        print(f"Inference count:      {counter_before}")
        print(f"Firmware message:     {msg_before}")
        print("==================================================")

        if len(packed_data) != expected_bytes:
            print(
                f"ERROR: NNPA expects {expected_bytes} bytes from its input-size "
                f"register, but the file contains {len(packed_data)} bytes.",
                file=sys.stderr,
            )
            return 2
        if status_before & 0x1:
            print("ERROR: NNPA status reports busy; no input was sent.", file=sys.stderr)
            return 2
        if msg_before != 0:
            print(
                "ERROR: firmware message is not idle (expected 0). "
                "No input was sent; check the UART and firmware state.",
                file=sys.stderr,
            )
            return 2

        print("Transferring packed input over SPIS...")
        ll.bin_copy(io.BytesIO(packed_data), input_addr)
        print(f"Transferred {len(packed_data)} bytes.")

        # Request inference from the currently running firmware. Do not reset
        # the CPU or rewrite flash in this script.
        ll.mmap_write(SCU_CORE_MSG0_ADDR, MSG_WAIT_INFERENCE)
        print("Inference request sent; waiting for firmware completion...")

        deadline = time.monotonic() + args.timeout
        run_seen = False
        msg_after = msg_before
        counter_after = counter_before
        status_after = status_before

        while time.monotonic() < deadline:
            msg_after = ll.mmap_read(SCU_CORE_MSG0_ADDR)
            counter_after = ll.mmap_read(NNPA_REG_BASE + REG_INFERENCE_COUNTER)
            status_after = ll.mmap_read(NNPA_REG_BASE + REG_STATUS)
            if msg_after == MSG_RUNNING:
                run_seen = True
            if counter_after != counter_before and msg_after == 0:
                break
            time.sleep(0.05)
        else:
            print("ERROR: inference timed out.", file=sys.stderr)
            print(f"Firmware message: 0x{msg_after:08X}", file=sys.stderr)
            print(f"Inference count: {counter_after}", file=sys.stderr)
            print(f"NNPA status: 0x{status_after:08X}", file=sys.stderr)
            return 3

        ofmap_start = ll.mmap_read(NNPA_REG_BASE + REG_OFMAP_START_MADDR)
        result_message = ll.mmap_read(SCU_CORE_MSG0_ADDR + 4)

        print("========== INFERENCE RESULT ==========")
        print(f"Firmware running state observed: {run_seen}")
        print(f"Inference count before/after:     {counter_before} -> {counter_after}")
        print(f"Final firmware message:           {msg_after}")
        print(f"Final NNPA status:                0x{status_after:08X}")
        print(f"Output start word:                {ofmap_start}")
        print(f"Firmware result message (raw):    {result_message}")
        print("Note: the result message is not a person count or confidence score.")
        print("This model outputs a feature map; detection decoding is a separate step.")
        print("======================================")
        return 0

    except Exception as exc:
        print(
            f"ERROR: SPIS board operation failed: {type(exc).__name__}: {exc}",
            file=sys.stderr,
        )
        return 4


if __name__ == "__main__":
    raise SystemExit(main())
