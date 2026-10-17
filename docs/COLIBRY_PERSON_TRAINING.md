# Training a compact Colibry person detector

This training entry point produces a small, hardware-oriented grid detector for
the project's **160-pixel-wide × 128-pixel-high** grayscale input. It is not the
original YOLOv8 architecture. The original YOLO model can provide optional
training-only pseudo boxes, and should remain the accuracy reference.

## Input and labels

The processed dataset directory must contain:

```text
processed/
  images/
    train/
    val/
  labels/
    train/
    val/
```

Images must already be 160 × 128 pixels (width × height). They are converted
to grayscale and normalized to [0, 1]. Labels use standard YOLO text format:
`class_id center_x center_y width height`, normalized to [0, 1], with class
`0` representing a person. Include an empty `.txt` file for every image with
no person annotations so background images are retained during training.

Dataset images and model weights are not committed to GitHub. Pass the location
of your local processed dataset with `--dataset`.

## 1. Validate before training

Use the Python environment that has TensorFlow 2.18.x, `tf_keras`, NumPy, and
Pillow installed. From the repository root:

```bash
python scripts/model/train_colibry_person_detector.py \
  --dataset /path/to/coco_person_hardneg/processed \
  --teacher-train-cache /path/to/yolo_teacher_train.json \
  --check-only
```

The teacher-cache argument is optional. When supplied, it must be a JSON array
whose entries contain `image`, `teacher_xyxy`, and `teacher_scores`; box
coordinates are normalized `[x1, y1, x2, y2]`. The cache IDs must match the
training image IDs exactly. The teacher adds a limited number of high-confidence
boxes only to images that already have ground-truth people. It never adds
pseudo people to a known-empty image. Validation does not use pseudo boxes.

Check the printed contract before training:

- Keras input: `(N, 128, 160, 1)` (height, width, channel order).
- Image dimensions: **160 wide × 128 high**.
- Model grid output: `(N, 32, 40, 5)`.
- Training targets: `(N, 32, 40, 6)`; channel 6 is a target-only background eligibility mask.
- Background loss ignores cells whose centers fall inside a ground-truth or accepted teacher box.
- Known-empty image count and pseudo-label count.
- Ground-truth/label alignment and all image dimensions.

## 2. Train

After the validation check passes, run the same command without `--check-only`.
Optional flags include `--epochs`, `--batch-size`, `--learning-rate`,
`--positive-loss-weight`, `--negative-loss-weight`, `--channels`,
`--residual-blocks`, `--min-person-height-px`, `--pseudo-confidence`,
and `--output-dir`.

The positive-cell objectness loss multiplier is controlled by
`--positive-loss-weight` (experimental default: `2.0`); the combined dense
background/hard-negative term is controlled by `--negative-loss-weight`
(default: `1.0`). These values affect objectness gradients only. Evaluate
precision, recall and empty-image false detections before accepting a new setting.
The default backbone uses 24 feature channels and 6 residual blocks (the original
compact variant). Larger `--channels` or changed block counts produce a different
weight shape; do not load a checkpoint across architecture settings. Test variants
with the same validation protocol and retain the smallest one that demonstrates
a credible precision/recall trade-off.

For example:

```bash
python scripts/model/train_colibry_person_detector.py \
  --dataset /path/to/coco_person_hardneg/processed \
  --teacher-train-cache /path/to/yolo_teacher_train.json \
  --epochs 40 \
  --batch-size 8 \
  --output-dir runs/colibry_person_detector
```

The objectness loss averages positive-cell loss separately, then combines
per-image dense background loss with hard-negative mining. Background loss is
applied only to grid cells outside known ground-truth and accepted teacher boxes;
cells inside those boxes but away from the assigned center are ignored for
background loss to avoid contradictory supervision. The positive-cell multiplier
is controlled by `--positive-loss-weight` (experimental default: `2.0`), while the
combined negative term is controlled by `--negative-loss-weight` (default: `1.0`).
These settings and the box-aware mask are experimental. They do not guarantee
improved accuracy and must be evaluated on the validation split before deployment.

The run writes the best weights, final weights, Keras model, epoch history, and
`training_report.json` to the output directory. The report uses a greedy
one-to-one box-match metric with IoU 0.50 and NMS IoU 0.50; it is **not COCO
mAP**. Compare precision, recall, and false positives with the original
YOLOv8n baseline before deciding whether to continue.

## 3. Compile and test separately

Training success does not prove that the graph compiles for NNPA or works on a
physical ColibryNPU board. First inspect the saved model with
`scripts/model/check_yolo_colibry_compat.py`, then follow the documented
compiler gate. Do not flash firmware until the inference runtime and output
decoder are ready.
