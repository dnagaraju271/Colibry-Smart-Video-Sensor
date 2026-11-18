uart_printf ?= 1
board ?= EVBTINY

COLIBRY_SDK_PATH ?= $(HOME)/colibryNPU_SDK
ROOT_DIR := $(abspath $(CURDIR)/../../..)

APP = own_count_cnn_sram_demo
APP_SRCS = $(CURDIR)/main.c
CFLAGS += -O2

PYTHON ?= $(HOME)/colibry-cnn-env/bin/python
NNPA ?= $(HOME)/models/edgeguard_own_count/ptq/own_count_cnn_uint8.tflite.nnpa
DATASET_ARCHIVE ?= $(ROOT_DIR)/datasets/mobile_160x120/OWN_Dataset_160x120_Grayscale_processed_only.zip
GENERATED_HEADER = $(CURDIR)/build/own_count_board_data.h
BATCH_START ?= 0
BATCH_SIZE ?= 16

include $(COLIBRY_SDK_PATH)/components_drivers/BOARDS/colibry_board.mk
include $(COLIBRY_SDK_PATH)/riscv_runtime/runtime.mk

.PHONY: prepare-data
prepare-data:
	mkdir -p $(CURDIR)/build
	$(PYTHON) $(CURDIR)/generate_board_data.py \
		--nnpa "$(NNPA)" \
		--archive "$(DATASET_ARCHIVE)" \
		--batch-start "$(BATCH_START)" \
		--batch-size "$(BATCH_SIZE)" \
		--output "$(GENERATED_HEADER)"

.PHONY: check-inputs
check-inputs:
	test -f "$(NNPA)" || (echo "Missing NNPA: $(NNPA)"; exit 1)
	test -f "$(DATASET_ARCHIVE)" || (echo "Missing dataset archive: $(DATASET_ARCHIVE)"; exit 1)
	test -f "$(GENERATED_HEADER)" || (echo "Run make prepare-data first"; exit 1)
