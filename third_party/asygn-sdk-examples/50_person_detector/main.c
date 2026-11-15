/*
 * SPDX-License-Identifier: LicenseRef-Asygn-BSD3-NoReverse-NoExternalUse
 *
 * Copyright (c) 2026 Asygn SAS
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Asygn SAS
 *    integrated circuit in a product or a software update for such product, must
 *    reproduce the above copyright notice, this list of conditions and the
 *    following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of Asygn SAS nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific
 *    prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a Asygn
 *    SAS integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY ASYGN SAS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ASYGN SAS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdio.h>
#include "chip.h"
#include "colibry_drivers.h"
#include "colibry_board.hpp"

/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application settings
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIP_VDD_VOLTAGE            0.9

#define HFCLK_FREQUENCY_MHZ          40

#define APB_CLK_DIV                   2
#define I2C_SW_HALF_PERIOD_US         2
#define BITSTREAM_FLASH_ADDR    0x20000
#define IMGRX_DMA_CHANNEL             1
#define SPIS_DMA_CHANNEL              3


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Static/extern variables
/////////////////////////////////////////////////////////////////////////////////////////////////////
enum app_messages {MSG_IDLE = 0,
                   MSG_WAIT_INFERENCE = 5,
                   MSG_RUNNING = 10};



volatile uint8_t dma_irq_reached = 0;
volatile uint8_t image_start = 0;
volatile uint8_t image_received = 0;
volatile uint8_t nnpa_inference_done = 0;
volatile uint8_t spis_dma_finalize_pending = 0;
volatile uint32_t spis_dma_error_status = 0;

ColibryBoard colibryBoard;
extern volatile uint32_t spis_dma_rx_drain_count;


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  IRQ handlers
/////////////////////////////////////////////////////////////////////////////////////////////////////
irq_handler_t dma_irq_handler(void) {
  uint32_t status = DMA_reg->dma_int_status.word;
  uint32_t done = (1u << SPIS_DMA_CHANNEL);
  uint32_t read_error = (1u << (DMA_EVT_READ_ERROR_OFFSET + SPIS_DMA_CHANNEL));
  uint32_t write_error = (1u << (DMA_EVT_WRITE_ERROR_OFFSET + SPIS_DMA_CHANNEL));
  uint32_t errors = read_error | write_error;
  uint32_t events = status & (done | errors);

  if (events != 0u) {
    DMA_reg->dma_int_clear.word = events;
    spis_dma_error_status |= events & errors;
    spis_dma_finalize_pending = 1;
  }
  if (status & (1u << IMGRX_DMA_CHANNEL)) {
    DMA_reg->dma_int_clear.word = (1u << IMGRX_DMA_CHANNEL);
    dma_irq_reached = 1;
  }
}

irq_handler_t nnpa_irq_handler(void) {
  if (NNPA_reg->nnpa_int_status.fields.evt_data_loaded) {
    image_received = 1;
    NNPA_reg->nnpa_int_clear.word = EVT_DATA_LOADED_MASK;
  }
  else if (NNPA_reg->nnpa_int_status.fields.evt_exec_end) {    
    nnpa_inference_done = 1;
    NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application functions
/////////////////////////////////////////////////////////////////////////////////////////////////////
static void service_spis_dma_finalize(void) {
  if (!spis_dma_finalize_pending) return;

  uint32_t errors = spis_dma_error_status;
  spis_dma_finalize_pending = 0;
  spis_dma_error_status = 0;
  spis_burst_finalize();

  if (errors != 0u)
    printf("SPIS DMA error event bits: 0x%08lx\n", (unsigned long)errors);
  if (spis_burst_finalize_timed_out)
    printf("ERROR: SPIS burst finalization timed out\n");
}

void process_nnpa_results()
{
  static uint32_t nn_output_addr;
  static uint8_t *ptr;
  
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
  nn_output_addr = NNPA_NBYTE_PER_MEM_WORD * NNPA_reg->ofmap_start_maddr.word + NNPAMEM_BASE_ADDR;
  ptr = (uint8_t *) nn_output_addr;
  //printf("%x\n", NNPA_reg->rd_ofmap_size.word);
  printf("prediction %d\n", ptr[0]);
  colibrySCU->regmap->scu_core_msg_1.word = ptr[0];
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application main function
/////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
  
  enum {WAIT_IMG_START, RECEIVING_IMG, RUN_INFERENCE, POST_PROC} main_state;
  
  //-----------------------------------------------------------------------------------------------//
  // Initializations
  //-----------------------------------------------------------------------------------------------//
  COLIBRY_DRIVERS_INIT();

  board_init_cfg_t board_init_cfg;

  // Initialize the board and peripherals
  //
  board_init_cfg.vdd_setting = CHIP_VDD_VOLTAGE;
  board_init_cfg.hfclk_freq_mhz = HFCLK_FREQUENCY_MHZ;
  board_init_cfg.apb_clk_fdiv = APB_CLK_DIV;
  board_init_cfg.nnpa_clk_fdiv = 1;
  board_init_cfg.spis_enable = 1; 
  board_init_cfg.spim_rf_enable = 1;
  board_init_cfg.spim_acc_enable = 0;
  board_init_cfg.spis_dma_channel = SPIS_DMA_CHANNEL;
  board_init_cfg.spim0_csn_margin = 100;
  colibryBoard.global_init(board_init_cfg);

  // Download bitstream from external flash and initialize NNPA
  //
  colibryBoard.copy_bitstream_to_mem(BITSTREAM_FLASH_ADDR, NNPAMEM_BASE_ADDR);
  if (colibryBoard.check_nnpa_bitstream())
    exit(1);
  colibryBoard.nnpa_allocate_buffers();

  // Temporary diagnostics for the YOLO backbone integration.
  printf("\n========== NNPA MEMORY DIAGNOSTICS ==========\n");
  printf("rd_model_size: %u\n", NNPA_reg->rd_model_size.word);
  printf("rd_input_size: %u\n", NNPA_reg->rd_input_size.word);
  printf("rd_data_size: %u\n", NNPA_reg->rd_data_size.word);
  printf("rd_ifmap_size: 0x%08x\n", NNPA_reg->rd_ifmap_size.word);
  printf("rd_ofmap_size: 0x%08x\n", NNPA_reg->rd_ofmap_size.word);
  printf("rd_ofmap_info: 0x%08x\n", NNPA_reg->rd_ofmap_info.word);
  printf("input_start_maddr: %u\n", NNPA_reg->input_start_maddr.word);
  printf("data_start_maddr: %u\n", NNPA_reg->data_start_maddr.word);
  printf("ofmap_start_maddr: %u\n", NNPA_reg->ofmap_start_maddr.word);
  printf("============================================\n\n");




  NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
  NNPA_reg->nnpa_int_en.word = EVT_EXEC_END_MASK;

  printf("System ready. Waiting for SPI input\n");

  while (1) {
    service_spis_dma_finalize();
    for (volatile uint32_t diag_delay = 0;
         diag_delay < 6000000; ++diag_delay) {
      NOP();
    }

    printf("DIAG_HEARTBEAT model=%u input=%u data=%u "
           "ifmap=0x%08x ofmap=0x%08x "
           "input_addr=%u data_addr=%u output_addr=%u "
           "SPIS_DMA_RX_DRAIN_HEARTBEAT rx_drained=%u\n",
           NNPA_reg->rd_model_size.word,
           NNPA_reg->rd_input_size.word,
           NNPA_reg->rd_data_size.word,
           NNPA_reg->rd_ifmap_size.word,
           NNPA_reg->rd_ofmap_size.word,
           NNPA_reg->input_start_maddr.word,
           NNPA_reg->data_start_maddr.word,
           NNPA_reg->ofmap_start_maddr.word,
           spis_dma_rx_drain_count);

    if (colibrySCU->regmap->scu_core_msg_0.word == MSG_WAIT_INFERENCE) {

      colibrySCU->regmap->scu_core_msg_0.word = MSG_RUNNING;
      nnpa_inference_done = 0;
      NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;
      NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
      NNPA_reg->nnpa_trig_control.word = RUN_START_MASK;

      while (!nnpa_inference_done) {
        service_spis_dma_finalize();
      }
      process_nnpa_results();
      colibrySCU->regmap->scu_core_msg_0.word = MSG_IDLE;
    }
  }
  
  return 0;
}


