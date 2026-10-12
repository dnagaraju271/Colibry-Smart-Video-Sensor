
from pathlib import Path
import cv2

IMAGE_DIR = Path("dataset/processed/coco128/images/train")
LABEL_DIR = Path("dataset/processed/coco128/labels/train")
OUTPUT_DIR = Path("tests/label_preview")

OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

images = sorted(IMAGE_DIR.glob("*.png"))
shown = 0

for image_path in images:
    label_path = LABEL_DIR / f"{image_path.stem}.txt"

    if not label_path.exists():
        continue

    # Select only images with person annotations.
    label_lines = [
        line for line in label_path.read_text().splitlines()
        if line.strip() and line.split()[0] == "0"
    ]

    if not label_lines:
        continue

    image = cv2.imread(str(image_path), cv2.IMREAD_GRAYSCALE)

    if image is None:
        continue

    height, width = image.shape
    preview = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)

    for line in label_lines:
        values = line.split()

        if len(values) != 5:
            continue

        _, x, y, w, h = values
        x, y, w, h = map(float, (x, y, w, h))

        x1 = round((x - w / 2) * width)
        y1 = round((y - h / 2) * height)
        x2 = round((x + w / 2) * width)
        y2 = round((y + h / 2) * height)

        cv2.rectangle(
            preview, (x1, y1), (x2, y2), (0, 255, 0), 1
        )

        cv2.putText(
            preview, "person",
            (x1, max(y1 - 3, 10)),
            cv2.FONT_HERSHEY_SIMPLEX,
            0.35, (0, 255, 0), 1
        )

    cv2.imwrite(str(OUTPUT_DIR / image_path.name), preview)
    shown += 1

    if shown >= 12:
        break

print("Person preview images created:", shown)
print("Preview folder:", OUTPUT_DIR)

