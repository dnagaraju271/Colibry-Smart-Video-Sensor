# Simple CNN Experiment Report

## Objective
Build and test a minimal CNN for binary person/no_person image classification.

## Input
- Image dimensions: 32 x 32
- Channels: 1 (grayscale)
- Classes: person and no_person

## Architecture
1. Rescaling (1/255)
2. Conv2D: 4 filters, 3 x 3 kernel, ReLU
3. Flatten
4. Dense: 1 neuron, sigmoid

Trainable parameters: 4,137

## Training Demonstration
Training images:
- 000000016730.png (person)
- 000000000459.png (no_person)

The two-image experiment demonstrates the training and model-export pipeline only.
It does not establish generalization accuracy.

## Prediction Results
| Image | Expected class | Prediction | Person probability |
|---|---|---|---|
| 000000016730.png | person | person | 0.8567 |
| 000000000459.png | no_person | no_person | 0.1814 |

## Exported Models
- model/simple_person_cnn.keras
- model/simple_person_cnn.tflite

## Limitation
A larger, independently collected outdoor dataset is required for meaningful
validation and generalization testing.
