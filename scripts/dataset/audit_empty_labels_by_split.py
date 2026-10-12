import csv
import json
from pathlib import Path

root = Path("dataset/coco_person_1000/processed")
ann_dir = Path("dataset/coco2017_annotations/annotations")
out = Path("tests/empty_label_audit")
out.mkdir(parents=True, exist_ok=True)

rows = []

for split in ("train", "val"):
    json_path = ann_dir / f"instances_{split}2017.json"
    print(f"\nLoading {json_path} ...")

    with json_path.open() as f:
        coco = json.load(f)

    image_info = {
        str(img["id"]).zfill(12): img
        for img in coco["images"]
    }

    person_counts = {}
    for ann in coco["annotations"]:
        if ann["category_id"] == 1:
            image_id = str(ann["image_id"]).zfill(12)
            person_counts[image_id] = person_counts.get(image_id, 0) + 1

    label_dir = root / "labels" / split
    image_dir = root / "images" / split

    for label in sorted(label_dir.glob("*.txt")):
        if label.read_text().strip():
            continue

        image_id = label.stem
        info = image_info.get(image_id)
        count = person_counts.get(image_id, 0) if info else None
        processed = image_dir / f"{image_id}.png"
        raw = (
            root / "raw_downloads" / info["file_name"]
            if info else None
        )

        if info is None:
            status = "NOT_FOUND_IN_CORRECT_SPLIT_JSON"
        elif count > 0:
            status = "MISMATCH_PERSON_ANNOTATIONS_EXIST"
        else:
            status = "ZERO_PERSON_ANNOTATIONS_VISUAL_REVIEW_REQUIRED"

        rows.append({
            "split": split,
            "image_id": image_id,
            "person_boxes_in_correct_split_json": count,
            "status": status,
            "processed_image_exists": processed.exists(),
            "raw_image_exists": bool(raw and raw.exists()),
            "decision": "PENDING",
        })

csv_path = out / "corrected_empty_label_audit.csv"
with csv_path.open("w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=rows[0].keys())
    writer.writeheader()
    writer.writerows(rows)

print("\n--- AUDIT SUMMARY ---")
print("Empty-label images:", len(rows))

for status in sorted({r["status"] for r in rows}):
    matching = [r for r in rows if r["status"] == status]
    print(f"\n{status}: {len(matching)}")
    for r in matching:
        if status != "ZERO_PERSON_ANNOTATIONS_VISUAL_REVIEW_REQUIRED":
            print(" ", r["split"], r["image_id"],
                  "boxes =", r["person_boxes_in_correct_split_json"])

print("\nCorrected report:", csv_path)
print("No labels were modified.")
