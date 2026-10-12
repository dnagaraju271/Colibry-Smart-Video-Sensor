import json
from collections import Counter
from pathlib import Path

annotation_file = Path(
    "dataset/coco2017_annotations/annotations/instances_train2017.json"
)

print("Loading COCO annotations...")
with annotation_file.open("r", encoding="utf-8") as f:
    data = json.load(f)

images = data["images"]
annotations = data["annotations"]

# COCO category ID 1 represents person.
person_counts = Counter(
    ann["image_id"]
    for ann in annotations
    if ann["category_id"] == 1
)

total_images = len(images)
positive_images = sum(
    1 for image in images if person_counts[image["id"]] > 0
)
negative_candidates = total_images - positive_images
total_person_boxes = sum(person_counts.values())

print("\nCOCO 2017 Training Annotation Audit")
print("-" * 38)
print(f"Total images listed: {total_images:,}")
print(f"Images with annotated people: {positive_images:,}")
print(f"Images without annotated people: {negative_candidates:,}")
print(f"Total person bounding boxes: {total_person_boxes:,}")

print("\nSample image records:")
for image in images[:3]:
    print(
        f"ID: {image['id']}, "
        f"file: {image['file_name']}, "
        f"size: {image['width']}x{image['height']}, "
        f"person boxes: {person_counts[image['id']]}"
    )

print("\nAudit completed.")
print(
    "Note: Images without person annotations are only candidate "
    "negative images; they may still contain unannotated people."
)
