import csv
import json
import time
import urllib.error
import urllib.request
from pathlib import Path
from urllib.parse import urlparse

import cv2
import numpy as np

MANIFEST = Path("dataset/coco_person_1000/manifest.csv")
ANNOTATIONS = Path(
    "dataset/coco2017_annotations/annotations/instances_train2017.json"
)
OUTPUT = Path("dataset/coco_person_1000/processed")
WIDTH, HEIGHT = 160, 128
RETRIES = 3

# Load selected images.
with MANIFEST.open(newline="", encoding="utf-8") as f:
    manifest_rows = list(csv.DictReader(f))

# Map each selected COCO image ID to its person annotations.
print("Loading person annotations...")
with ANNOTATIONS.open(encoding="utf-8") as f:
    coco = json.load(f)

image_info = {int(row["image_id"]): row for row in manifest_rows}
boxes_by_image = {image_id: [] for image_id in image_info}

for ann in coco["annotations"]:
    image_id = ann["image_id"]
    if image_id in boxes_by_image and ann["category_id"] == 1:
        boxes_by_image[image_id].append(ann["bbox"])

def download_image(url, destination):
    # COCO metadata commonly uses HTTP; try HTTPS first.
    urls = [url]
    parsed = urlparse(url)
    if parsed.scheme == "http":
        urls.insert(0, parsed._replace(scheme="https").geturl())

    last_error = None
    for candidate_url in urls:
        for attempt in range(RETRIES):
            try:
                request = urllib.request.Request(
                    candidate_url,
                    headers={"User-Agent": "ColibryDatasetPreparation/1.0"},
                )
                with urllib.request.urlopen(request, timeout=30) as response:
                    data = response.read()

                if not data:
                    raise ValueError("Empty download")

                destination.write_bytes(data)
                image = cv2.imread(str(destination), cv2.IMREAD_COLOR)
                if image is None:
                    destination.unlink(missing_ok=True)
                    raise ValueError("Downloaded file is not a readable image")

                return image
            except Exception as exc:
                last_error = exc
                time.sleep(attempt + 1)

    raise RuntimeError(f"Could not download {url}: {last_error}")

def letterbox_gray(image):
    original_h, original_w = image.shape[:2]
    scale = min(WIDTH / original_w, HEIGHT / original_h)
    new_w = max(1, round(original_w * scale))
    new_h = max(1, round(original_h * scale))

    resized = cv2.resize(image, (new_w, new_h), interpolation=cv2.INTER_AREA)
    gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)

    left = (WIDTH - new_w) // 2
    top = (HEIGHT - new_h) // 2

    canvas = np.zeros((HEIGHT, WIDTH), dtype=np.uint8)
    canvas[top:top + new_h, left:left + new_w] = gray

    return canvas, scale, left, top, original_w, original_h

def transform_box(box, scale, left, top, original_w, original_h):
    x, y, w, h = box

    # Clip the original COCO box to the source image.
    x1 = max(0.0, min(float(original_w), x))
    y1 = max(0.0, min(float(original_h), y))
    x2 = max(0.0, min(float(original_w), x + w))
    y2 = max(0.0, min(float(original_h), y + h))

    if x2 <= x1 or y2 <= y1:
        return None

    x1 = x1 * scale + left
    x2 = x2 * scale + left
    y1 = y1 * scale + top
    y2 = y2 * scale + top

    cx = ((x1 + x2) / 2) / WIDTH
    cy = ((y1 + y2) / 2) / HEIGHT
    bw = (x2 - x1) / WIDTH
    bh = (y2 - y1) / HEIGHT

    if bw <= 0 or bh <= 0:
        return None

    return cx, cy, bw, bh

OUTPUT.mkdir(parents=True, exist_ok=True)
raw_dir = OUTPUT / "raw_downloads"
raw_dir.mkdir(parents=True, exist_ok=True)

failures = []
processed = 0
retained_boxes = 0

for index, row in enumerate(manifest_rows, start=1):
    image_id = int(row["image_id"])
    split = row["split"]
    filename = row["file_name"]

    image_dir = OUTPUT / "images" / split
    label_dir = OUTPUT / "labels" / split
    image_dir.mkdir(parents=True, exist_ok=True)
    label_dir.mkdir(parents=True, exist_ok=True)

    raw_path = raw_dir / filename

    try:
        url = row["coco_url"].strip()
        if not url:
            raise ValueError("Manifest has no COCO image URL")

        if raw_path.exists():
            image = cv2.imread(str(raw_path), cv2.IMREAD_COLOR)
            if image is None:
                raw_path.unlink()
                image = download_image(url, raw_path)
        else:
            image = download_image(url, raw_path)

        gray, scale, left, top, ow, oh = letterbox_gray(image)

        output_image = image_dir / f"{Path(filename).stem}.png"
        if not cv2.imwrite(str(output_image), gray):
            raise RuntimeError("Could not save processed image")

        lines = []
        for box in boxes_by_image[image_id]:
            transformed = transform_box(box, scale, left, top, ow, oh)
            if transformed is not None:
                cx, cy, bw, bh = transformed
                lines.append(
                    f"0 {cx:.6f} {cy:.6f} {bw:.6f} {bh:.6f}"
                )

        label_path = label_dir / f"{Path(filename).stem}.txt"
        label_path.write_text(
            "\n".join(lines) + ("\n" if lines else ""),
            encoding="utf-8",
        )

        processed += 1
        retained_boxes += len(lines)

    except Exception as exc:
        failures.append({
            "image_id": image_id,
            "file_name": filename,
            "split": split,
            "error": str(exc),
        })

    if index % 25 == 0 or index == len(manifest_rows):
        print(f"Progress: {index}/{len(manifest_rows)}")

failure_file = OUTPUT / "failures.csv"
with failure_file.open("w", newline="", encoding="utf-8") as f:
    writer = csv.DictWriter(
        f, fieldnames=["image_id", "file_name", "split", "error"]
    )
    writer.writeheader()
    writer.writerows(failures)

print("\nPreprocessing summary")
print(f"Selected: {len(manifest_rows)}")
print(f"Processed successfully: {processed}")
print(f"Failed: {len(failures)}")
print(f"Person boxes retained: {retained_boxes}")
print(f"Output: {OUTPUT}")
print(f"Failure log: {failure_file}")
print("Review candidate negative images before using them for training.")
