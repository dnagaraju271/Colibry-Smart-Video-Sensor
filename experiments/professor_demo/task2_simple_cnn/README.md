# Task 2 — Independent Minimal CNN

This is the professor's separate two-trainable-layer CNN implementation. It does not import or use the root-level `simple_person_cnn/` baseline.

## Architecture

| Stage | Operation | Configuration |
|---|---|---|
| Input | Grayscale image | 32 × 32 × 1 |
| Trainable layer 1 | Conv2D | 4 filters, 3 × 3 kernel, ReLU |
| Shape operation | Flatten | No trainable parameters |
| Trainable layer 2 | Dense | 1 output, sigmoid |

The sigmoid output is interpreted as:

- `no_person`: probability below 0.5
- `person`: probability at or above 0.5

Rescaling is a non-trainable preprocessing layer. The script checks that the only trainable layers are Conv2D and Dense.

## Dataset

The script uses the professor's separate Task 3 dataset by default:

```text
experiments/professor_demo/task3_dataset/images/
├── train/
│   ├── no_person/
│   └── person/
└── validation/
    ├── no_person/
    └── person/
```

Collect real outdoor photographs and keep training and validation scenes separate. The dataset folders are managed by Task 3.

## Install

On the development computer:

```bash
python3 -m pip install tensorflow==2.18.1 numpy
```

## Inspect and verify the architecture

From the repository root:

```bash
python3 -c "from experiments.professor_demo.task2_simple_cnn.simple_cnn import build_model; m=build_model(); print('Trainable layers:', [l.name for l in m.layers if l.trainable_weights]); m.summary()"
```

Expected trainable layers: `conv2d` and `person_probability`.

## Train, evaluate, and export

After Task 3 contains training and validation photographs:

```bash
python3 experiments/professor_demo/task2_simple_cnn/simple_cnn.py --epochs 5
```

An alternate dataset root can be supplied with `--data-dir`.

The script reports validation loss and accuracy, then saves:

- `model/professor_simple_cnn.keras`
- `model/professor_simple_cnn.tflite`

## Predict one image

After training:

```bash
python3 experiments/professor_demo/task2_simple_cnn/simple_cnn.py --predict /path/to/outdoor_image.jpg
```

This is an image-level person/no-person classifier. It does not produce bounding boxes and is not the main Colibry detector. TensorFlow Lite export alone does not establish Colibry compiler compatibility or physical-board inference.
