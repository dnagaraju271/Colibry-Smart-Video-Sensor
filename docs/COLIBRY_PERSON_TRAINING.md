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
- Training targets: `(N, 32, 40, 8)`; the extra channels carry positive-loss weights, background eligibility, and box-regression weights.
- Only assigned ground-truth/teacher centre cells are excluded from the background loss; every other cell is trained as background. This matches the decoder, where each grid cell predicts a box centered on that cell.
- Known-empty image count and pseudo-label count.
- Ground-truth/label alignment and all image dimensions.

## 2. Train

After the validation check passes, run the same command without `--check-only`.
Optional flags include `--epochs`, `--batch-size`, `--learning-rate`,
`--positive-loss-weight`, `--negative-loss-weight`, `--teacher-box-weight`,
`--channels`, `--residual-blocks`, `--min-person-height-px`,
`--pseudo-confidence`, and `--output-dir`.

The positive-cell objectness loss multiplier is controlled by
`--positive-loss-weight` (experimental default: `2.0`); the combined dense
background/hard-negative term is controlled by `--negative-loss-weight`
(default: `1.0`). Accepted teacher-only boxes use soft objectness labels from
the teacher's confidence scores and a reduced box-regression weight configured by
`--teacher-box-weight` (default: `0.25`). Ground-truth boxes remain authoritative;
teacher-only pseudo labels are never added to known-empty images or when they
overlap ground truth above the exclusion threshold. These mechanisms are
experimental and must be validated. The default backbone uses 24 feature channels
and 6 residual blocks; larger `--channels` or changed block counts produce a
different weight shape, so do not load checkpoints across architectures.

For example:

```bash
python scripts/model/train_colibry_person_detector.py \
  --dataset /path/to/coco_person_hardneg/processed \
  --teacher-train-cache /path/to/yolo_teacher_train.json \
  --epochs 40 \
  --batch-size 8 \
  --output-dir runs/colibry_person_detector
```

The objectness loss averages weighted positive-cell loss separately,
then combines dense per-image background loss with hard-negative mining. Every
unassigned grid cell contributes to background loss; only cells assigned a GT or
accepted teacher target are excluded. This is intentional because the detector
predicts one cell-centred box per grid cell. Teacher-only cells use the cached
teacher confidence as a soft objectness target and a reduced box weight, rather
than treating every teacher box as a hard ground-truth label. These distillation
changes are experimental and do not guarantee improved accuracy. Compare
precision, recall, false positives, and empty-image detections against the YOLO
teacher before considering deployment.

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
