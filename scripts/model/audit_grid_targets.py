from collections import Counter
from pathlib import Path
import math

ROOT = Path(__file__).resolve().parents[2]
DATASET = ROOT / "dataset/coco_person_1000/processed"

IMAGE_WIDTH = 160
IMAGE_HEIGHT = 128
GRID_COLS = 10
GRID_ROWS = 8

total_images = 0
positive_images = 0
empty_images = 0
total_boxes = 0
malformed_lines = 0
invalid_boxes = 0
tiny_boxes = 0
images_with_collisions = 0
total_extra_boxes = 0
max_boxes_in_cell = 0
total_colliding_cells = 0

for split in ("train", "val"):
    label_dir = DATASET / "labels" / split
    split_images = 0
    split_boxes = 0
    split_empty = 0
    split_collisions = 0
    split_extra = 0

    if not label_dir.exists():
        print(f"ERROR: Missing label directory: {label_dir}")
        raise SystemExit(1)

    labels = sorted(label_dir.glob("*.txt"))

    print(f"\n=== {split.upper()} ===")
    print("Label files found:", len(labels))

    for label_path in labels:
        total_images += 1
        split_images += 1
        text = label_path.read_text().strip()

        if not text:
            empty_images += 1
            split_empty += 1
            continue

        positive_images += 1
        cell_counts = Counter()

        for line in text.splitlines():
            parts = line.split()
            if len(parts) != 5:
                malformed_lines += 1
                continue

            try:
                cls, cx, cy, bw, bh = map(float, parts)
            except ValueError:
                malformed_lines += 1
                continue

            values = (cls, cx, cy, bw, bh)
            if (
                not all(math.isfinite(v) for v in values)
                or cls != 0
                or not (0 <= cx <= 1 and 0 <= cy <= 1)
                or not (0 < bw <= 1 and 0 < bh <= 1)
            ):
                invalid_boxes += 1
                continue

            total_boxes += 1
            split_boxes += 1

            if bw * IMAGE_WIDTH < 8 or bh * IMAGE_HEIGHT < 8:
                tiny_boxes += 1

            col = min(GRID_COLS - 1, int(cx * GRID_COLS))
            row = min(GRID_ROWS - 1, int(cy * GRID_ROWS))
            cell_counts[(row, col)] += 1

        colliding = [n for n in cell_counts.values() if n > 1]
        if colliding:
            images_with_collisions += 1
            split_collisions += 1
            total_colliding_cells += len(colliding)
            extra = sum(n - 1 for n in colliding)
            total_extra_boxes += extra
            split_extra += extra
            max_boxes_in_cell = max(max_boxes_in_cell, max(colliding))

    print("Images:", split_images)
    print("Images with no boxes:", split_empty)
    print("Valid person boxes:", split_boxes)
    print("Images with multiple boxes in a grid cell:", split_collisions)
    print("Extra boxes lost by one-box-per-cell target:", split_extra)

print("\n=== OVERALL SUMMARY ===")
print("Image/label files audited:", total_images)
print("Images with person boxes:", positive_images)
print("Empty-label images:", empty_images)
print("Valid person boxes:", total_boxes)
print("Images with grid-cell collisions:", images_with_collisions)
print("Colliding grid cells:", total_colliding_cells)
print("Extra boxes that a one-box-per-cell target cannot represent:",
      total_extra_boxes)
print("Maximum boxes sharing one cell:", max_boxes_in_cell)
print("Boxes with width or height under 8 input pixels:", tiny_boxes)
print("Malformed label lines:", malformed_lines)
print("Invalid boxes:", invalid_boxes)
