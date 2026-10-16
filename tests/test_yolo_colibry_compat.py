"""Unit tests for the dependency-light parts of the YOLO compatibility checker."""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from scripts.model.check_yolo_colibry_compat import (  # noqa: E402
    _simple_pattern_match,
    assess_inputs,
    inspect_model,
)

TARGET = {
    "input_width": 160,
    "input_height": 128,
    "input_channels": 1,
    "input_color": "grayscale",
    "preferred_format": "tflite-int8",
}


class YoloCompatibilityCheckerTests(unittest.TestCase):
    def test_candidate_pattern_is_case_insensitive(self) -> None:
        self.assertTrue(_simple_pattern_match("YOLOv8n.ONNX", "*yolo*.onnx"))
        self.assertFalse(_simple_pattern_match("mobilenet_person.h5", "*yolo*.h5"))

    def test_nchw_rgb_640_input_reports_target_mismatch(self) -> None:
        model_info = {
            "inspected": True,
            "inputs": [{"name": "images", "shape": [1, 3, 640, 640], "layout": "unknown"}],
        }
        warnings = assess_inputs(model_info, TARGET)
        self.assertTrue(any("3 channels" in item for item in warnings))
        self.assertTrue(any("height is 640" in item for item in warnings))
        self.assertTrue(any("width is 640" in item for item in warnings))
        self.assertEqual(model_info["inputs"][0]["layout"], "NCHW")

    def test_nhwc_grayscale_target_has_no_mismatch(self) -> None:
        model_info = {
            "inspected": True,
            "inputs": [{"name": "grayscale_input", "shape": [1, 128, 160, 1], "layout": "NHWC"}],
        }
        self.assertEqual(assess_inputs(model_info, TARGET), [])

    def test_dynamic_shape_is_reported_as_warning(self) -> None:
        model_info = {
            "inspected": True,
            "inputs": [{"name": "images", "shape": ["batch", 128, 160, 1], "layout": "NHWC"}],
        }
        warnings = assess_inputs(model_info, TARGET)
        self.assertTrue(any("dynamic/unknown" in item for item in warnings))

    def test_model_with_unsupported_extension_is_not_inspected(self) -> None:
        # A missing path is fine here: extension validation happens before opening a file.
        result = inspect_model(ROOT / "example.weights")
        self.assertFalse(result["inspected"])
        self.assertIn("Unsupported extension", result["reason"])


if __name__ == "__main__":
    unittest.main()
