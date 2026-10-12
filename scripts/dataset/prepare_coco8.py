from pathlib import Path
import cv2
import numpy as np

SOURCE = Path("dataset/coco8_demo/coco8")
OUTPUT = Path("dataset/processed/coco8")

TARGET_W = 160
TARGET_H = 128

SPLITS = ["train", "val"]


def letterbox_grayscale(image):
    """Convert to grayscale and resize without distorting proportions."""
    height, width = image.shape[:2]

    scale = min(TARGET_W / width, TARGET_H / height)

    new_w = round(width * scale)
    new_h = round(height * scale)

    resized = cv2.resize(
        image, (new_w, new_h),
        interpolation=cv2.INTER_AREA
    )

    gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)

    # Center the image on a black canvas.
    canvas = np.zeros((TARGET_H, TARGET_W), dtype=np.uint8)

    left = (TARGET_W - new_w) // 2
    top = (TARGET_H - new_h) // 2

    canvas[top:top + new_h, left:left + new_w] = gray

    return canvas, scale, left, top


def process_split(split):
    image_dir = SOURCE / "images" / split
    label_dir = SOURCE / "labels" / split

    out_images = OUTPUT / "images" / split
    out_labels = OUTPUT / "labels" / split

    out_images.mkdir(parents=True, exist_ok=True)
    out_labels.mkdir(parents=True, exist_ok=True)

    processed = 0
    people_found = 0

    for image_path in sorted(image_dir.glob("*")):
        image = cv2.imread(str(image_path))

        if image is None:
            print("Skipping unreadable image:", image_path)
            continue

        label_path = label_dir / f"{image_path.stem}.txt"

        if not label_path.exists():
            print("Missing label:", label_path)
            continue

        height, width = image.shape[:2]

        # Keep only COCO class 0: person.
        person_boxes = []

        for line in label_path.read_text().splitlines():
            values = line.split()

            if len(values) != 5:
                raise ValueError(f"Invalid label: {label_path}: {line}")

            class_id = int(values[0])

            if class_id != 0:
                continue

            x_center, y_center, box_w, box_h = map(
                float, values[1:]
            )

            # Convert normalized coordinates to source pixels.
            cx = x_center * width
            cy = y_center * height
            bw = box_w * width
            bh = box_h * height

            person_boxes.append((cx, cy, bw, bh))

        small, scale, left, top = letterbox_grayscale(image)

        # Adjust boxes for resizing and padding.
        output_lines = []

        for cx, cy, bw, bh in person_boxes:
            cx_new = (cx * scale + left) / TARGET_W
            cy_new = (cy * scale + top) / TARGET_H
            bw_new = (bw * scale) / TARGET_W
            bh_new = (bh * scale) / TARGET_H

            output_lines.append(
                f"0 {cx_new:.6f} {cy_new:.6f} "
                f"{bw_new:.6f} {bh_new:.6f}"
            )

        output_image = out_images / f"{image_path.stem}.png"
        output_label = out_labels / f"{image_path.stem}.txt"

        if not cv2.imwrite(str(output_image), small):
            raise RuntimeError(f"Could not save {output_image}")

        # An empty file means no annotated people in this image.
        output_label.write_text(
            "\n".join(output_lines) +
            ("\n" if output_lines else "")
        )

        processed += 1
        people_found += len(person_boxes)

    print(
        f"{split}: {processed} images processed; "
        f"{people_found} person boxes retained"
    )


def main():
    for split in SPLITS:
        process_split(split)

    print("Target image size: 160 x 128")
    print("Image format: grayscale PNG")
    print("Class retained: person (class 0)")
    print("Output:", OUTPUT)


if __name__ == "__main__":
    main()

