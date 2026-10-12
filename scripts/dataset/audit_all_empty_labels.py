import csv
import json
from pathlib import Path
from PIL import Image, ImageDraw
import math

root = Path("dataset/coco_person_1000/processed")
ann_path = Path(
    "dataset/coco2017_annotations/annotations/instances_train2017.json"
)
out_dir = Path("tests/empty_label_audit")
out_dir.mkdir(parents=True, exist_ok=True)

print("Loading COCO annotations...")
with ann_path.open() as f:
    coco = json.load(f)

images_by_id = {
    str(x["id"]).zfill(12): x
    for x in coco["images"]
}
person_counts = {}

for ann in coco["annotations"]:
    if ann["category_id"] == 1:
        image_id = str(ann["image_id"]).zfill(12)
        person_counts[image_id] = person_counts.get(image_id, 0) + 1

items = []
report = []

for split in ("train", "val"):
    label_dir = root / "labels" / split
    image_dir = root / "images" / split

    for label in sorted(label_dir.glob("*.txt")):
        if label.read_text().strip():
            continue

        image_id = label.stem
        processed_image = image_dir / f"{image_id}.png"
        coco_info = images_by_id.get(image_id)
        raw_image = (
            root / "raw_downloads" / coco_info["file_name"]
            if coco_info else None
        )

        count = person_counts.get(image_id, 0) if coco_info else None
        status = (
            "COCO_PERSON_ANNOTATIONS_EXIST"
            if count and count > 0
            else "COCO_ZERO_PERSON_ANNOTATIONS"
            if coco_info
            else "NOT_FOUND_IN_TRAIN_ANNOTATIONS"
        )

        report.append({
            "split": split,
            "image_id": image_id,
            "coco_person_boxes": count,
            "annotation_status": status,
            "processed_image_exists": processed_image.exists(),
            "raw_image_exists": bool(raw_image and raw_image.exists()),
            "review_decision": "PENDING_VISUAL_REVIEW",
        })

        if processed_image.exists():
            items.append((split, image_id, processed_image, status))

csv_path = out_dir / "empty_label_audit.csv"
with csv_path.open("w", newline="") as f:
    writer = csv.DictWriter(f, fieldnames=report[0].keys())
    writer.writeheader()
    writer.writerows(report)

# Create contact sheets with 40 images per page for readable thumbnails.
per_page = 40
cols = 4
cell_w, cell_h = 320, 230

for page_start in range(0, len(items), per_page):
    page_items = items[page_start:page_start + per_page]
    rows = math.ceil(len(page_items) / cols)
    sheet = Image.new("RGB", (cols * cell_w, rows * cell_h), "white")
    draw = ImageDraw.Draw(sheet)

    for i, (split, image_id, path, status) in enumerate(page_items):
        img = Image.open(path).convert("RGB")
        img.thumbnail((cell_w - 12, cell_h - 48))
        x = (i % cols) * cell_w
        y = (i // cols) * cell_h
        sheet.paste(img, (x + (cell_w - img.width)//2, y + 4))
        draw.text((x + 6, y + cell_h - 42),
                  f"{split} {image_id}", fill="black")
        draw.text((x + 6, y + cell_h - 24),
                  status.replace("COCO_", "")[:42], fill="black")

    page_num = page_start // per_page + 1
    path = out_dir / f"contact_sheet_{page_num:02d}.jpg"
    sheet.save(path, quality=92)
    print("Created:", path)

print("\nEmpty labels audited:", len(report))
print("Images available for visual review:", len(items))
print("CSV report:", csv_path)
print("COCO annotations are evidence, not proof that an image contains no people.")
