"""Lightweight contract tests for the Colibry training entry point.

These tests intentionally use only the Python standard library so CI can catch
resolution regressions without installing TensorFlow on every pull request.
"""
from __future__ import annotations

import ast
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TRAINER = ROOT / "scripts" / "model" / "train_colibry_person_detector.py"
SOURCE = TRAINER.read_text(encoding="utf-8")
TREE = ast.parse(SOURCE)


def module_integer(name: str) -> int:
    for node in TREE.body:
        value = None
        if isinstance(node, ast.Assign):
            if any(isinstance(target, ast.Name) and target.id == name for target in node.targets):
                value = node.value
        elif isinstance(node, ast.AnnAssign) and isinstance(node.target, ast.Name) and node.target.id == name:
            value = node.value
        if isinstance(value, ast.Constant) and isinstance(value.value, int):
            return value.value
    raise AssertionError(f"Could not find integer module constant {name!r}")


class ColibryPersonTrainingContractTests(unittest.TestCase):
    def test_target_resolution_is_160_wide_by_128_high(self) -> None:
        self.assertEqual(module_integer("IMAGE_WIDTH"), 160)
        self.assertEqual(module_integer("IMAGE_HEIGHT"), 128)
        self.assertEqual(module_integer("INPUT_CHANNELS"), 1)

    def test_grid_matches_stride_four_feature_map(self) -> None:
        self.assertEqual(module_integer("GRID_COLS"), 40)
        self.assertEqual(module_integer("GRID_ROWS"), 32)
        self.assertEqual(module_integer("VALUES_PER_CELL"), 5)

    def test_keras_input_uses_height_width_channel_order(self) -> None:
        self.assertIn(
            "shape=(IMAGE_HEIGHT, IMAGE_WIDTH, INPUT_CHANNELS)",
            SOURCE,
        )
        self.assertIn(
            "pixels.shape != (IMAGE_HEIGHT, IMAGE_WIDTH)",
            SOURCE,
        )

    def test_loss_normalizes_positive_and_negative_image_terms_separately(self) -> None:
        self.assertIn("pos_loss_per_image", SOURCE)
        self.assertIn("positive_image_mask", SOURCE)
        self.assertIn("negative_loss = tf.reduce_mean(negative_loss_per_image)", SOURCE)
        self.assertIn(
            "objectness_loss = pos_loss + NEGATIVE_LOSS_WEIGHT * negative_loss",
            SOURCE,
        )
        self.assertNotIn("pos_loss + 1.5 * hard_neg_loss", SOURCE)

    def test_saved_weights_can_be_evaluated_without_retraining(self) -> None:
        self.assertIn('"--evaluate-weights"', SOURCE)
        self.assertIn("model.load_weights(str(args.evaluate_weights))", SOURCE)

    def test_native_keras_save_does_not_pass_unsupported_optimizer_argument(self) -> None:
        self.assertIn('model.save(str(args.output_dir / "colibry_person_detector.keras"))', SOURCE)
        self.assertNotIn("include_optimizer=False", SOURCE)

    def test_known_empty_images_are_not_given_teacher_pseudo_labels(self) -> None:
        self.assertIn("if teacher_record is None or not boxes:", SOURCE)
        self.assertIn('"pseudo_labels_in_known_empty_images": 0', SOURCE)


if __name__ == "__main__":
    unittest.main()
