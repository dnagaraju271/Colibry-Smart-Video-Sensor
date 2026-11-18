uart_printf ?= 1
board ?= EVBTINY

COLIBRY_SDK_PATH ?= $(HOME)/colibryNPU_SDK

APP = own_count_camera_live
APP_SRCS = $(CURDIR)/main.c
CFLAGS += -O2

NNPA_MODEL ?= $(HOME)/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa
GENERATED_HEADER = $(CURDIR)/build/own_count_nnpa_data.h

.PHONY: prepare-model
prepare-model:
	python3 "$(CURDIR)/generate_nnpa_header.py" \
		--nnpa "$(NNPA_MODEL)" \
		--output "$(GENERATED_HEADER)"

include $(COLIBRY_SDK_PATH)/components_drivers/BOARDS/colibry_board.mk
include $(COLIBRY_SDK_PATH)/riscv_runtime/runtime.mk
