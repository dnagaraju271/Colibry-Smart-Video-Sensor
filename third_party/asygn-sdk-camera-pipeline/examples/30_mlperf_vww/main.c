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

#include "nn_data_in.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application settings
/////////////////////////////////////////////////////////////////////////////////////////////////////
#define CHIP_VDD_VOLTAGE            (0.76+0.0125)
//#define CHIP_VDD_VOLTAGE           0.76
#define HFCLK_FREQUENCY_MHZ          40
#define IMAGE_WIDTH                 160
#define IMAGE_HEIGHT                120
#define APB_CLK_DIV                   4
#define I2C_SW_HALF_PERIOD_US         2

#define BITSTREAM_FLASH_ADDR    0x100000

#define IMGRX_DMA_CHANNEL             1
#define SPIS_DMA_CHANNEL              3

#define APP_SPI_RF_BRIDGE_OPCODE   0x9B
#define APP_SPI_LFCLK_EN_OPCODE    0x11
#define APP_SPI_LFCLK_DIS_OPCODE   0x20
#define APP_RF_DEMO_ID           0x54AC


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Static/extern variables
/////////////////////////////////////////////////////////////////////////////////////////////////////
enum app_messages {MSG_NO_STREAMING = 0,
                   MSG_READ_REGISTER = 3,
                   MSG_WAIT_IMAGE_READY = 5,
                   MSG_IMAGE_READY_FOR_DL = 10};

extern const int __sram_user_start;
extern const int __sram_buffer_size;

const int sram_buffers_size = (int)&__sram_buffer_size;

volatile uint8_t nnpa_inference_done = 0;


ColibryBoard colibryBoard;


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  IRQ handlers
/////////////////////////////////////////////////////////////////////////////////////////////////////
irq_handler_t nnpa_irq_handler(void) {
    nnpa_inference_done = 1;
    NNPA_reg->nnpa_int_clear.word = EVT_EXEC_END_MASK;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application main function
/////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {

  uint32_t nb_sram_buffers_used;
  board_init_cfg_t board_init_cfg;
  uint32_t input_data_word;
  uint32_t nn_output_addr;
  uint8_t *ptr;
  uint32_t inference_counter = 0;
  
  
  //-----------------------------------------------------------------------------------------------//
  // Initializations
  //-----------------------------------------------------------------------------------------------//
  COLIBRY_DRIVERS_INIT();
  colibrySCU->regmap->scu_periph_clk_en.word = 0xFFFFFFFF;


  // Initialize the board and peripherals
  //
  board_init_cfg.vdd_setting = CHIP_VDD_VOLTAGE;
  board_init_cfg.hfclk_freq_mhz = HFCLK_FREQUENCY_MHZ;
  board_init_cfg.apb_clk_fdiv = APB_CLK_DIV;
  board_init_cfg.nnpa_clk_fdiv = 1;
  board_init_cfg.spis_enable = 0; 
  board_init_cfg.spim_rf_enable = 1;
  board_init_cfg.spim_acc_enable = 0;
  board_init_cfg.spis_dma_channel = SPIS_DMA_CHANNEL;
  board_init_cfg.spim0_csn_margin = 100;
  colibryBoard.global_init(board_init_cfg);

  printf("---------------------------\n");
  printf("VWW benchmark              \n");
  printf("---------------------------\n");
  
  // Fixed part of the SPI packets to the nRF
  //
  colibryBoard.spim0_tx_buff[0] = APP_SPI_RF_BRIDGE_OPCODE;
  colibryBoard.spim0_tx_buff[1] = APP_RF_DEMO_ID & 0xFF;
  colibryBoard.spim0_tx_buff[2] = (APP_RF_DEMO_ID >> 8) & 0xFF;
  
  // Request to disable the LFCLK on nRF
  //
  colibryBoard.spim0_tx_buff[0] = APP_SPI_LFCLK_DIS_OPCODE;
  colibryBoard.spim0_exchange(1);
  colibryBoard.spim0_tx_buff[0] = APP_SPI_RF_BRIDGE_OPCODE;  // Set back default opcode

  // Download bitstream from external flash and initialize NNPA
  //
  colibryBoard.copy_bitstream_to_mem(BITSTREAM_FLASH_ADDR, NNPAMEM_BASE_ADDR);
  if (colibryBoard.check_nnpa_bitstream())
    exit(1);
  colibryBoard.nnpa_allocate_buffers();
  
  // Blink once after bitstream loaded
  //
  USER_LED_ON();
  colibrySCU->delay_ms(200);
  USER_LED_OFF();
  
  // IRQ enabling
  //
  NNPA_reg->nnpa_int_en.word = EVT_EXEC_END_MASK;

  // Input data loading through the INPUT LOADER
  //
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
  NNPA_reg->input_loader_cfg.word = (3 << LOADER_NBYTE_VALID_OFFSET) |
                                    (0 << LOADER_NBIT_MODE_OFFSET) |
                                    (0 << LOADER_SIGNED_DATA_OFFSET);
  NNPA_reg->nnpa_trig_control.word = INPUT_LOADER_RST_MASK;

  for (int i = 0; i < N_SAMPLE_INPUT / 4; i++) {
    //input_data_word = (input_data[0][i*4 + 3] << 24) | (input_data[0][i*4 + 2] << 16) |
    //                  (input_data[0][i*4 + 1] << 8) | input_data[0][i*4];
    input_data_word = * (uint32_t *) (input_data[0] + i*4);
    
    NNPA_reg->input_loader_data.word = input_data_word;
  }

  // Blink again when ready
  //
  colibrySCU->delay_ms(100);
  USER_LED_ON();
  colibrySCU->delay_ms(200);
  USER_LED_OFF();

  // Optimize power consumption
  //
  // Keep only needed peripheral clocks
  colibrySCU->regmap->scu_periph_clk_en.word = SCU_APB_CLK_EN_MASK | SCU_DMA_CLK_EN_MASK |
    SCU_UART_CLK_EN_MASK | SCU_CAMIF_CLK_EN_MASK | SCU_ISP_CLK_EN_MASK | SCU_DBGM_CLK_EN_MASK |
    SCU_NNPA_CLK_EN_MASK | SCU_PORT_CLK_EN_MASK | SCU_SPIS_CLK_EN_MASK | SCU_SPIM0_CLK_EN_MASK;
  // Powering down some unused SRAM and NNPA memory buffers
  nb_sram_buffers_used = ((uint32_t) &__sram_user_start - 0x10000000) / sram_buffers_size + 1;
  colibrySCU->regmap->sram_cuts_pd_ctrl.word = (uint32_t) ~((1 << nb_sram_buffers_used) - 1);
  //NNPA_reg->nnpa_control.fields.manual_buff_pd = 1;
  //NNPA_reg->mem_buff_pd0.word = 0xFFFFFFFF;
  //NNPA_reg->mem_buff_pd1.word = 0xFFFFFFFF;

  
  colibryBoard.extflash_power_disable();
  colibryBoard.sensors_power_disable();
  colibryBoard.configure_lowpower_io();
  
  //colibryBoard.enter_low_power(LOW_POWER_CLOCK_OFF_MODE);
  
  //-----------------------------------------------------------------------------------------------//
  // Main loop
  //-----------------------------------------------------------------------------------------------//
  while (1) {
    /// Start inference
    USER_LED_OFF();
    NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
    NNPA_reg->nnpa_trig_control.word = RUN_START_MASK;
    printf_flush();
    colibryBoard.enter_low_power(LOW_POWER_INFERENCE_MODE);
    //wait_for_interrupt();
    
    inference_counter++;

    // Comment the lines below to optimize average power
    if (inference_counter % 100 == 0) {
      printf("%d inferences run\n", inference_counter);
    }
    
    USER_LED_ON();
    
    colibrySCU->delay_us(1);
  
    /// Get outputs
    NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
    nn_output_addr = NNPA_NBYTE_PER_MEM_WORD * NNPA_reg->ofmap_start_maddr.word + NNPAMEM_BASE_ADDR;
    
    ptr = (uint8_t *) nn_output_addr;
    if (ptr[0] != 41 || ptr[1] != 149) {
      //printf("Outputs: %d %d\n", ptr[0], ptr[1]);
      USER_LED_OFF();
      printf("ERROR: incorrect outputs\n");
      return 1;
    }
  }
  return 0;
}
