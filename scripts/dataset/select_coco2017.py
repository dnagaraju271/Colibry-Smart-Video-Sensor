import csv
import json
import random
from collections import defaultdict
from pathlib import Path

SOURCE = Path(
    "dataset/coco2017_annotations/annotations/instances_train2017.json"
)
OUTPUT = Path("dataset/coco_person_1000")
SEED = 42

POSITIVE_COUNT = 800
NEGATIVE_COUNT = 200
VALIDATION_FRACTION = 0.20

print("Loading COCO annotations...")
with SOURCE.open("r", encoding="utf-8") as f:
    data = json.load(f)

images = data["images"]
person_boxes = defaultdict(int)

for ann in data["annotations"]:
    if ann["category_id"] == 1:
        person_boxes[ann["image_id"]] += 1

positive = [img for img in images if person_boxes[img["id"]] > 0]
negative = [img for img in images if person_boxes[img["id"]] == 0]

rng = random.Random(SEED)
rng.shuffle(positive)
rng.shuffle(negative)

if len(positive) < POSITIVE_COUNT or len(negative) < NEGATIVE_COUNT:
    raise RuntimeError("Not enough images for the requested selection.")

selected = (
    [(img, "positive") for img in positive[:POSITIVE_COUNT]]
    + [(img, "negative_candidate") for img in negative[:NEGATIVE_COUNT]]
)

# Shuffle and split within each category to preserve class proportions.
manifest_rows = []

for category in ("positive", "negative_candidate"):
    group = [(img, label) for img, label in selected if label == category]
    rng.shuffle(group)

    val_count = round(len(group) * VALIDATION_FRACTION)
    validation = group[:val_count]
    training = group[val_count:]

    for split, items in (("val", validation), ("train", training)):
        for img, label in items:
            manifest_rows.append({
                "image_id": img["id"],
                "file_name": img["file_name"],
                "coco_url": img.get("coco_url", ""),
                "width": img["width"],
                "height": img["height"],
                "category": label,
                "split": split,
                "person_box_count": person_boxes[img["id"]],
            })

OUTPUT.mkdir(parents=True, exist_ok=True)
manifest = OUTPUT / "manifest.csv"

with manifest.open("w", newline="", encoding="utf-8") as f:
    writer = csv.DictWriter(f, fieldnames=manifest_rows[0].keys())
    writer.writeheader()
    writer.writerows(manifest_rows)

print("\nSelection complete.")
print(f"Manifest: {manifest}")
print(f"Total selected: {len(manifest_rows)}")

for split in ("train", "val"):
    rows = [r for r in manifest_rows if r["split"] == split]
    positives = sum(r["category"] == "positive" for r in rows)
    negatives = sum(r["category"] == "negative_candidate" for r in rows)
    print(f"{split}: {len(rows)} images "
          f"({positives} positive, {negatives} negative candidates)")

print("\nNo images downloaded yet.")
