from pathlib import Path
import csv
import cv2

BASE = Path("dataset/coco_person_1000")
ROOT = BASE / "processed"
AUDIT = Path("tests/empty_label_audit")
WIDTH, HEIGHT = 160, 128

DECISIONS_FILE = AUDIT / "review_decisions.csv"
MANIFEST_FILE = BASE / "manifest.csv"

errors = []
warnings = []
image_count = 0
box_count = 0
empty_labels = 0
small_boxes = 0
active_ids = {"train": set(), "val": set()}

# Load documented review decisions.
decisions = {}
if not DECISIONS_FILE.exists():
    errors.append(f"Missing review decisions file: {DECISIONS_FILE}")
else:
    with DECISIONS_FILE.open(newline="", encoding="utf-8") as f:
        for row in csv.DictReader(f):
            decisions[row["image_id"]] = row["decision"]

expected_decisions = {
    "000000198635": "KEEP_EMPTY_LABEL",
    "000000454637": "KEEP_EMPTY_LABEL",
    "000000252203": "EXCLUDE_FROM_ACTIVE_DATASET",
}
for image_id, expected in expected_decisions.items():
    if decisions.get(image_id) != expected:
        errors.append(
            f"Missing or incorrect decision for {image_id}: "
            f"expected {expected}"
        )

# Check active train/validation data.
for split in ("train", "val"):
    image_dir = ROOT / "images" / split
    label_dir = ROOT / "labels" / split
    images = sorted(image_dir.glob("*.png"))
    labels = sorted(label_dir.glob("*.txt"))

    image_stems = {p.stem for p in images}
    label_stems = {p.stem for p in labels}
    active_ids[split] = image_stems

    if image_stems != label_stems:
        errors.append(
            f"{split}: image/label mismatch "
            f"(images={len(image_stems)}, labels={len(label_stems)})"
        )

    for image_path in images:
        image_count += 1
        image = cv2.imread(str(image_path), cv2.IMREAD_UNCHANGED)

        if image is None:
            errors.append(f"Unreadable image: {image_path}")
            continue

        if image.shape != (HEIGHT, WIDTH):
            errors.append(f"Wrong shape: {image_path}: {image.shape}")

        label_path = label_dir / f"{image_path.stem}.txt"
        if not label_path.exists():
            continue

        lines = label_path.read_text(encoding="utf-8").splitlines()
        lines = [line.strip() for line in lines if line.strip()]

        if not lines:
            empty_labels += 1

        for line_number, line in enumerate(lines, start=1):
            parts = line.split()
            if len(parts) != 5:
                errors.append(
                    f"Invalid label format: {label_path}:{line_number}"
                )
                continue

            try:
                cls, cx, cy, bw, bh = map(float, parts)
            except ValueError:
                errors.append(
                    f"Non-numeric label: {label_path}:{line_number}"
                )
                continue

            if cls != 0:
                errors.append(
                    f"Unexpected class ID: {label_path}:{line_number}"
                )

            if not (
                0 <= cx <= 1 and 0 <= cy <= 1
                and 0 < bw <= 1 and 0 < bh <= 1
                and cx - bw / 2 >= -1e-6
                and cx + bw / 2 <= 1 + 1e-6
                and cy - bh / 2 >= -1e-6
                and cy + bh / 2 <= 1 + 1e-6
            ):
                errors.append(
                    f"Out-of-range box: {label_path}:{line_number}"
                )

            if bh < 8 / HEIGHT:
                small_boxes += 1

            box_count += 1

    print(
        f"{split}: {len(images)} images, "
        f"{len(labels)} label files"
    )

# Check train/validation leakage.
overlap = active_ids["train"] & active_ids["val"]
if overlap:
    errors.append(
        f"Train/validation overlap: {len(overlap)} IDs; "
        f"examples: {sorted(overlap)[:10]}"
    )

# Check the excluded image has not re-entered active data.
excluded_id = "000000252203"
if excluded_id in active_ids["train"] or excluded_id in active_ids["val"]:
    errors.append(f"Excluded image is still active: {excluded_id}")

excluded_dir = BASE / "review_excluded"
if not (excluded_dir / "images" / f"{excluded_id}.png").exists():
    warnings.append("Excluded driver's processed image was not found.")
if not (excluded_dir / "labels" / f"{excluded_id}.txt").exists():
    warnings.append("Excluded driver's label record was not found.")

print("\nValidation summary")
print(f"Active images checked: {image_count}")
print(f"Person boxes checked: {box_count}")
print(f"Empty label files: {empty_labels}")
print(f"Boxes shorter than 8 pixels: {small_boxes}")
print(f"Train/validation overlap: {len(overlap)}")
print(f"Errors: {len(errors)}")
print(f"Warnings: {len(warnings)}")

for warning in warnings:
    print("WARNING:", warning)
for error in errors[:30]:
    print("ERROR:", error)

if errors:
    raise SystemExit(
        "Validation failed. Review the errors before training."
    )

print("\nStructural validation passed.")
print("Documented review decisions were checked.")
print("Visual annotation review is not a guarantee of label accuracy.")
