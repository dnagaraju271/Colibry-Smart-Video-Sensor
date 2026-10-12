from pathlib import Path
import cv2

SOURCE = Path("dataset/coco128_demo/coco128")
IMAGE_DIR = SOURCE / "images/train2017"
LABEL_DIR = SOURCE / "labels/train2017"

TARGET_W = 160
TARGET_H = 128

heights_resized = []
missing_labels = 0

for image_path in sorted(IMAGE_DIR.glob("*.jpg")):
    label_path = LABEL_DIR / f"{image_path.stem}.txt"

    if not label_path.exists():
        missing_labels += 1
        continue

    image = cv2.imread(str(image_path))

    if image is None:
        continue

    original_h, original_w = image.shape[:2]
    scale = min(TARGET_W / original_w, TARGET_H / original_h)

    for line in label_path.read_text().splitlines():
        values = line.split()

        if len(values) != 5 or values[0] != "0":
            continue

        box_h = float(values[4]) * original_h
        resized_h = box_h * scale
        heights_resized.append(resized_h)

if not heights_resized:
    print("No person annotations found.")
    raise SystemExit(1)

heights_resized.sort()
n = len(heights_resized)

print("Person boxes analyzed:", n)
print("Images missing annotation files:", missing_labels)
print("Median person height after resizing:",
      round(heights_resized[n // 2], 2), "pixels")

for limit in (4, 8, 12, 20):
    count = sum(h < limit for h in heights_resized)
    print(f"Person boxes shorter than {limit} pixels: {count} ({count/n:.1%})")
