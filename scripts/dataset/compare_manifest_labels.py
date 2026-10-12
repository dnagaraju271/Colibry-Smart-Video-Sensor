import csv
from pathlib import Path

base = Path("dataset/coco_person_1000")
processed = base / "processed"
manifest_path = base / "manifest.csv"
output = Path("tests/empty_label_audit/manifest_label_mismatches.csv")
output.parent.mkdir(parents=True, exist_ok=True)

with manifest_path.open(newline="") as f:
    manifest = list(csv.DictReader(f))

rows = []

for item in manifest:
    image_id = str(item["image_id"]).zfill(12)
    split = item["split"]
    expected = int(item["person_box_count"])

    label = processed / "labels" / split / f"{image_id}.txt"
    image = processed / "images" / split / f"{image_id}.png"

    if not label.exists() or not image.exists():
        rows.append({
            "image_id": image_id,
            "split": split,
            "original_person_boxes": expected,
            "current_label_boxes": None,
            "issue": "MISSING_IMAGE_OR_LABEL",
        })
        continue

    content = label.read_text().strip()
    actual = len(content.splitlines()) if content else 0

    if actual != expected:
        rows.append({
            "image_id": image_id,
            "split": split,
            "original_person_boxes": expected,
            "current_label_boxes": actual,
            "issue": "BOX_COUNT_MISMATCH",
        })

with output.open("w", newline="") as f:
    writer = csv.DictWriter(
        f,
        fieldnames=[
            "image_id", "split", "original_person_boxes",
            "current_label_boxes", "issue"
        ],
    )
    writer.writeheader()
    writer.writerows(rows)

print("Manifest entries:", len(manifest))
print("Mismatches:", len(rows))
for row in rows:
    print(row)
print("\nReport saved to:", output)
