# Task 2 — Minimal two-layer CNN

This is the small CNN requested by the professor. It uses an image as input and has exactly two trainable layers:

| Stage | Operation | Configuration |
|---|---|---|
| Input | Grayscale image | 32 x 32 x 1 |
| Layer 1 | Conv2D | 4 filters, 3 x 3 kernel, ReLU |
| Shape operation | Flatten | No trainable parameters |
| Layer 2 | Dense | 1 output, sigmoid |

The output is a binary classification:
- `no_person` — probability below 0.5
- `person` — probability of 0.5 or above

This is a tiny classification test, not a bounding-box detector.

## Install

Use the Python environment on the development computer:

```bash
python3 -m pip install tensorflow==2.18.1
```

## Train and export

From the repository root, after placing collected images in the folders described in `task3_dataset/README.md`:

```bash
python3 experiments/simple_person_cnn/simple_cnn.py --epochs 5
```

The script reports the model architecture and validation accuracy, then saves the Keras and TensorFlow Lite models in the local `model/` folder.

## Test one image

```bash
python3 experiments/simple_person_cnn/simple_cnn.py --predict /path/to/person.jpg
```

The TensorFlow Lite file is an export for the next compatibility check. It is not automatically a Colibry board executable; the Colibry compiler/SDK must accept the model before board inference can be claimed.
