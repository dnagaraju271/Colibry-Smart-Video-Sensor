#!/usr/bin/env python3
"""Read the Colibry JTAG IDCODE only.

This intentionally uses PyFtdi's JtagEngine directly instead of constructing
Colibry_jtag_api, whose constructor continues into jtag_init() after reading
IDCODE. The script resets only the JTAG TAP state machine; it does not call
the CPU halt/restart methods, access MMAP registers, or touch flash.
"""

from __future__ import annotations

import argparse
import sys


EXPECTED_IDCODE = 0x249511C3


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Read-only Colibry JTAG TAP IDCODE probe."
    )
    parser.add_argument(
        "--port",
        required=True,
        help="Current explicit FTDI URI, e.g. ftdi://ftdi:4232hp:3:4/1.",
    )
    parser.add_argument(
        "--freq",
        type=int,
        default=400_000,
        help="JTAG clock in Hz (default: 400000).",
    )
    args = parser.parse_args()

    if args.freq < 1 or args.freq > 10_000_000:
        parser.error("--freq must be between 1 and 10000000 Hz")

    try:
        from pyftdi.bits import BitSequence
        from pyftdi.jtag import JtagEngine
    except ImportError as exc:
        print(
            f"ERROR: cannot import PyFtdi JTAG support: {exc}",
            file=sys.stderr,
        )
        return 2

    jtag = None
    try:
        print("========== READ-ONLY JTAG IDCODE PROBE ==========")
        print(f"FTDI URI: {args.port}")
        print(f"JTAG frequency: {args.freq} Hz")
        print("Scope: JTAG TAP reset and IDCODE instruction/read only.")
        print("No CPU halt/restart, MMAP access, SPI transaction, or flash write.")

        jtag = JtagEngine(trst=False, frequency=args.freq)
        jtag.configure(args.port)
        jtag.reset()
        idcode_instruction = BitSequence("00001", msb=True, length=5)
        jtag.write_ir(idcode_instruction)
        idcode = int(jtag.read_dr(32))
        jtag.go_idle()

        print(f"JTAG IDCODE: 0x{idcode:08X}")
        if idcode == EXPECTED_IDCODE:
            print("RESULT: PASS — expected Colibry IDCODE received.")
            return 0

        print(
            f"RESULT: UNEXPECTED IDCODE — expected 0x{EXPECTED_IDCODE:08X}; "
            "do not run any recovery/reset operation yet."
        )
        return 3

    except Exception as exc:
        print(
            f"ERROR: JTAG probe failed: {type(exc).__name__}: {exc}",
            file=sys.stderr,
        )
        return 4
    finally:
        if jtag is not None:
            try:
                jtag.close()
            except Exception:
                pass


if __name__ == "__main__":
    raise SystemExit(main())
