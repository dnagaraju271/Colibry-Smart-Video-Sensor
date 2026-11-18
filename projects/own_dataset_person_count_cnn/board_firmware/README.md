# Task 2 — Own-dataset two-trainable-layer CNN on Colibry

This is an SRAM-only hardware inference demonstration for the compact CNN
trained from the team's own grayscale dataset.

## Important distinction

The Colibry NNPA is used to **run inference**. The model weights are trained on
the Ubuntu host using TensorFlow. This firmware does not train or update
weights.

The host training and NNPA compilation have already been completed. The
existing model has exactly two trainable layers (Conv2D and Dense), 14,443
parameters, and a 40x30x1 grayscale input. Its validation results are weak:
80% image-level accuracy, but zero recall for the 2-person and 3-person classes
on the previous validation split. This firmware tests hardware execution;
it is not a claim of reliable people counting.

## Flash preservation

The NNPA bitstream and test inputs are generated into a C header and embedded
inside the firmware image. At runtime, the firmware copies the model directly
into NNPA SRAM and runs three input samples.

**Do not use \`toflash\`, \`nnpa\`, or the flash programming tool for this
demonstration.** The documented \`tosram\` path loads and executes the firmware
in volatile SRAM; the external flash sector at \`0x20000\` is not changed by
this application.

The existing original-sector and current-sector backups must remain untouched.

## Required local files

- SDK: \`~/colibryNPU_SDK\`
- NNPA model:
  \`~/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa\`
- Dataset:
  \`datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip\`
- Python environment:
  \`~/colibry-cnn-env\`

The generator checks the exact expected NNPA size and SHA-256 before embedding
it. It creates three test inputs using the same preprocessing as training:
160x120 grayscale resized with Lanczos to 40x30.

Samples:
- \`1_person/I1__008__120x160.png\`
- \`2_person/I2__007__120x160.png\`
- \`3_person/I3__004__120x160.png\`

## Prepare and build

From this directory:

\`\`\`bash
make prepare-data
make check-inputs
make all board=EVBTINY
\`\`\`

The build embeds the model and three input samples into the app. It does not
program flash.

## Run from SRAM

Follow the Evaluation Kit manual's **Wait Start** sequence:

1. Press and hold \`PWR_OFF\` on the Colibry Probe.
2. Press and hold the User Button on the EvalboardTiny.
3. Release \`PWR_OFF\`.
4. Release the User Button.

Then run:

\`\`\`bash
make all tosram board=EVBTINY
\`\`\`

This is intentionally the SRAM load path. Do not substitute \`toflash\` or run
\`make nnpa\`.

The UART console is \`/dev/ttyUSB2\` at 115200 baud per the Evaluation Kit
manual. Observe the three predictions and the raw uint8 output values.

## Expected checks

The application prints:
- NNPA bitstream CRC/configuration-check result;
- model ID and input/data size registers;
- input/output quantization registers;
- raw output bytes and three class logits for each sample;
- predicted class compared with the sample's folder label.

A model execution timeout, configuration failure, unexpected input size, or
output shape other than three values is reported as a failure.

The existing host-side confusion matrix showed that the old model predicted
\`1_person\` for both validation examples from \`2_person\` and the sole validation
example from \`3_person\`. A mismatch during board inference is therefore
consistent with known weak host validation; it does not automatically mean
NNPA execution failed.

## Scope

This implements only the professor's small-CNN task. It uses three selected
images from the existing own dataset, performs inference, and prints the
classification results. It does not add camera capture, tracking, dashboards,
or other features.
