#!/usr/bin/env python3
"""Generate a C header embedding the verified own-count NNPA bitstream."""
from __future__ import annotations
import argparse
import hashlib
from pathlib import Path

EXPECTED_SIZE = 19392
EXPECTED_SHA256 = "ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e"

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nnpa", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    nnpa_path = args.nnpa.expanduser().resolve()
    output_path = args.output.expanduser().resolve()
    blob = nnpa_path.read_bytes()
    digest = hashlib.sha256(blob).hexdigest()
    if len(blob) != EXPECTED_SIZE or digest != EXPECTED_SHA256:
        raise SystemExit(
            f"ERROR: NNPA artifact mismatch\nsize={len(blob)}\nsha256={digest}"
        )
    if len(blob) % 4:
        raise SystemExit("ERROR: NNPA size is not divisible by 4")

    words = [
        int.from_bytes(blob[i:i + 4], "little")
        for i in range(0, len(blob), 4)
    ]
    output_path.parent.mkdir(parents=True, exist_ok=True)
    lines = [
        "/* Generated file. Do not edit manually. */",
        "#pragma once",
        "#include <stdint.h>",
        f"#define OWN_COUNT_NNPA_SIZE {len(blob)}u",
        f"#define OWN_COUNT_NNPA_WORD_COUNT {len(words)}u",
        f'#define OWN_COUNT_NNPA_SHA256 "{digest}"',
        "static const uint32_t own_count_nnpa_words[OWN_COUNT_NNPA_WORD_COUNT] = {",
    ]
    for i in range(0, len(words), 6):
        chunk = words[i:i + 6]
        lines.append("    " + ", ".join(f"0x{x:08X}u" for x in chunk) + ",")
    lines.extend(["};", ""])
    output_path.write_text("\n".join(lines), encoding="utf-8")
    print("NNPA header generated.")
    print(f"Size: {len(blob)} bytes")
    print(f"SHA-256: {digest}")
    print(f"Header: {output_path}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
