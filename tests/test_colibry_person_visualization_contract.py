"""Dependency-light contract tests for the Colibry visual diagnostic utility."""
from __future__ import annotations

import ast
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
VISUALIZER = ROOT / "scripts" / "model" / "visualize_colibry_person_detections.py"
SOURCE = VISUALIZER.read_text(encoding="utf-8")
TREE = ast.parse(SOURCE)


class ColibryPersonVisualizationContractTests(unittest.TestCase):
    def test_script_is_valid_python_source(self) -> None:
        self.assertIsInstance(TREE, ast.Module)

    def test_visualizer_preserves_160_by_128_input(self) -> None:
        self.assertIn("IMAGE_WIDTH = 160", SOURCE)
        self.assertIn("IMAGE_HEIGHT = 128", SOURCE)
        self.assertIn("(IMAGE_HEIGHT, IMAGE_WIDTH, 1)", SOURCE)

    def test_visualizer_uses_same_yolo_grid_and_nms_coordinate_order(self) -> None:
        self.assertIn("GRID_COLS = 40", SOURCE)
        self.assertIn("GRID_ROWS = 32", SOURCE)
        self.assertIn("boxes[:, [1, 0, 3, 2]]", SOURCE)
        self.assertIn("iou_threshold=NMS_IOU", SOURCE)

    def test_visualizer_marks_ground_truth_true_positives_and_false_positives(self) -> None:
        self.assertIn("GT_COLOR =", SOURCE)
        self.assertIn("TP_COLOR =", SOURCE)
        self.assertIn("FP_COLOR =", SOURCE)
        self.assertIn('"tp": tp', SOURCE)
        self.assertIn('"fp": fp', SOURCE)
        self.assertIn('"fn": len(ground_truth) - tp', SOURCE)

    def test_visualizer_outputs_montage_and_json_report(self) -> None:
        self.assertIn('"diagnostic_montage.png"', SOURCE)
        self.assertIn('"diagnostic_report.json"', SOURCE)
        self.assertIn("negative_images_with_detections", SOURCE)


if __name__ == "__main__":
    unittest.main()
