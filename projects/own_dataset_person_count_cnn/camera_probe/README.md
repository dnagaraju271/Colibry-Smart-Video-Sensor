# Colibry camera sensor detection probe

This is the first live-camera bring-up step for the own-dataset person-count CNN.

## Purpose

- Initialize Colibry EvalboardTiny.
- Enable the board camera sensor power rail.
- Run the ASYGN SDK camera auto-detection for EIS316, HM0360 and PAG7920LT.
- Print the detected sensor through UART.

This probe does **not** capture an image, run NNPA inference, train a model, or access/modify external flash.

## Hardware

Connect a supported camera module to the appropriate EvalboardTiny camera connector. The evaluation kit manual describes the PAG7920-family camera FPC and the Arducam daughterboard connector. If no supported module is attached or powered, the SDK prints that no camera was detected.

## Build on Ubuntu

From the repository root:

```bash
cd projects/own_dataset_person_count_cnn/camera_probe
make all board=EVBTINY
```

The Makefile uses the already-installed local SDK at `~/colibryNPU_SDK`.

## Run from SRAM only

Use the Evaluation Kit's Wait Start sequence (hold ColibryProbe PWR_OFF, hold EvalboardTiny User Button, release PWR_OFF, release the User Button).

UART is `/dev/ttyUSB2` at 115200 baud. Start UART capture, then load the generated SRAM binary. The exact binary location is printed by the build output; normally it is `build/colibry_camera_sensor_probe.bin`.

```bash
# From this camera_probe directory:
stty -F /dev/ttyUSB2 115200 raw -echo -ixon -ixoff cs8 -cstopb -parenb
cat /dev/ttyUSB2 > build/camera_probe_uart.log &
UART_PID=$!
sleep 2

# Use the exact .bin path produced by make:
sudo "$HOME/colibry-flash-env/bin/python" \
  "$HOME/colibryNPU_SDK/riscv_runtime/tools/bin_loader/colibry-sramtool.py" \
  "$PWD/build/colibry_camera_sensor_probe.bin" 0x10000000 --run \
  -p "ftdi://ftdi:4232hp:5:2/1"

# After the result is printed:
sleep 3
kill "$UART_PID" 2>/dev/null || true
wait "$UART_PID" 2>/dev/null || true
cat build/camera_probe_uart.log
```

The loader writes to volatile SRAM only. Do not use `toflash`, `make nnpa`, or the flash loader for this probe.

## Expected successful output

```text
CAMERA_DETECTION_RESULT: PAG7920LT
CAMERA_INITIALIZATION: SUCCESS
FRAME_CAPTURE: NOT STARTED
NNPA_INFERENCE: NOT STARTED
EXTERNAL_FLASH: UNTOUCHED
```

The sensor name depends on the actual connected module.

## After detection

If detection succeeds, the next step is a one-frame SRAM test and verification of the camera/ISP output. The current CNN needs exactly 40x30 grayscale uint8 (1200 bytes); the camera pipeline must be configured to produce that input correctly before NNPA inference is integrated.
