# Simple CNN - Quick Demo Guide

## Objective
Binary image classification:
- person
- no_person

## Input
- Image size: 32 x 32
- Grayscale: 1 channel

## CNN architecture
1. Rescaling (1/255)
2. Conv2D: 4 filters, 3x3 kernel, ReLU
3. Flatten
4. Dense: 1 neuron, sigmoid

Trainable parameters: 4,137

## Demo images
- demo_dataset/train/person/000000016730.png
- demo_dataset/train/no_person/000000000459.png

## Train the demonstration CNN
source ~/colibry-cnn-env/bin/activate
python train_demo.py

## Test predictions
python simple_cnn.py --predict test_images/person/000000016730.png
python simple_cnn.py --predict test_images/no_person/000000000459.png

## Model exports
- model/simple_person_cnn.keras
- model/simple_person_cnn.tflite

## Important
The two-image experiment demonstrates the training and inference pipeline.
It is not a reliable accuracy or generalization evaluation.
Use the separately collected outdoor dataset for real evaluation.
