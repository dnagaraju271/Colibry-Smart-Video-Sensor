uart_printf ?= 1
board ?= EVBTINY

COLIBRY_SDK_PATH ?= $(CURDIR)/../..

APP = demo
APP_SRCS = $(CURDIR)/main.c

CFLAGS = -O2

include ${COLIBRY_SDK_PATH}/components_drivers/BOARDS/colibry_board.mk
include ${COLIBRY_SDK_PATH}/riscv_runtime/runtime.mk

bitstream_addr = 0x100000

custom: 
	@echo	"***********************************************************************"
	@echo	${USR_MSG}
	@echo	"***********************************************************************"
