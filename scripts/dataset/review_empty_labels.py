from pathlib import Path
import math
import cv2
import numpy as np

ROOT = Path("dataset/coco_person_1000/processed")
OUT = Path("tests/empty_label_contact_sheet.jpg")

items = []
for split in ("train", "val"):
    labels = sorted((ROOT / "labels" / split).glob("*.txt"))
    for label in labels:
        if not label.read_text(encoding="utf-8").strip():
            image_path = ROOT / "images" / split / f"{label.stem}.png"
            if image_path.exists():
                items.append((split, label.stem, image_path))

items = items[:40]

thumb_w, thumb_h = 240, 220
cols = 4
rows = math.ceil(len(items) / cols)
sheet = np.full((rows * thumb_h, cols * thumb_w, 3), 245, dtype=np.uint8)

for i, (split, stem, path) in enumerate(items):
    image = cv2.imread(str(path), cv2.IMREAD_GRAYSCALE)
    if image is None:
        continue

    thumb = cv2.resize(image, (220, 176), interpolation=cv2.INTER_AREA)
    thumb = cv2.cvtColor(thumb, cv2.COLOR_GRAY2BGR)

    x = (i % cols) * thumb_w + 10
    y = (i // cols) * thumb_h + 8

    sheet[y:y + 176, x:x + 220] = thumb
    cv2.putText(
        sheet, f"{split} {stem}", (x, y + 198),
        cv2.FONT_HERSHEY_SIMPLEX, 0.48, (0, 0, 0), 1
    )

OUT.parent.mkdir(parents=True, exist_ok=True)
cv2.imwrite(str(OUT), sheet)
print(f"Contact sheet: {OUT}")
print(f"Images displayed: {len(items)}")
