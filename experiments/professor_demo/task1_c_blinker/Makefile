# Build configuration for the Colibry EvalboardTiny.
# Override COLIBRY_SDK_PATH if the SDK is installed elsewhere.
board ?= EVBTINY
COLIBRY_SDK_PATH ?= $(HOME)/colibryNPU_SDK

APP = blinker
APP_SRCS = $(CURDIR)/main.c
CFLAGS = -O2

include ${COLIBRY_SDK_PATH}/components_drivers/BOARDS/colibry_board.mk
include ${COLIBRY_SDK_PATH}/riscv_runtime/runtime.mk
