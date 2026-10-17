"""Dependency-light contract tests for cached YOLO teacher diagnostics."""
from __future__ import annotations

import ast
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "scripts" / "model" / "visualize_yolo_teacher_cache.py"
SOURCE = SCRIPT.read_text(encoding="utf-8")
TREE = ast.parse(SOURCE)


class YoloTeacherCacheVisualizationTests(unittest.TestCase):
    def test_script_parses_as_python(self) -> None:
        self.assertIsInstance(TREE, ast.Module)

    def test_input_dimensions_match_project_contract(self) -> None:
        self.assertIn("IMAGE_WIDTH = 160", SOURCE)
        self.assertIn("IMAGE_HEIGHT = 128", SOURCE)
        self.assertIn("pixels.shape != (IMAGE_HEIGHT, IMAGE_WIDTH)", SOURCE)

    def test_teacher_cache_reads_box_and_score_arrays(self) -> None:
        self.assertIn('record.get("teacher_xyxy", [])', SOURCE)
        self.assertIn('record.get("teacher_scores", [])', SOURCE)
        self.assertIn("if len(boxes) != len(scores):", SOURCE)

    def test_teacher_cache_ids_must_match_validation_images(self) -> None:
        self.assertIn("if set(cache) != set(image_map):", SOURCE)
        self.assertIn("Teacher cache IDs don't match validation images", SOURCE)

    def test_outputs_visual_montage_and_per_image_report(self) -> None:
        self.assertIn('"teacher_diagnostic_montage.png"', SOURCE)
        self.assertIn('"teacher_diagnostic_report.json"', SOURCE)
        self.assertIn('"empty_images_with_detections"', SOURCE)

    def test_ground_truth_and_prediction_colors_are_explicit(self) -> None:
        self.assertIn("GT_COLOR =", SOURCE)
        self.assertIn("TP_COLOR =", SOURCE)
        self.assertIn("FP_COLOR =", SOURCE)
        self.assertIn("box_match_iou_threshold", SOURCE)


if __name__ == "__main__":
    unittest.main()
