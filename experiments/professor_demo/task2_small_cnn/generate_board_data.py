#!/usr/bin/env python3
"""Generate C arrays for the SRAM-only Colibry own-count CNN demo.

The model binary and input samples are embedded in the firmware image. This
allows the NNPA model to be copied into NNPA SRAM at runtime without writing
the external flash.
"""

from __future__ import annotations

import argparse
import hashlib
import io
from pathlib import Path
import zipfile

from PIL import Image

EXPECTED_NNPA_SIZE = 19392
EXPECTED_NNPA_SHA256 = (
    "ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e"
)

IMAGE_WIDTH = 40
IMAGE_HEIGHT = 30
INPUT_BYTES = IMAGE_WIDTH * IMAGE_HEIGHT

SAMPLES = (
    ("1_person", "1_person/I1__008__120x160.png"),
    ("2_person", "2_person/I2__007__120x160.png"),
    ("3_person", "3_person/I3__004__120x160.png"),
)


def c_u32_array(name: str, values: list[int], per_line: int = 6) -> str:
    lines = [f"static const uint32_t {name}[{len(values)}] = {{"]
    for offset in range(0, len(values), per_line):
        group = values[offset : offset + per_line]
        lines.append("    " + ", ".join(f"0x{value:08X}u" for value in group) + ",")
    lines.append("};")
    return "\n".join(lines)


def c_u8_2d_array(name: str, values: list[bytes]) -> str:
    lines = [
        f"static const uint8_t {name}[{len(values)}][{INPUT_BYTES}] = {{"
    ]
    for sample in values:
        lines.append("    {")
        for offset in range(0, len(sample), 16):
            group = sample[offset : offset + 16]
            lines.append("        " + ", ".join(f"0x{value:02X}" for value in group) + ",")
        lines.append("    },")
    lines.append("};")
    return "\n".join(lines)


def load_sample(zip_file: zipfile.ZipFile, member: str) -> bytes:
    try:
        raw = zip_file.read(member)
    except KeyError as exc:
        raise FileNotFoundError(f"Dataset archive member is missing: {member}") from exc

    with Image.open(io.BytesIO(raw)) as image:
        if image.size != (160, 120):
            raise ValueError(f"{member}: expected 160x120, got {image.size}")
        gray = image.convert("L").resize(
            (IMAGE_WIDTH, IMAGE_HEIGHT), Image.Resampling.LANCZOS
        )
        payload = gray.tobytes()

    if len(payload) != INPUT_BYTES:
        raise RuntimeError(f"{member}: expected {INPUT_BYTES} input bytes")
    return payload


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nnpa", type=Path, required=True)
    parser.add_argument("--archive", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    nnpa_path = args.nnpa.expanduser().resolve()
    archive_path = args.archive.expanduser().resolve()
    output_path = args.output.expanduser().resolve()

    if not nnpa_path.is_file():
        raise FileNotFoundError(f"NNPA model not found: {nnpa_path}")
    if not archive_path.is_file():
        raise FileNotFoundError(f"Dataset archive not found: {archive_path}")

    model = nnpa_path.read_bytes()
    model_hash = hashlib.sha256(model).hexdigest()
    if len(model) != EXPECTED_NNPA_SIZE:
        raise ValueError(
            f"Expected NNPA size {EXPECTED_NNPA_SIZE}, found {len(model)}"
        )
    if model_hash != EXPECTED_NNPA_SHA256:
        raise ValueError(
            "NNPA SHA-256 mismatch. Refusing to embed an unverified model: "
            f"{model_hash}"
        )
    if len(model) % 4:
        raise ValueError("NNPA model size must be divisible by 4")

    with zipfile.ZipFile(archive_path) as archive:
        members = set(archive.namelist())
        manifest_members = [
            item for item in members
            if Path(item).name.lower() == "manifest.csv"
        ]
        if len(manifest_members) != 1:
            raise ValueError(
                f"Expected exactly one manifest.csv, found {len(manifest_members)}"
            )

        inputs: list[bytes] = []
        for expected_class, member in SAMPLES:
            if member.split("/", 1)[0] != expected_class:
                raise RuntimeError(f"Incorrect sample class mapping: {member}")
            inputs.append(load_sample(archive, member))

    model_words = [
        int.from_bytes(model[offset : offset + 4], "little")
        for offset in range(0, len(model), 4)
    ]

    output_path.parent.mkdir(parents=True, exist_ok=True)
    sections = [
        "/* Generated file. Do not edit manually. */",
        "#pragma once",
        "#include <stdint.h>",
        f"#define OWN_COUNT_NNPA_SIZE {len(model)}u",
        f"#define OWN_COUNT_NNPA_WORD_COUNT {len(model_words)}u",
        f"#define OWN_COUNT_INPUT_BYTES {INPUT_BYTES}u",
        f"#define OWN_COUNT_SAMPLE_COUNT {len(SAMPLES)}u",
        "",
        c_u32_array("own_count_nnpa_words", model_words),
        "",
        c_u8_2d_array("own_count_test_inputs", inputs),
        "",
        "static const char *const own_count_sample_names[OWN_COUNT_SAMPLE_COUNT] = {",
        *(f'    "{member}",' for _, member in SAMPLES),
        "};",
        "",
        "static const char *const own_count_expected_classes[OWN_COUNT_SAMPLE_COUNT] = {",
        *(f'    "{class_name}",' for class_name, _ in SAMPLES),
        "};",
        "",
    ]
    output_path.write_text("\n".join(sections), encoding="utf-8")

    print("===== GENERATED SRAM DEMO DATA =====")
    print(f"NNPA file:       {nnpa_path}")
    print(f"NNPA size:       {len(model)} bytes")
    print(f"NNPA SHA-256:    {model_hash}")
    print(f"Input shape:     [{IMAGE_HEIGHT}, {IMAGE_WIDTH}, 1]")
    print(f"Input bytes:     {INPUT_BYTES} per sample")
    print(f"Samples:         {len(SAMPLES)}")
    for class_name, member in SAMPLES:
        print(f"  {class_name}: {member}")
    print(f"Generated file:  {output_path}")
    print("External flash:  not accessed or modified")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
