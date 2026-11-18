# Professor's basic board demonstrations

This folder contains only the three demonstrations requested for the Colibry board assignment:

1. A simple C LED blinker.
2. A very small CNN with exactly two trainable layers.
3. Collection and organization of the team's own person-image dataset.

## 1. C LED blinker

See task1_c_blinker/README.md for the C source and SDK Makefile.

The physical board's red user LED has been observed blinking. The current booted LED firmware's provenance has not been re-verified against this repository's main.c.

## 2. Small CNN — own data and board inference

See task2_small_cnn/README.md.

- Training is performed on the Ubuntu host using TensorFlow and the team's dataset.
- Model architecture: Conv2D + Dense, with Flatten between them; exactly two trainable layers.
- Input: 40 x 30 grayscale, one channel.
- Output classes: 1_person, 2_person, 3_person.
- The trained model has already been quantized to uint8 and compiled into an NNPA bitstream.
- The SRAM-only test app embeds the NNPA bitstream in its firmware, copies it into NNPA SRAM, and tests one input from each class.
- The SRAM test uses the SDK's tosram path and does not rewrite external flash sector 0x20000.

The current experiment is not a reliable people counter yet. Its prior image-level validation accuracy was 80%, but recall was zero for the 2-person and 3-person validation examples. The dataset is small and imbalanced, and all four 3-person images come from one source group.

## 3. Own dataset

Canonical archive:

datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip

Current labeled distribution:

| Class | Images |
|---|---:|
| 1_person | 59 |
| 2_person | 8 |
| 3_person | 4 |
| **Total** | **71** |

Three zero-person originals remain in pending_for_later_originals and are not part of the current three-class model. The extracted datasets/mobile_160x120/processed/ directory is byte-for-byte the same 71 labeled images from the archive; it is not additional data.

The class and source-group limitations are documented in the current session handoff at ../../docs/SESSION_HANDOFF_2026-11-18.md. More independently captured 2-person and 3-person examples are needed before claiming good generalization.
