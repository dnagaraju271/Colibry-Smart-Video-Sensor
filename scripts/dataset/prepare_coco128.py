
from pathlib import Path
import cv2
import numpy as np

SOURCE = Path("dataset/coco128_demo/coco128")
OUTPUT = Path("dataset/processed/coco128")

TARGET_W = 160
TARGET_H = 128


def preprocess_image(image):
    height, width = image.shape[:2]

    scale = min(TARGET_W / width, TARGET_H / height)
    new_w = round(width * scale)
    new_h = round(height * scale)

    resized = cv2.resize(
        image, (new_w, new_h),
        interpolation=cv2.INTER_AREA
    )
    gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)

    canvas = np.zeros((TARGET_H, TARGET_W), dtype=np.uint8)
    left = (TARGET_W - new_w) // 2
    top = (TARGET_H - new_h) // 2

    canvas[top:top + new_h, left:left + new_w] = gray
    return canvas, scale, left, top


def process_dataset():
    image_dir = SOURCE / "images" / "train2017"
    label_dir = SOURCE / "labels" / "train2017"

    output_images = OUTPUT / "images" / "train"
    output_labels = OUTPUT / "labels" / "train"

    if not image_dir.exists() or not label_dir.exists():
        raise FileNotFoundError(
            f"Check the source folders: {image_dir} and {label_dir}"
        )

    output_images.mkdir(parents=True, exist_ok=True)
    output_labels.mkdir(parents=True, exist_ok=True)

    image_paths = sorted(image_dir.glob("*.jpg"))

    if not image_paths:
        raise FileNotFoundError(f"No JPG images in {image_dir}")

    processed = 0
    person_boxes = 0
    images_with_people = 0
    missing_labels = 0

    for image_path in image_paths:
        label_path = label_dir / f"{image_path.stem}.txt"

        image = cv2.imread(str(image_path))
        if image is None:
            raise RuntimeError(f"Cannot read image: {image_path}")

        height, width = image.shape[:2]
        boxes = []

        # Do not silently assume a missing label means no people.
        if not label_path.exists():
            missing_labels += 1
            print(f"WARNING: Missing label, skipping image: {image_path.name}")
            continue

        for line in label_path.read_text().splitlines():
            values = line.split()

            if len(values) != 5:
                raise ValueError(f"Invalid label in {label_path}: {line}")

            class_id = int(values[0])
            if class_id != 0:
                continue

            x, y, w, h = map(float, values[1:])

            boxes.append((
                x * width,
                y * height,
                w * width,
                h * height
            ))

        small, scale, left, top = preprocess_image(image)
        new_labels = []

        for cx, cy, bw, bh in boxes:
            cx = (cx * scale + left) / TARGET_W
            cy = (cy * scale + top) / TARGET_H
            bw = (bw * scale) / TARGET_W
            bh = (bh * scale) / TARGET_H

            if not (
                0 <= cx <= 1 and 0 <= cy <= 1
                and 0 < bw <= 1 and 0 < bh <= 1
            ):
                raise ValueError(f"Invalid bounding box: {image_path.name}")

            new_labels.append(
                f"0 {cx:.6f} {cy:.6f} {bw:.6f} {bh:.6f}"
            )

        image_out = output_images / f"{image_path.stem}.png"
        label_out = output_labels / f"{image_path.stem}.txt"

        if not cv2.imwrite(str(image_out), small):
            raise RuntimeError(f"Cannot save image: {image_out}")

        label_out.write_text(
            "\n".join(new_labels) + ("\n" if new_labels else "")
        )

        processed += 1
        person_boxes += len(boxes)

        if boxes:
            images_with_people += 1

    print("\n--- Dataset summary ---")
    print("Images processed:", processed)
    print("Images skipped due to missing labels:", missing_labels)
    print("Images with person boxes:", images_with_people)
    print("Person boxes retained:", person_boxes)
    print("Resolution: 160 x 128")
    print("Format: grayscale PNG")
    print("Output:", OUTPUT)


if __name__ == "__main__":
    process_dataset()

