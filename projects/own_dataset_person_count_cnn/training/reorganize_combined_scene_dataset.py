#!/usr/bin/env python3
"""Combine the existing 71-image archive and additional 111-image archive.

Output filenames use I{count}_{scene:03d}_{frame:03d}.png, for example
2_person/I2_001_003.png. Scene IDs derive from WhatsApp source-group timestamps.
Frame IDs restart at 001 within a scene and follow manifest row order.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import io
import re
import zipfile
from collections import Counter, defaultdict
from datetime import datetime
from pathlib import Path, PurePosixPath

from PIL import Image

SIZE = (160, 120)
GROUP_RE = re.compile(r"(WhatsApp Image \d{4}-\d{2}-\d{2} at \d{2}\.\d{2}\.\d{2})")


def source_group(filename: str, explicit: str = "") -> str:
    if explicit.strip():
        return explicit.strip()
    match = GROUP_RE.search(filename)
    return match.group(1) if match else f"UNGROUPED::{filename}"


def group_order(key: str):
    match = GROUP_RE.search(key)
    if not match:
        return (1, key.casefold())
    try:
        dt = datetime.strptime(match.group(1).replace("WhatsApp Image ", ""),
                               "%Y-%m-%d at %H.%M.%S")
        return (0, dt.strftime("%Y%m%d%H%M%S"))
    except ValueError:
        return (1, key.casefold())


def load_frames(path: Path, archive_tag: str):
    if not path.is_file():
        raise FileNotFoundError(path)
    frames = []
    with zipfile.ZipFile(path) as src:
        names = set(src.namelist())
        manifests = [n for n in names if PurePosixPath(n).name.lower() == "manifest.csv"]
        if len(manifests) != 1:
            raise ValueError(f"{path.name}: expected exactly one manifest.csv")
        with src.open(manifests[0]) as raw:
            reader = csv.DictReader(io.TextIOWrapper(raw, encoding="utf-8-sig", newline=""))
            required = {"source_filename", "visible_person_count", "output_file"}
            if not reader.fieldnames or not required.issubset(reader.fieldnames):
                raise ValueError(f"{path.name}: manifest needs {sorted(required)}")
            for row_no, row in enumerate(reader, start=2):
                filename = (row.get("source_filename") or "").strip()
                member = (row.get("output_file") or "").strip()
                if not filename or member not in names:
                    raise FileNotFoundError(f"{path.name}: invalid/missing image at manifest row {row_no}")
                count = int(row["visible_person_count"])
                if count < 0:
                    raise ValueError(f"Negative people count: {filename}")
                raw_image = src.read(member)
                with Image.open(io.BytesIO(raw_image)) as image:
                    if image.size != SIZE:
                        raise ValueError(
                            f"{path.name}:{member}: expected 160x120, got {image.width}x{image.height}; "
                            "refusing to stretch or silently crop"
                        )
                    image = image.convert("L")
                    buf = io.BytesIO()
                    image.save(buf, "PNG", optimize=True)
                frames.append({
                    "source_archive": archive_tag,
                    "source_filename": filename,
                    "source_member": member,
                    "source_group": source_group(filename, row.get("source_group") or ""),
                    "count": count,
                    "data": buf.getvalue(),
                })
    return frames


def direction_fields(has_additional: bool):
    if has_additional:
        return {
            "entry_side": "LEFT_AND_RIGHT",
            "exit_side": "LEFT_AND_RIGHT",
            "movement_direction": "BIDIRECTIONAL_OBSERVED_AT_COLLECTION_LEVEL",
            "direction_status": "COARSE_VISUAL_REVIEW_NOT_PERSON_TRACKED",
            "direction_notes": (
                "Across the additional outdoor stills, people enter and leave the camera views "
                "from both left and right. This is a collection-level label, not a per-person track; "
                "persistent identities and exact individual paths are not annotated."
            ),
        }
    return {
        "entry_side": "UNKNOWN",
        "exit_side": "UNKNOWN",
        "movement_direction": "NOT_ANNOTATED",
        "direction_status": "NEEDS_SCENE_REVIEW",
        "direction_notes": "The legacy archive has no verified person-track entry/exit annotations.",
    }


def build(args):
    frames = (
        load_frames(args.legacy_archive, "legacy_71")
        + load_frames(args.additional_archive, "additional_111")
    )
    if len(frames) != 182:
        raise ValueError(f"Expected 182 frames (71 + 111), found {len(frames)}")

    by_scene = defaultdict(list)
    for frame in frames:
        by_scene[frame["source_group"]].append(frame)

    scene_keys = sorted(by_scene, key=group_order)
    scene_ids = {key: i + 1 for i, key in enumerate(scene_keys)}
    if args.output.exists() and not args.overwrite:
        raise FileExistsError(f"{args.output} exists; pass --overwrite to replace it")
    args.output.parent.mkdir(parents=True, exist_ok=True)

    manifest_fields = [
        "source_archive", "source_filename", "source_member", "visible_person_count",
        "class_folder", "scene_key", "scene_id", "frame_id", "output_file",
        "entry_side", "exit_side", "movement_direction", "direction_status",
        "direction_notes", "output_width", "output_height", "output_mode", "sha256",
    ]
    manifest_rows, direction_rows, summary_rows = [], [], []
    totals = Counter()

    with zipfile.ZipFile(args.output, "w", zipfile.ZIP_DEFLATED, compresslevel=9) as out:
        for key in scene_keys:
            scene_id = scene_ids[key]
            scene = by_scene[key]
            has_additional = any(f["source_archive"] == "additional_111" for f in scene)
            direction = direction_fields(has_additional)
            counts = Counter()

            # Manifest order is preserved as frame order within each source timestamp.
            for frame_id, frame in enumerate(scene, start=1):
                count = frame["count"]
                folder = f"{count}_person"
                output_file = f"{folder}/I{count}_{scene_id:03d}_{frame_id:03d}.png"
                out.writestr(output_file, frame["data"])
                row = {
                    "source_archive": frame["source_archive"],
                    "source_filename": frame["source_filename"],
                    "source_member": frame["source_member"],
                    "visible_person_count": count,
                    "class_folder": folder,
                    "scene_key": key,
                    "scene_id": f"{scene_id:03d}",
                    "frame_id": f"{frame_id:03d}",
                    "output_file": output_file,
                    **direction,
                    "output_width": 160,
                    "output_height": 120,
                    "output_mode": "L",
                    "sha256": hashlib.sha256(frame["data"]).hexdigest(),
                }
                manifest_rows.append(row)
                counts[count] += 1
                totals[count] += 1

            direction_rows.append({
                "scene_id": f"{scene_id:03d}",
                "scene_key": key,
                "frame_count": len(scene),
                **direction,
            })
            summary_rows.append({
                "scene_id": f"{scene_id:03d}",
                "scene_key": key,
                "frame_count": len(scene),
                **{f"{c}_person": counts[c] for c in range(5)},
            })

        def put_csv(name, fields, rows):
            stream = io.StringIO(newline="")
            writer = csv.DictWriter(stream, fieldnames=fields, extrasaction="ignore")
            writer.writeheader()
            writer.writerows(rows)
            out.writestr(name, stream.getvalue().encode("utf-8-sig"))

        put_csv("manifest.csv", manifest_fields, manifest_rows)
        put_csv("scene_directions.csv", list(direction_rows[0]), direction_rows)
        put_csv("scene_summary.csv", list(summary_rows[0]), summary_rows)

        readme = f"""# Combined scene/frame dataset

- Images: {len(frames)}
- Format: 160x120, 8-bit grayscale PNG (L)
- Scene groups: {len(scene_keys)}
- Count classes: 0, 1, 2, 3, and 4 people

## Naming

I{{count}}_{{scene:03d}}_{{frame:03d}}.png

Example: 2_person/I2_001_003.png = two visible people, scene 001, frame 003.
Frame numbering resets inside each scene.

Scene IDs come from source WhatsApp timestamp/source_group metadata. Frame
order follows each source manifest. The original archives are not modified.

## Class handling

Zero-person images are retained in 0_person; the current four-class trainer
skips them. Classes 1, 2, 3, and 4 are training-eligible.

## Entry/exit direction

Direction columns are present in manifest.csv and scene_directions.csv. The
additional outdoor stills show people entering and leaving the views from both
left and right sides across the collection, so their coarse scene label is
bidirectional. These stills do not establish persistent person IDs or exact
individual tracks. Legacy scenes are marked UNKNOWN pending review.

Files: manifest.csv, scene_directions.csv, and scene_summary.csv.
"""
        out.writestr("README.md", readme.encode("utf-8"))

    with zipfile.ZipFile(args.output) as check:
        bad = check.testzip()
        if bad:
            raise RuntimeError(f"ZIP integrity failure: {bad}")

    print("COMBINED SCENE DATASET CREATED")
    print(f"Output: {args.output}")
    print(f"Images: {len(frames)}")
    print(f"Scene groups: {len(scene_keys)}")
    for count in range(5):
        print(f"{count}_person: {totals[count]}")
    print("ZIP integrity: PASS")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--legacy-archive", type=Path, default=Path(
        "datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip"))
    parser.add_argument("--additional-archive", type=Path, default=Path(
        "datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_additional_111_4class.zip"))
    parser.add_argument("--output", type=Path, default=Path(
        "datasets/mobile_160x120/OWN_Dataset_182_Scene_Frames_160x120.zip"))
    parser.add_argument("--overwrite", action="store_true")
    build(parser.parse_args())


if __name__ == "__main__":
    main()
