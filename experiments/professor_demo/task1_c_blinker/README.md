# Task 1 — C LED blinker

A minimal C LED-blinker application for the **ASYGN Colibry EvalboardTiny**.

## Board configuration

- SDK board selection: `EVBTINY`
- LED control: SDK-provided `USER_LED_ON()` and `USER_LED_OFF()` macros
- The Evalboard LED is active-low (ON drives the LED signal LOW).
- Blink interval: 300 ms ON, 300 ms OFF.

The source follows the initialization and LED-control calls used by the SDK example `examples/10_uart_printf_blink`, but removes the UART print loop so the demonstration focuses only on the LED.

## Files

- `main.c` — minimal application
- `Makefile` — build configuration

## Build

The Makefile expects the SDK at `~/colibryNPU_SDK` by default. From this directory:

```bash
make
```

If the SDK is installed elsewhere:

```bash
make COLIBRY_SDK_PATH=/path/to/colibryNPU_SDK
```

## Hardware test

Build the application using the SDK toolchain, program the generated application image to the Colibry EvalboardTiny using the normal board flashing procedure, and confirm that the red user LED blinks at approximately 1.7 cycles per second.

**Status:** Source and Makefile committed. The files still need to be built and tested from this repository checkout on the target board before the task can be marked hardware-verified.
