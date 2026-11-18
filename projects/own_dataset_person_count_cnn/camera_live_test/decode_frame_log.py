#!/usr/bin/env python3
"""Recreate the 40x30 grayscale frame from UART FRAME_ROW records."""
from __future__ import annotations
import argparse
import re
from pathlib import Path
from PIL import Image

ROW_RE = re.compile(r"^FRAME_ROW,(\d{2}),([0-9A-Fa-f]{80})$")

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("uart_log", type=Path)
    parser.add_argument("--output", type=Path, default=Path("captured_pag7920_frame.png"))
    args = parser.parse_args()

    rows: dict[int, bytes] = {}
    for line in args.uart_log.read_text(encoding="utf-8", errors="replace").splitlines():
        match = ROW_RE.match(line.strip())
        if not match:
            continue
        index = int(match.group(1))
        if index in rows:
            raise SystemExit(f"Duplicate FRAME_ROW {index}")
        rows[index] = bytes.fromhex(match.group(2))

    missing = sorted(set(range(30)) - set(rows))
    if missing or len(rows) != 30:
        raise SystemExit(f"Expected 30 frame rows; missing={missing}; received={len(rows)}")

    pixels = b"".join(rows[i] for i in range(30))
    if len(pixels) != 1200:
        raise SystemExit(f"Expected 1200 pixels, received {len(pixels)}")
    image = Image.frombytes("L", (40, 30), pixels)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    image.save(args.output)
    print(f"Frame saved: {args.output.resolve()}")
    print("Dimensions: 40x30 grayscale")
    print(f"Pixel min/max: {min(pixels)}/{max(pixels)}")
    print(f"Pixel mean: {sum(pixels)/len(pixels):.2f}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
