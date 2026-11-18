# Colibry EvalboardTiny SRAM inference firmware

This directory contains the board-specific firmware for testing the team's own 3-class CNN on the Colibry NNPA. The model is trained on the Ubuntu host; the NNPA executes inference only.

## Firmware contents

- `main.c` — embeds and copies the NNPA model into NNPA SRAM, loads three test images, runs inference and prints results over UART.
- `generate_board_data.py` — verifies the NNPA artifact size/SHA-256 and creates a C header containing the NNPA words and three labelled input samples.
- `Makefile` — compiles the firmware for `EVBTINY`.

## Inputs

Three samples from the canonical dataset archive:
- `1_person/I1__008__120x160.png`
- `2_person/I2__007__120x160.png`
- `3_person/I3__004__120x160.png`

Preprocessing: grayscale, Lanczos resize from 160x120 to 40x30, 1200 uint8 bytes per image.

## Build

From this directory:

```bash
make prepare-data
make check-inputs
make all board=EVBTINY
```

The build completed on 2026-11-18. Generated firmware binary SHA-256:
`5d1fe38aca93bdda2c7fcd948d354b4cef22f1a704b8b30d5288d5631d150196`.

## Run from SRAM only

Use the Evaluation Kit manual's Wait Start sequence:
1. Press and hold `PWR_OFF` on ColibryProbe.
2. Press and hold the EvalboardTiny User Button.
3. Release `PWR_OFF`.
4. Release the User Button.

UART console: `/dev/ttyUSB2`, 115200 baud. Configure and start capture before launching the loader:

```bash
stty -F /dev/ttyUSB2 115200 raw -echo -ixon -ixoff cs8 -cstopb -parenb
cat /dev/ttyUSB2 > build/uart_own_count_cnn.log &
UART_PID=$!
sleep 2
```

In this Ubuntu VM, system `python3` lacks PyFtdi. Invoke the SDK SRAM loader with the flash environment's Python, which has PyFtdi 0.57.2:

```bash
sudo "$HOME/colibry-flash-env/bin/python" \
  "$HOME/colibryNPU_SDK/riscv_runtime/tools/bin_loader/colibry-sramtool.py" \
  "$PWD/build/own_count_cnn_sram_demo.bin" \
  0x10000000 --run \
  -p "ftdi://ftdi:4232hp:5:2/1"
```

After allowing the app to finish, stop capture and inspect:

```bash
sleep 30
kill "$UART_PID" 2>/dev/null || true
wait "$UART_PID" 2>/dev/null || true
cat build/uart_own_count_cnn.log
```

**Do not use `toflash`, `make nnpa`, or the flash programming tool for this demonstration.** The SRAM loader writes the firmware to volatile SRAM at `0x10000000`; the app copies the embedded NNPA into NNPA SRAM. It does not access or modify external flash sector `0x20000`.

## Hardware test result — 2026-11-18

The physical board successfully loaded the NNPA (model ID `0x723E`) and completed all three inference calls. Input size register was 75 x 16-byte words = 1200 bytes; output shape was `[1,1,3]`.

| Sample | Expected | Board prediction |
|---|---|---|
| 1_person | 1_person | 1_person — match |
| 2_person | 2_person | 1_person — mismatch |
| 3_person | 3_person | 2_person — mismatch |

Host INT8 TFLite and board predictions matched on the same three images. Raw uint8 outputs differed by at most 11 levels, so bit-exact output parity is not established. Only one of the three labels was classified correctly. This is a successful hardware execution demonstration, not a reliable people counter.

After the app completes, it loops indefinitely and the LED remains off. A normal power cycle/boot returns to the flash-resident firmware. The external flash was not changed.

## Known model limitation

The dataset currently has 59 one-person, 8 two-person and 4 three-person samples. All four three-person images belong to one source group. The previous validation split leaks source groups, and recall was zero for the 2-person and 3-person validation examples. Collect more independent 2-person and 3-person images before retraining and claiming useful counting accuracy.

