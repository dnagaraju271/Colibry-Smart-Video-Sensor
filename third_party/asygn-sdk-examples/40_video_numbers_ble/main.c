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
#define CHIP_VDD_VOLTAGE           0.80
#define HFCLK_FREQUENCY_MHZ          40

#define IMAGE_WIDTH                 160
#define IMAGE_HEIGHT                120
#define APB_CLK_DIV                   2

#define BITSTREAM_FLASH_ADDR    0x20000
#define IMGRX_DMA_CHANNEL             1
#define SPIS_DMA_CHANNEL              3

#define APP_RF_DEMO_ID           0x54AC
#define APP_VIDEO_FPS                 4

#define GESTURE_CONFIRMATION_NEEDED   2
#define CAMERA_ONCHIP_MCLK            1

#define CAMERA_TRIGGER_MODE           0

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

volatile uint8_t dma_irq_reached = 0;
volatile uint8_t frame_trigger = 0;
volatile uint8_t image_received = 0;
volatile uint8_t nnpa_inference_done = 0;
volatile uint8_t camera_img_start_dio = CAM_PDWN_IRQ_DIO;

ColibryBoard colibryBoard;


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  IRQ handlers
/////////////////////////////////////////////////////////////////////////////////////////////////////
irq_handler_t dma_irq_handler(void) {
  if (DMA_reg->dma_int_status.word & (1 << SPIS_DMA_CHANNEL)) {
    DMA_reg->dma_int_clear.word = (1 << SPIS_DMA_CHANNEL);
    spis_burst_finalize();
  }
  if (DMA_reg->dma_int_status.word & (1 << IMGRX_DMA_CHANNEL)) {
    DMA_reg->dma_int_clear.word = (1 << IMGRX_DMA_CHANNEL);
    dma_irq_reached = 1;
  }
}

irq_handler_t i2cm_irq_handler(void) {
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

irq_handler_t port_irq_handler() {
  frame_trigger = 1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application functions
/////////////////////////////////////////////////////////////////////////////////////////////////////

/** @brief Processing of NNPA inference results
 *
 */
void process_nnpa_results() {
  static uint32_t nn_output_addr;
  static uint8_t *ptr;
  static uint8_t max_idx, max_2nd_idx;
  static uint8_t max, max_2nd;
  static uint16_t accuracy_est;
  static uint16_t timestamp = 0;
  static uint8_t gesture_class = 0;
  static uint32_t confirm_cnt = 0;
  static uint8_t consecutive_sending = 0;

  NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
  nn_output_addr = NNPA_NBYTE_PER_MEM_WORD * NNPA_reg->ofmap_start_maddr.word + NNPAMEM_BASE_ADDR;
  ptr = (uint8_t *) nn_output_addr;
  //printf("%x\n", NNPA_reg->rd_ofmap_size.word);
  max = ptr[0];
  max_2nd = 0;
  max_idx = 0;
  max_2nd_idx = 0;
  for (int i = 1; i < 7; i++) {
    //printf("%d ", ptr[i]);
    if (ptr[i] > max) {
      max = ptr[i];
      max_2nd_idx = max_idx;
      max_idx = i;
    }
    else if (ptr[i] > max_2nd) {
      max_2nd = ptr[i];
      max_2nd_idx = i;
    }
  }
  
  // Accuracy estimation
  accuracy_est = 75 + ((max - max_2nd) * 25 / 255);
  if (accuracy_est > 99)
    accuracy_est = 99;
      
  // Confirm the gesture
  if (max_idx == gesture_class)
    confirm_cnt++;
  else
    confirm_cnt = 0;
      
  gesture_class = max_idx;

  printf("max: %d (%d %%)\n", max_idx, accuracy_est);
  if (timestamp == 0 || (confirm_cnt >= GESTURE_CONFIRMATION_NEEDED && gesture_class != 6)) {
    if (consecutive_sending % 4 == 0) {
      printf("Gesture detected: %d\n", gesture_class);
      // Send info to the nRF
      colibryBoard.spim0_tx_buff[0] = APP_SPI_RF_BRIDGE_OPCODE;
      colibryBoard.spim0_tx_buff[1] = APP_RF_DEMO_ID & 0xFF;
      colibryBoard.spim0_tx_buff[2] = (APP_RF_DEMO_ID >> 8) & 0xFF;
      colibryBoard.spim0_tx_buff[3] = timestamp++;
      colibryBoard.spim0_tx_buff[4] = gesture_class;
      colibryBoard.spim0_exchange(5);
    }
    consecutive_sending += 1;
  }
  else {
    consecutive_sending = 0;
  }
  
  printf_flush();
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
//  Application main function
/////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
  
  COLIBRY_DRIVERS_INIT();
  
  enum {WAIT_FRAME_TRIGGER, WAIT_IMG_START, RECEIVING_IMG, RUN_INFERENCE, POST_PROC} main_state;
  
  uint32_t nb_sram_buffers_used;
  uint16_t frame_period_ms;
  board_init_cfg_t board_init_cfg;

  // Initialize the board and peripherals
  //
  board_init_cfg.vdd_setting = CHIP_VDD_VOLTAGE;
  board_init_cfg.hfclk_freq_mhz = HFCLK_FREQUENCY_MHZ;
  board_init_cfg.apb_clk_fdiv = APB_CLK_DIV;
  board_init_cfg.nnpa_clk_fdiv = 1;
  board_init_cfg.spis_enable = 1; 
  board_init_cfg.spim_rf_enable = 1;  // Only for iot or Tiny boards
  board_init_cfg.spim_acc_enable = 0;
  board_init_cfg.spis_dma_channel = SPIS_DMA_CHANNEL;
  board_init_cfg.spim0_csn_margin = 300;
  colibryBoard.global_init(board_init_cfg);
  
  /// Below: to NRF_PROTOCOL ??
  
  colibrySCU->regmap->scu_core_msg_0.word = MSG_NO_STREAMING;
  
  // Fixed part of the SPI packets to the nRF
  //
  colibryBoard.spim0_tx_buff[0] = APP_SPI_RF_BRIDGE_OPCODE;
  colibryBoard.spim0_tx_buff[1] = APP_RF_DEMO_ID & 0xFF;
  colibryBoard.spim0_tx_buff[2] = (APP_RF_DEMO_ID >> 8) & 0xFF;
  
  // Request to disable the LFCLK on nRF
  //
  colibryBoard.spim0_tx_buff[0] = APP_SPI_LFCLK_DIS_OPCODE;
  colibryBoard.spim0_exchange(1);

  // Download bitstream from external flash and initialize NNPA
  //
  colibryBoard.copy_bitstream_to_mem(BITSTREAM_FLASH_ADDR, NNPAMEM_BASE_ADDR);
  if (colibryBoard.check_nnpa_bitstream())
    exit(1);
  colibryBoard.nnpa_allocate_buffers();

  // Optimize power consumption
  //
  colibryPORT->regmap->port_lfxo_cfg.word = LFXO_MODE_OFF;
  colibryBoard.extflash_power_disable();
  colibryBoard.sensors_power_disable();
  colibryBoard.configure_lowpower_io();
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
  
  // Blink once after bitstream loaded
  //
  USER_LED_ON();
  colibrySCU->delay_ms(200);
  USER_LED_OFF();
  
  // Camera initialization
  //
  colibryBoard.camera_init(IMAGE_WIDTH, IMAGE_HEIGHT, CAMERA_ONCHIP_MCLK, APP_VIDEO_FPS);
  CAMERA_API_CALL(colibryBoard.camera_model, setImageFlip(VFLIP_ENABLED, HFLIP_DISABLED));
  
  // IRQ enabling
  //
  NNPA_reg->nnpa_int_en.word = EVT_EXEC_END_MASK | EVT_DATA_LOADED_MASK;
  // IRQ to prepare (wake-up) for frame start
  if (colibryBoard.camera_model == EIS316) {
    camera_img_start_dio = CAM_PDWN_IRQ_DIO;
  }
  else if (colibryBoard.camera_model == PAG7920LT) {
    if (CAMERA_TRIGGER_MODE)
      camera_img_start_dio = CAM_PCLK_DIO;
    else
      camera_img_start_dio = CAM_PDWN_IRQ_DIO;
  }
  else {
    camera_img_start_dio = CAM_VSYNC_DIO;
  }
  colibryPORT->dio_cfg_in_pd(camera_img_start_dio);
  
  
  // Start BLE advertising
  //
  colibryBoard.spim0_tx_buff[0] = APP_SPI_ADV_START_OPCODE;
  colibryBoard.spim0_exchange(1);
  
  // Blink again when ready
  //
  colibrySCU->delay_ms(100);
  USER_LED_ON();
  colibrySCU->delay_ms(200);
  USER_LED_OFF();

  printf("---------------------------\n");
  printf("Hand fingers detection Demo\n");
  printf("---------------------------\n");
  printf_flush();
  
  if (CAMERA_TRIGGER_MODE) {
    main_state = WAIT_FRAME_TRIGGER;
    // Enable tick on RF interrupt line
    //
    frame_period_ms = 1000 / APP_VIDEO_FPS;
    colibryBoard.spim0_tx_buff[0] = APP_SPI_RTC_TICK_START_OPCODE;
    colibryBoard.spim0_tx_buff[1] = frame_period_ms & 0xFF;
    colibryBoard.spim0_tx_buff[2] = frame_period_ms >> 8;
    colibryBoard.spim0_exchange(3);
  
    colibryPORT->dio_cfg_in(NRF_INT_DIO);
    colibryPORT->dio_int_clear(NRF_INT_DIO);
    colibryPORT->dio_int_rise_en_set(NRF_INT_DIO);
  }
  else {
    main_state = WAIT_IMG_START;
    colibryBoard.startVideoMode(APP_VIDEO_FPS);
  }
  
  //-----------------------------------------------------------------------------------------------//
  // Main loop
  //-----------------------------------------------------------------------------------------------//
  while (1) {
    if (main_state == WAIT_FRAME_TRIGGER) {
      // All clocks and PLL off, wakeup on tick from nRF
      colibryPORT->dio_out_val_set(CAM_MCLK_SEL_DIO, 1);  // MCLK_SEL high -> EIS316 Sleep
      frame_trigger = 0;
      while (frame_trigger == 0) {
        colibryBoard.enter_low_power(LOW_POWER_CLOCK_OFF_MODE);
      }
      colibryPORT->dio_out_val_set(CAM_MCLK_SEL_DIO, 0);
      CAMERA_API_CALL(colibryBoard.camera_model, trigSingleFrame());
      main_state = WAIT_IMG_START;
    }
    
    else if (main_state == WAIT_IMG_START) {
      // All clocks and PLL off, wakeup image start signal
      colibryPORT->dio_int_clear(camera_img_start_dio);
      colibryPORT->dio_int_rise_en_set(camera_img_start_dio);
      while (colibryPORT->dio_in_val_get(camera_img_start_dio) == 0) {
        colibryBoard.enter_low_power(LOW_POWER_CLOCK_OFF_MODE);
      }
      colibryPORT->dio_int_rise_en_clear(camera_img_start_dio);
      CAMERA_API_CALL(colibryBoard.camera_model, clearFrameStartIrq());
      main_state = RECEIVING_IMG;
    }
    
    else if (main_state == RECEIVING_IMG) {
      // CPU in sleep, AXI frequency decreased to 12MHz
      while (image_received == 0){
        colibryBoard.enter_low_power(LOW_POWER_CLOCK_SLOW_MODE);
      }
      image_received = 0;
      
      // Wait while registers are accessed through SPIS
      while (colibrySCU->regmap->scu_core_msg_0.word == MSG_READ_REGISTER) {
        colibrySCU->delay_ms(10);
      }
        
      // Handshake in case of streaming of images is needed
      if (colibrySCU->regmap->scu_core_msg_0.word == MSG_WAIT_IMAGE_READY) {
        NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
        colibrySCU->regmap->scu_core_msg_0.word = MSG_IMAGE_READY_FOR_DL;
      }

      // Wait that the image is downloaded by the SPI, if case of streaming
      while (colibrySCU->regmap->scu_core_msg_0.word == MSG_IMAGE_READY_FOR_DL) {
        colibrySCU->delay_ms(5);
      }
      main_state = RUN_INFERENCE;
    }
    
    else if (main_state == RUN_INFERENCE) {
      colibrySCU->regmap->scu_periph_clk_en.fields.scu_camif_clk_en = 0;
      colibrySCU->regmap->scu_periph_clk_en.fields.scu_isp_clk_en = 0;
      NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
      NNPA_reg->nnpa_trig_control.word = RUN_START_MASK;
      // CPU in sleep, AXI frequency decreased, NNPA running
      while (nnpa_inference_done == 0){
        colibryBoard.enter_low_power(LOW_POWER_INFERENCE_MODE);
      }
      nnpa_inference_done = 0;
      main_state = POST_PROC;
    }
    
    else if (main_state == POST_PROC) {
      // CPU running at nominal speed
      process_nnpa_results();
      NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
      colibrySCU->regmap->scu_periph_clk_en.fields.scu_camif_clk_en = 1;
      colibrySCU->regmap->scu_periph_clk_en.fields.scu_isp_clk_en = 1;
      if (CAMERA_TRIGGER_MODE) {
        main_state = WAIT_FRAME_TRIGGER;
      }
      else {
        main_state = WAIT_IMG_START;
      }
    }
  }
}
