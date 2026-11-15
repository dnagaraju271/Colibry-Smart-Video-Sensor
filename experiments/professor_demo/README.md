# Professor's basic board demonstrations

This folder contains only the three demonstrations requested for the Colibry board meeting:

1. A simple C LED blinker.
2. A very small two-trainable-layer CNN using a grayscale image as input.
3. Our own outdoor person / no-person image dataset.

## 1. C LED blinker

See [task1_c_blinker/README.md](task1_c_blinker/README.md). The already-tested board-specific C source must be copied here as `main.c`. It is deliberately not replaced with guessed GPIO or SDK function names.

## 2. Simple CNN

The image classifier has a 32 x 32 grayscale input, one Conv2D layer and one Dense output layer. Flatten only reshapes the feature map; it has no trainable parameters.

Dataset folders are in `task3_dataset/images/`. After photographs have been collected and sorted, run from the repository root:

```bash
python3 -m pip install tensorflow==2.18.1
python3 experiments/professor_demo/task2_simple_cnn/simple_cnn.py --epochs 5
```

The script trains the model, evaluates it on the validation images, and writes:
- `simple_person_cnn.keras`
- `simple_person_cnn.tflite`

Both files are written to `experiments/professor_demo/task2_simple_cnn/model/`.

To classify one image after training:

```bash
python3 experiments/professor_demo/task2_simple_cnn/simple_cnn.py --predict /path/to/image.jpg
```

This is a small person / no-person classification experiment, not the full bounding-box detector.

## 3. Our own dataset

See [task3_dataset/README.md](task3_dataset/README.md). Put the photographs taken by the team into the matching train/validation folders. The image folders are currently empty placeholders; real photographs have not yet been collected.

## Move the files to the board computer

After the changes are committed and pushed to GitHub, on the Ubuntu board computer:

```bash
git pull origin main
```

Run the CNN training/export on the development computer with the collected dataset. Copy the generated `.tflite` model and the board-compatible project files to the Colibry SDK workflow only after confirming the SDK's supported model format and operators.
