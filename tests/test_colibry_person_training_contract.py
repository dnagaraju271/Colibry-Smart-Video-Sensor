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

    def test_training_targets_hold_distillation_weights_without_changing_model_output(self) -> None:
        self.assertEqual(module_integer("VALUES_PER_CELL"), 5)
        self.assertEqual(module_integer("TARGET_CHANNELS"), 8)
        self.assertEqual(module_integer("TARGET_POSITIVE_WEIGHT_INDEX"), 5)
        self.assertEqual(module_integer("TARGET_BACKGROUND_MASK_INDEX"), 6)
        self.assertEqual(module_integer("TARGET_BOX_WEIGHT_INDEX"), 7)
        self.assertIn("target[..., TARGET_BACKGROUND_MASK_INDEX] = 1.0", SOURCE)
        self.assertIn('"model_output_values_per_cell": VALUES_PER_CELL', SOURCE)
        self.assertIn('"target_values_per_cell": TARGET_CHANNELS', SOURCE)

    def test_every_unassigned_cell_is_eligible_background(self) -> None:
        self.assertIn("background_mask = background_eligibility", SOURCE)
        self.assertIn("eligible_background = image_background_eligibility > 0.5", SOURCE)
        self.assertNotIn("def _mask_box_cells_from_background(", SOURCE)
        self.assertNotIn("background_cells_ignored_by_gt_boxes", SOURCE)

    def test_teacher_boxes_use_soft_objectness_and_reduced_box_weight(self) -> None:
        self.assertIn('"--teacher-box-weight"', SOURCE)
        self.assertIn("soft_score = float(np.clip(score, 0.01, 0.99))", SOURCE)
        self.assertIn("target[row, col, TARGET_POSITIVE_WEIGHT_INDEX] = soft_score", SOURCE)
        self.assertIn("target[row, col, TARGET_BOX_WEIGHT_INDEX] = soft_score * teacher_box_weight", SOURCE)
        self.assertIn('"teacher_soft_target_cells"', SOURCE)

    def test_detector_backbone_capacity_is_configurable(self) -> None:
        self.assertIn('"--channels"', SOURCE)
        self.assertIn('"--residual-blocks"', SOURCE)
        self.assertIn("build_model(channels=args.channels, residual_blocks=args.residual_blocks)", SOURCE)
        self.assertIn('"model_architecture"', SOURCE)

    def test_keras_input_uses_height_width_channel_order(self) -> None:
        self.assertIn(
            "shape=(IMAGE_HEIGHT, IMAGE_WIDTH, INPUT_CHANNELS)",
            SOURCE,
        )
        self.assertIn(
            "pixels.shape != (IMAGE_HEIGHT, IMAGE_WIDTH)",
            SOURCE,
        )

    def test_negative_loss_weight_is_configurable(self) -> None:
        self.assertIn('"--negative-loss-weight"', SOURCE)
        self.assertIn("NEGATIVE_LOSS_WEIGHT = args.negative_loss_weight", SOURCE)
        self.assertIn('"negative_loss_weight": NEGATIVE_LOSS_WEIGHT', SOURCE)

    def test_positive_loss_weight_is_configurable(self) -> None:
        self.assertIn('"--positive-loss-weight"', SOURCE)
        self.assertIn("POSITIVE_LOSS_WEIGHT = args.positive_loss_weight", SOURCE)
        self.assertIn('"positive_loss_weight": POSITIVE_LOSS_WEIGHT', SOURCE)
        self.assertIn(
            "objectness_loss = POSITIVE_LOSS_WEIGHT * pos_loss + NEGATIVE_LOSS_WEIGHT * negative_loss",
            SOURCE,
        )

    def test_loss_balances_positive_dense_background_and_hard_negative_terms(self) -> None:
        self.assertIn("pos_loss_per_image", SOURCE)
        self.assertIn("positive_image_mask", SOURCE)
        self.assertIn("all_background_per_image", SOURCE)
        self.assertIn("all_background_loss = tf.reduce_mean(all_background_per_image)", SOURCE)
        self.assertIn("hard_negative_loss = tf.reduce_mean(hard_negative_per_image)", SOURCE)
        self.assertIn(
            "negative_loss = 0.5 * all_background_loss + 0.5 * hard_negative_loss",
            SOURCE,
        )
        self.assertIn(
            "objectness_loss = POSITIVE_LOSS_WEIGHT * pos_loss + NEGATIVE_LOSS_WEIGHT * negative_loss",
            SOURCE,
        )
        self.assertNotIn("pos_loss + 1.5 * hard_neg_loss", SOURCE)

    def test_saved_weights_can_be_evaluated_without_retraining(self) -> None:
        self.assertIn('"--evaluate-weights"', SOURCE)
        self.assertIn("model.load_weights(str(args.evaluate_weights))", SOURCE)

    def test_validation_supports_multiple_confidence_thresholds(self) -> None:
        self.assertIn('"--eval-confidence-thresholds"', SOURCE)
        self.assertIn("for threshold in eval_thresholds", SOURCE)
        self.assertIn('"average_predictions_per_image"', SOURCE)

    def test_validation_reports_positive_and_background_objectness_scores(self) -> None:
        self.assertIn("def summarize_objectness_scores(", SOURCE)
        self.assertIn('"positive_target_cells"', SOURCE)
        self.assertIn('"background_cells"', SOURCE)
        self.assertIn('"teacher_soft_target_cells"', SOURCE)
        self.assertIn('"objectness_score_diagnostics"', SOURCE)

    def test_native_keras_save_does_not_pass_unsupported_optimizer_argument(self) -> None:
        self.assertIn('model.save(str(args.output_dir / "colibry_person_detector.keras"))', SOURCE)
        self.assertNotIn("include_optimizer=False", SOURCE)

    def test_known_empty_images_are_not_given_teacher_pseudo_labels(self) -> None:
        self.assertIn("if teacher_record is None or not boxes:", SOURCE)
        self.assertIn('"pseudo_labels_in_known_empty_images": 0', SOURCE)


if __name__ == "__main__":
    unittest.main()
