# Simple person / no-person CNN experiment

This directory contains an earlier binary image-classification exercise for `person` versus `no_person` images. It is separate from the team's own three-class person-count CNN project.

## Architecture

| Stage | Operation | Configuration |
|---|---|---|
| Input | Grayscale image | 32 x 32 x 1 |
| Layer 1 | Conv2D | 4 filters, 3 x 3 kernel, ReLU |
| Shape operation | Flatten | No trainable parameters |
| Layer 2 | Dense | 1 output, sigmoid |

Trainable parameters: 4,137.

## Train and test

The binary experiment uses `train_demo.py` and `simple_cnn.py`. See [DEMO_GUIDE.md](DEMO_GUIDE.md) for the earlier two-image demonstration. That tiny experiment does not establish generalization accuracy.

## Separate three-class own-dataset CNN

The three-class model using the team's 160x120 grayscale dataset is maintained separately at [projects/own_dataset_person_count_cnn](../projects/own_dataset_person_count_cnn/README.md). It has 40x30 input and classes `1_person`, `2_person`, and `3_person`.

