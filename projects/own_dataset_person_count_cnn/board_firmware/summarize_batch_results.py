#!/usr/bin/env python3
"""Aggregate machine-readable RESULT lines from Colibry SRAM batch logs."""
from __future__ import annotations

import argparse
import csv
import re
import sys
from collections import Counter
from pathlib import Path

CLASSES = ("1_person", "2_person", "3_person")
RESULT_RE = re.compile(
    r"^RESULT,(\d+),(1_person|2_person|3_person),(\d+),(\d+),(\d+),(\d+)\s*$"
)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("logs", nargs="+", type=Path)
    parser.add_argument("--expected-images", type=int, default=71)
    parser.add_argument("--csv", type=Path, default=None)
    args = parser.parse_args()

    rows: dict[int, tuple[str, int, int, int, int]] = {}
    errors: list[str] = []
    for path in args.logs:
        if not path.is_file():
            errors.append(f"Missing log: {path}")
            continue
        for line_number, line in enumerate(
            path.read_text(encoding="utf-8", errors="replace").splitlines(), 1
        ):
            if not line.startswith("RESULT,"):
                continue
            match = RESULT_RE.match(line.strip())
            if not match:
                errors.append(f"{path}:{line_number}: malformed RESULT row: {line}")
                continue
            index = int(match.group(1))
            expected = match.group(2)
            predicted_id = int(match.group(3))
            logits = tuple(int(match.group(i)) for i in range(4, 7))
            if predicted_id not in range(3):
                errors.append(f"{path}:{line_number}: invalid predicted class ID {predicted_id}")
                continue
            if index in rows:
                errors.append(f"Duplicate dataset index {index} (also in {path}:{line_number})")
                continue
            rows[index] = (expected, predicted_id, *logits)

    if errors:
        print("===== INPUT LOG ERRORS =====")
        for error in errors:
            print(error)

    expected_indices = set(range(args.expected_images))
    actual_indices = set(rows)
    missing = sorted(expected_indices - actual_indices)
    unexpected = sorted(actual_indices - expected_indices)

    matrix = [[0 for _ in CLASSES] for _ in CLASSES]
    per_class_expected = Counter()
    per_class_predicted = Counter()
    correct = 0

    for index, (expected, predicted, *_logits) in sorted(rows.items()):
        true_id = CLASSES.index(expected)
        matrix[true_id][predicted] += 1
        per_class_expected[true_id] += 1
        per_class_predicted[predicted] += 1
        correct += int(true_id == predicted)

    print("\n===== BOARD DATASET INFERENCE AUDIT =====")
    print(f"Unique results: {len(rows)} / {args.expected_images}")
    print(f"Missing indices: {missing}")
    print(f"Unexpected indices: {unexpected}")
    print(f"Duplicate/malformed/missing-log errors: {len(errors)}")
    print(f"Correct predictions: {correct} / {len(rows)}")
    print(f"Accuracy on collected rows: {correct / len(rows):.4f}" if rows else "Accuracy: n/a")

    print("\nConfusion matrix (rows=true, columns=predicted):")
    print("true\\pred," + ",".join(CLASSES))
    for name, values in zip(CLASSES, matrix):
        print(name + "," + ",".join(str(value) for value in values))

    print("\nPer-class metrics:")
    print("class,support,precision,recall,f1")
    for class_id, name in enumerate(CLASSES):
        tp = matrix[class_id][class_id]
        support = sum(matrix[class_id])
        predicted = sum(matrix[row][class_id] for row in range(3))
        precision = tp / predicted if predicted else 0.0
        recall = tp / support if support else 0.0
        f1 = 2 * precision * recall / (precision + recall) if precision + recall else 0.0
        print(f"{name},{support},{precision:.4f},{recall:.4f},{f1:.4f}")

    if args.csv:
        args.csv.parent.mkdir(parents=True, exist_ok=True)
        with args.csv.open("w", newline="", encoding="utf-8") as stream:
            writer = csv.writer(stream)
            writer.writerow([
                "dataset_index", "expected", "predicted",
                "logit_1_person", "logit_2_person", "logit_3_person",
            ])
            for index, (expected, predicted, *logits) in sorted(rows.items()):
                writer.writerow([index, expected, CLASSES[predicted], *logits])
        print(f"\nCSV written: {args.csv}")

    # Distinguish a complete, clean report from partial batch coverage.
    return 0 if not errors and not missing and not unexpected else 2


if __name__ == "__main__":
    raise SystemExit(main())
