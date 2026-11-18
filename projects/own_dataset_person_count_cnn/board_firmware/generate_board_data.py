#!/usr/bin/env python3
"""Generate one bounded SRAM batch from the own-dataset CNN ZIP manifest."""
from __future__ import annotations
import argparse, csv, hashlib, io, zipfile
from pathlib import Path, PurePosixPath
from PIL import Image

EXPECTED_NNPA_SIZE = 19392
EXPECTED_NNPA_SHA256 = "ee9940c9edb9c66a75b4c005b9eb264a1732bf3428404c958a6f864cc156080e"
IMAGE_WIDTH, IMAGE_HEIGHT = 40, 30
INPUT_BYTES = IMAGE_WIDTH * IMAGE_HEIGHT
CLASS_NAMES = ("1_person", "2_person", "3_person")
CLASS_TO_ID = {name: i for i, name in enumerate(CLASS_NAMES)}
MAX_BATCH_SIZE = 16

def c_u32_array(name: str, values: list[int], per_line: int = 6) -> str:
    lines = [f"static const uint32_t {name}[{len(values)}] = {{"]
    for offset in range(0, len(values), per_line):
        group = values[offset:offset + per_line]
        lines.append("    " + ", ".join(f"0x{v:08X}u" for v in group) + ",")
    lines.append("};")
    return "\n".join(lines)

def c_u8_2d_array(name: str, values: list[bytes]) -> str:
    lines = [f"static const uint8_t {name}[{len(values)}][{INPUT_BYTES}] = {{"]
    for sample in values:
        lines.append("    {")
        for offset in range(0, len(sample), 16):
            group = sample[offset:offset + 16]
            lines.append("        " + ", ".join(f"0x{v:02X}" for v in group) + ",")
        lines.append("    },")
    lines.append("};")
    return "\n".join(lines)

def load_sample(archive: zipfile.ZipFile, member: str) -> bytes:
    raw = archive.read(member)
    with Image.open(io.BytesIO(raw)) as image:
        if image.size != (160, 120):
            raise ValueError(f"{member}: expected 160x120, got {image.size}")
        image = image.convert("L").resize((IMAGE_WIDTH, IMAGE_HEIGHT), Image.Resampling.LANCZOS)
        pixels = image.tobytes()
    if len(pixels) != INPUT_BYTES:
        raise RuntimeError(f"{member}: expected {INPUT_BYTES} bytes")
    return pixels

def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--nnpa", type=Path, required=True)
    parser.add_argument("--archive", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--batch-start", type=int, default=0)
    parser.add_argument("--batch-size", type=int, default=16)
    args = parser.parse_args()
    if args.batch_start < 0:
        parser.error("--batch-start must be >= 0")
    if not 1 <= args.batch_size <= MAX_BATCH_SIZE:
        parser.error(f"--batch-size must be between 1 and {MAX_BATCH_SIZE}")

    nnpa_path = args.nnpa.expanduser().resolve()
    archive_path = args.archive.expanduser().resolve()
    output_path = args.output.expanduser().resolve()
    model = nnpa_path.read_bytes()
    model_hash = hashlib.sha256(model).hexdigest()
    if len(model) != EXPECTED_NNPA_SIZE or model_hash != EXPECTED_NNPA_SHA256:
        raise ValueError(f"Unverified NNPA artifact: size={len(model)} sha256={model_hash}")
    if not archive_path.is_file():
        raise FileNotFoundError(archive_path)

    with zipfile.ZipFile(archive_path) as archive:
        members = set(archive.namelist())
        manifests = [p for p in members if PurePosixPath(p).name.lower() == "manifest.csv"]
        if len(manifests) != 1:
            raise ValueError(f"Expected one manifest.csv, found {len(manifests)}")
        records: list[tuple[int, str, str]] = []
        seen: set[str] = set()
        with archive.open(manifests[0]) as raw:
            reader = csv.DictReader(io.TextIOWrapper(raw, encoding="utf-8-sig", newline=""))
            required = {"output_file", "visible_person_count"}
            if not reader.fieldnames or not required.issubset(reader.fieldnames):
                raise ValueError(f"Manifest must contain {sorted(required)}")
            for row in reader:
                member = (row.get("output_file") or "").strip()
                if not member:
                    raise ValueError("Manifest has an empty output_file")
                if member in seen:
                    raise ValueError(f"Duplicate manifest output: {member}")
                seen.add(member)
                class_name = PurePosixPath(member).parts[0]
                count = int(row["visible_person_count"])
                if class_name == "pending_for_later_originals":
                    if count != 0:
                        raise ValueError(f"{member}: pending original must have count 0")
                    continue
                if class_name not in CLASS_TO_ID:
                    raise ValueError(f"Unsupported class folder: {class_name}")
                if count != CLASS_TO_ID[class_name] + 1:
                    raise ValueError(f"{member}: manifest count conflicts with class folder")
                if member not in members:
                    raise FileNotFoundError(f"Manifest image missing in ZIP: {member}")
                records.append((len(records), member, class_name))
        if len(records) != 71:
            raise ValueError(f"Expected 71 labelled images, found {len(records)}")
        stop = min(args.batch_start + args.batch_size, len(records))
        if args.batch_start >= stop:
            raise ValueError(f"Batch start {args.batch_start} is outside 0..{len(records)-1}")
        selected = records[args.batch_start:stop]
        inputs = [load_sample(archive, member) for _, member, _ in selected]

    words = [int.from_bytes(model[i:i+4], "little") for i in range(0, len(model), 4)]
    output_path.parent.mkdir(parents=True, exist_ok=True)
    sections = [
        "/* Generated file. Do not edit manually. */",
        "#pragma once",
        "#include <stdint.h>",
        f"#define OWN_COUNT_NNPA_SIZE {len(model)}u",
        f"#define OWN_COUNT_NNPA_WORD_COUNT {len(words)}u",
        f"#define OWN_COUNT_INPUT_BYTES {INPUT_BYTES}u",
        f"#define OWN_COUNT_SAMPLE_COUNT {len(selected)}u",
        f"#define OWN_COUNT_BATCH_START {args.batch_start}u",
        f"#define OWN_COUNT_TOTAL_IMAGES {len(records)}u",
        "",
        c_u32_array("own_count_nnpa_words", words),
        "",
        c_u8_2d_array("own_count_test_inputs", inputs),
        "",
        "static const uint32_t own_count_dataset_indices[OWN_COUNT_SAMPLE_COUNT] = {",
        *(f"    {index}u," for index, _, _ in selected),
        "};",
        "",
        "static const uint8_t own_count_expected_class_ids[OWN_COUNT_SAMPLE_COUNT] = {",
        *(f"    {CLASS_TO_ID[name]}u," for _, _, name in selected),
        "};",
        "",
        "static const char *const own_count_sample_names[OWN_COUNT_SAMPLE_COUNT] = {",
        *(f'    "{member}",' for _, member, _ in selected),
        "};",
        "",
        "static const char *const own_count_expected_classes[OWN_COUNT_SAMPLE_COUNT] = {",
        *(f'    "{name}",' for _, _, name in selected),
        "};",
        "",
    ]
    output_path.write_text("\n".join(sections), encoding="utf-8")
    print("===== GENERATED SRAM DATASET BATCH =====")
    print(f"NNPA size:       {len(model)} bytes")
    print(f"NNPA SHA-256:    {model_hash}")
    print(f"Dataset images:  {len(records)}")
    print(f"Batch range:     {args.batch_start}..{stop-1}")
    print(f"Batch size:      {len(selected)}")
    print(f"Input shape:     [{IMAGE_HEIGHT}, {IMAGE_WIDTH}, 1]")
    print(f"Bytes per image: {INPUT_BYTES}")
    for index, member, name in selected:
        print(f"  {index:02d} {name}: {member}")
    print(f"Generated file:  {output_path}")
    print("External flash:  not accessed or modified")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
