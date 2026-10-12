import random
from pathlib import Path

import cv2

ROOT = Path("dataset/coco_person_1000/processed")
OUT = Path("tests/coco_person_1000_audit")
OUT.mkdir(parents=True, exist_ok=True)

random.seed(42)
selected = []

for split in ("train", "val"):
    image_dir = ROOT / "images" / split
    label_dir = ROOT / "labels" / split

    positives = []
    negatives = []

    for image_path in sorted(image_dir.glob("*.png")):
        label_path = label_dir / f"{image_path.stem}.txt"
        if not label_path.exists():
            continue

        lines = [
            line for line in
            label_path.read_text(encoding="utf-8").splitlines()
            if line.strip()
        ]

        if lines:
            positives.append(image_path)
        else:
            negatives.append(image_path)

    random.shuffle(positives)
    random.shuffle(negatives)

    # Review both splits, including some empty-label candidates.
    selected.extend((split, p) for p in positives[:12])
    selected.extend((split, p) for p in negatives[:8])

for index, (split, image_path) in enumerate(selected, start=1):
    label_path = ROOT / "labels" / split / f"{image_path.stem}.txt"
    image = cv2.imread(str(image_path), cv2.IMREAD_GRAYSCALE)

    if image is None:
        continue

    preview = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)

    for line in label_path.read_text(encoding="utf-8").splitlines():
        parts = line.split()
        if len(parts) != 5:
            continue

        _, cx, cy, bw, bh = map(float, parts)

        x1 = round((cx - bw / 2) * image.shape[1])
        y1 = round((cy - bh / 2) * image.shape[0])
        x2 = round((cx + bw / 2) * image.shape[1])
        y2 = round((cy + bh / 2) * image.shape[0])

        cv2.rectangle(preview, (x1, y1), (x2, y2), (0, 255, 0), 1)

    name = f"{index:03d}_{split}_{image_path.stem}.jpg"
    cv2.imwrite(str(OUT / name), preview)

print(f"Created {len(selected)} previews in: {OUT}")
print("Green rectangles show person annotations.")
