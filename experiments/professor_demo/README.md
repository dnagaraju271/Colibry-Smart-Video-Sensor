# Professor's basic board demonstrations

This directory indexes the three demonstrations requested for the Colibry board assignment. Project implementation code is kept in its own project directory; this folder is the assignment overview.

## 1. Simple C LED blinker

Implementation: [task1_c_blinker/](task1_c_blinker/).

The physical board's red user LED has been observed blinking. The currently booted LED firmware's provenance has not been re-verified against this repository's `main.c`.

## 2. Very small CNN with two trainable layers

Implementation and host training: [Own-dataset person-count CNN project](../../projects/own_dataset_person_count_cnn/README.md).

The CNN has two trainable layers (Conv2D and Dense), with Flatten between them. Training runs on the Ubuntu host; the Colibry NNPA executes inference. The SRAM-only board inference test completed on 2026-11-18.

Current model limitation: the board correctly classified 1 of the three selected sample images. Host INT8 TFLite and board class predictions agreed for all three, but two images were misclassified. The model is not a reliable people counter yet.

## 3. Own outdoor image dataset

Dataset organization and collection notes: [task3_dataset/](task3_dataset/).

Canonical three-class dataset archive: `datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip`.

The current archive includes 59 one-person, 8 two-person and 4 three-person labelled images; three zero-person originals remain pending and excluded. All 4 three-person images come from one source group, so more independent examples are needed.

