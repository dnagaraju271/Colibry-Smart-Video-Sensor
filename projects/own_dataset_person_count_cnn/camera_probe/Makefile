uart_printf ?= 1
board ?= EVBTINY

COLIBRY_SDK_PATH ?= $(HOME)/colibryNPU_SDK

APP = colibry_camera_sensor_probe
APP_SRCS = $(CURDIR)/main.c
CFLAGS += -O2

include $(COLIBRY_SDK_PATH)/components_drivers/BOARDS/colibry_board.mk
include $(COLIBRY_SDK_PATH)/riscv_runtime/runtime.mk
