// SPDX-License-Identifier: LicenseRef-Asygn-BSD3-NoReverse-NoExternalUse
//
// Copyright (c) 2026 Asygn SAS
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form, except as embedded into a Asygn SAS
//    integrated circuit in a product or a software update for such product, must
//    reproduce the above copyright notice, this list of conditions and the
//    following disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
// 3. Neither the name of Asygn SAS nor the names of its contributors may be used
//    to endorse or promote products derived from this software without specific
//    prior written permission.
//
// 4. This software, with or without modification, must only be used with a Asygn
//    SAS integrated circuit.
//
// 5. Any software provided in binary form under this license must not be reverse
//    engineered, decompiled, modified and/or disassembled.
//
// THIS SOFTWARE IS PROVIDED BY ASYGN SAS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL ASYGN SAS OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
// GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "colibry_board.hpp"


ColibryBoard::ColibryBoard() {
}

void ColibryBoard::global_init(board_init_cfg_t board_init_cfg) {
  this->board_init_cfg = board_init_cfg;
  colibrySCU->set_hf_clk_freq(INITIAL_HFCLK_FREQ * 1000000);

  i2cm_sw = I2CmSw(I2C_SW_HALF_PERIOD_US);
  tps6286x = Tps6286x(&i2cm_sw);
  cameraEIS316 = CameraEIS316(&i2cm_sw);
  cameraHM0360 = CameraHM0360(&i2cm_sw);
  cameraPAG7920LT = CameraPAG7920LT(&i2cm_sw);
  
  // I2C DIO for software I2C
  //
  colibryPORT->dio_cfg_in(I2C_SDA_DIO);
  colibryPORT->dio_cfg_none(I2C_SCL_DIO);

  // VDD voltage
  //
  //colibrySCU->regmap->pll_cal_bypass.word = 0x0001;  // Use lowest internal frequency
  float vdd = tps6286x.set_vo1_voltage(board_init_cfg.vdd_setting);
  colibrySCU->delay_ms(board_init_cfg.setup_delay);

  // LF clock
  //
  colibryPORT->regmap->port_lfxo_cfg.word = LFXO_MODE_EXT_CLOCK;
  
  // HF_CLK clock
  //
#if defined(__BOARD_IOT_REV0)
  colibryPORT->regmap->port_hfxo_cfg.word = HFXO_MODE_EXT_CLOCK;
  colibrySCU->configure_hf_clk(HFCLK_PLL_OPENLOOP_CALIBRATED,
                               0,
                               16384,
                               board_init_cfg.vdd_setting,
                               board_init_cfg.hfclk_freq_mhz);

  colibrySCU->delay_ms(100);  // Wait for the end of calibration
  colibryPORT->regmap->port_hfxo_cfg.word = HFXO_MODE_OFF;
  
#elif defined(__BOARD_EVBTINY_REV0)
  colibryPORT->dio_cfg_out(EXT_HFCLK_EN_DIO);
  colibryPORT->dio_out_val_set(EXT_HFCLK_EN_DIO, 0);
  colibryPORT->regmap->port_hfxo_cfg.word = HFXO_MODE_EXT_CLOCK;
  colibrySCU->configure_hf_clk(HFCLK_PLL_OPENLOOP_CALIBRATED,
                               0,
                               16384,
                               board_init_cfg.vdd_setting,
                               board_init_cfg.hfclk_freq_mhz);

  colibrySCU->delay_ms(100);  // Wait for the end of calibration
  colibryPORT->regmap->port_hfxo_cfg.word = HFXO_MODE_OFF;
#elif defined(__BOARD_EVBTINY_REV1)
  colibryPORT->dio_cfg_out(EXT_HFCLK_EN_DIO);
  colibryPORT->dio_out_val_set(EXT_HFCLK_EN_DIO, 1);
  colibryPORT->regmap->port_hfxo_cfg.word = HFXO_MODE_EXT_CLOCK;
  colibrySCU->delay_ms(200);
  colibrySCU->configure_hf_clk(HFCLK_PLL_OPENLOOP_CALIBRATED,
                               0,
                               8000000,
                               board_init_cfg.vdd_setting,
                               board_init_cfg.hfclk_freq_mhz);

  colibrySCU->delay_ms(100);  // Wait for the end of calibration
  colibryPORT->dio_out_val_set(EXT_HFCLK_EN_DIO, 0);
  colibryPORT->regmap->port_hfxo_cfg.word = HFXO_MODE_OFF;
#else
  colibrySCU->configure_hf_clk(HFCLK_PLL_OPENLOOP_UNCALIBRATED,
                               0,
                               0,
                               board_init_cfg.vdd_setting,
                               board_init_cfg.hfclk_freq_mhz);

  colibrySCU->regmap->pll_cal_bypass.word = 0x1071;   // Depends on board and voltage
  colibrySCU->regmap->pll_control.fields.pll_post_div = 0x01;
#endif
  
  colibrySCU->delay_ms(100);
  
  // Derived clocks
  //
  colibrySCU->regmap->scu_apb_clk_cfg.word = board_init_cfg.apb_clk_fdiv - 1;
  colibrySCU->regmap->scu_nnpa_clk_cfg.word = board_init_cfg.nnpa_clk_fdiv - 1;

  // Improve low-VDD robustness
  //
  colibryNNPA->regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_rm = 1;

  
  // Printf over UART
  //
#if defined(UART_PRINTF_ENABLED) && UART_PRINTF_ENABLED == 1
  init_uart_printf(UART_RX_DIO, UART_TX_DIO, colibrySCU->get_apb_clk_freq(), UART_PRINTF_BAUDRATE);
#endif
  
  // Reduce I/O power
  //
  colibryPORT->dio_cfg_none(1);
  colibryPORT->dio_cfg_none(CAM_MCLK_DIO);
 

  // Board user led
  //
  ENABLE_USER_LED_DIO();
  USER_LED_OFF();

  // Reset some peripherals before starting
  //
  colibrySCU->regmap->scu_periph_reset.word = SCU_ISP_RESET_MASK | SCU_CAMIF_RESET_MASK | SCU_NNPA_RESET_MASK;
  colibrySCU->regmap->scu_periph_reset.word = 0;

  // SPIS external interface
  //
  if (board_init_cfg.spis_enable) {
    spis_init(board_init_cfg.spis_dma_channel,
              SPIS_SCK_DIO,
              SPIS_MISO_DIO,
              SPIS_MOSI_DIO,
              SPIS_CSN_DIO);
    
    colibryPORT->dio_pull_res_en(SPIS_CSN_DIO, PULL_CFG_HIGH);
    colibryPORT->dio_pull_res_en(SPIS_SCK_DIO, PULL_CFG_LOW);
    colibryPORT->dio_pull_res_en(SPIS_MOSI_DIO, PULL_CFG_LOW);
  }

  
#if defined(__BOARD_IOT_REV0) || defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  // Interface to LIS2DW12 accelerometer
  //
  if (board_init_cfg.spim_acc_enable) {
    lis2dw12.init_io(SPIM_ACC_SCK_DIO,
                     SPIM_ACC_MISO_DIO,
                     SPIM_ACC_MOSI_DIO,
                     SPIM_ACC_CSN_DIO,
                     SPIM_ACC_CSN_INDEX,
                     SPIM_ACC_PERIPHERAL);
  }

  // SPIM_RF -- interface to nRF
  //
  if (board_init_cfg.spim_rf_enable) {
    colibrySPIM0->spim_set_config(SPIM_RF_SCK_DIO,
                                  SPIM_RF_MISO_DIO,
                                  SPIM_RF_MOSI_DIO,
                                  SPIM_RF_CSN_DIO,
                                  SPIM_RF_CSN_INDEX,
                                  0,        // cpol
                                  0,        // cpha
                                  0,        // tx_only
                                  4000000   // sck frequency (8 MHz max for nRF)
                                  );
  }
#endif

  // Disable sensors power by default
  //
  sensors_power_disable(); 
  
  if (!board_init_cfg.no_printf){
    printf("VDD = %f\n", vdd);
  }
}

void ColibryBoard::camera_init(uint16_t image_width, uint16_t image_height, uint8_t onchip_mclk, uint8_t expected_video_fps) {
  CAMIF_reg->camif_protocol_cfg.fields.data_in_mode = 0;
  CAMIF_reg->camif_protocol_cfg.fields.pclk_sampling_cfg = 1;
  CAMIF_reg->camif_img_size_cfg.fields.img_width = image_width;
  CAMIF_reg->camif_img_size_cfg.fields.img_height = image_height;
  colibryCAMIF->data_in_mode_set(CAMIF_DATA_IN_MODE_RAW);
  CAMIF_reg->camif_cdc_fifo_bypass.word = 1;
  
  ENABLE_CAMERA_DIO();
  colibrySCU->delay_ms(1);
  
  // Detect the camera model used
  if (cameraEIS316.checkVersion() == cameraEIS316.VERSION_OK) {
    printf("Camera EIS316 detected\n");
    camera_model = EIS316;
  }
  else if (cameraHM0360.checkVersion() == cameraHM0360.VERSION_OK) {
    printf("Camera HM0360 detected\n");
    camera_model = HM0360;
    colibryPORT->dio_cfg_in_pu(CAM_PDWN_IRQ_DIO);
  }
  else if (cameraPAG7920LT.checkVersion() == cameraPAG7920LT.VERSION_OK) {
    printf("Camera PAG7920LT detected\n");
    camera_model = PAG7920LT;
  }
  else {
    printf("ERROR: no camera detected. Exiting\n");
    exit(1);
  }

  // Configure MCLK_SEL. If high, external clock (12 MHz) shall be provided on MCLK
  colibryPORT->dio_cfg_out(CAM_MCLK_SEL_DIO);
  if (onchip_mclk) {
    colibryPORT->dio_out_val_set(CAM_MCLK_SEL_DIO, 0);  // MCLK_SEL low -> on-chip RC
  }
  else {
    colibryPORT->dio_out_val_set(CAM_MCLK_SEL_DIO, 1);  // MCLK_SEL high -> external 12 MHz
  }
  
  // Set a clock to MCLK. EIS316 requires a free-running 12 MHz clock or calibration clock
  // In case of calibration clock, we divide the PLL clock as much as possible (div 256)
  colibryPORT->dio_cfg_func(CAM_MCLK_DIO, PORT_FUNC_ALT3);  // ALT3 = CLK_OUT_0 on IOTboard else ALT3 = CLK_OUT_1
#if defined(__BOARD_IOT_REV0)
  if (onchip_mclk) {
    colibrySCU->regmap->scu_clk_out0_cfg.word = CLKOUT_SRC_PLL_DIV_CLK | (255 << SCU_CLK_OUT0_DIV_OFFSET);
  }
  else {
    colibrySCU->regmap->scu_clk_out0_cfg.word = CLKOUT_SRC_PLL_DIV_CLK |
      ((board_init_cfg.hfclk_freq_mhz / 12 - 1) << SCU_CLK_OUT0_DIV_OFFSET); // CLK_OUT_0 ~= 12 MHz
  }
#else
  if (onchip_mclk) {
    colibrySCU->regmap->scu_clk_out1_cfg.word = CLKOUT_SRC_PLL_DIV_CLK | (255 << SCU_CLK_OUT1_DIV_OFFSET);
  }
  else {
    colibrySCU->regmap->scu_clk_out1_cfg.word = CLKOUT_SRC_PLL_DIV_CLK |
      ((board_init_cfg.hfclk_freq_mhz / 12 - 1) << SCU_CLK_OUT1_DIV_OFFSET); // CLK_OUT_1 ~= 12 MHz
  }
#endif
  colibrySCU->delay_ms(10);
  
  // Startup sequence and calibrations
  _CAMERA_API_CALL(camera_model, startupSequence(onchip_mclk,
                                                board_init_cfg.hfclk_freq_mhz * 1000000 / 256,
                                                expected_video_fps));

#if defined(__BOARD_IOT_REV0)
  _CAMERA_API_CALL(camera_model, setImageFlip(VFLIP_ENABLED, HFLIP_DISABLED));
#else
  _CAMERA_API_CALL(camera_model, setImageFlip(VFLIP_DISABLED, HFLIP_ENABLED));
#endif
  
  if (onchip_mclk) {
    colibrySCU->regmap->scu_clk_out0_cfg.word = CLKOUT_SRC_OFF;
    colibrySCU->regmap->scu_clk_out1_cfg.word = CLKOUT_SRC_OFF;
    colibryPORT->dio_cfg_out(CAM_MCLK_DIO);
    colibryPORT->dio_out_val_set(CAM_MCLK_DIO, 0);
  }

  if (image_width <= 160 && image_height <= 120)
    _CAMERA_API_CALL(camera_model, setImageFormat(IMAGE_FORMAT_QQVGA));
  else if (image_width <= 320 && image_height <= 240)
    _CAMERA_API_CALL(camera_model, setImageFormat(IMAGE_FORMAT_QVGA));
  else
    _CAMERA_API_CALL(camera_model, setImageFormat(IMAGE_FORMAT_VGA));
}

void ColibryBoard::startVideoMode(uint8_t expected_video_fps) {
  _CAMERA_API_CALL(camera_model, startVideoMode(expected_video_fps));
}

void ColibryBoard::copy_bitstream_to_mem(uint32_t flash_addr, uint32_t dest_addr) {
  uint32_t *mmap_ptr, *mmap_ptr_d;
  uint32_t model_size_word;
  
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
  mmap_ptr = (uint32_t *) (EXTF_BASE_ADDR + flash_addr);
  mmap_ptr_d = (uint32_t *) dest_addr;
  model_size_word = ((*mmap_ptr >> 16) & 0xFFFF) * 4;  // Four 32-bit words per NNPA_MEM word
  //printf("%d\n", model_size_word);
  for (int i = 0; i < model_size_word; i++) {
    mmap_ptr_d[i] = mmap_ptr[i];
  }
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
}

uint8_t ColibryBoard::check_nnpa_bitstream() {
  if (NNPA_reg->rd_model_size.word > 40000 || NNPA_reg->rd_model_size.word < 10) {
    if (!board_init_cfg.no_printf){
      printf("ERROR: No bitstream seems in extflash. Exiting\n");
    }
    return 1;
  }
  if (!board_init_cfg.no_printf) {
    printf("CRC_TRACE_PRE model=%u status=0x%08x int=0x%08x trig=0x%08x control=0x%08x mask=0x%08x\n",
           NNPA_reg->rd_model_size.word,
           NNPA_reg->nnpa_status.word,
           NNPA_reg->nnpa_int_status.word,
           NNPA_reg->nnpa_trig_control.word,
           NNPA_reg->nnpa_control.word,
           CONFIG_CHECK_MASK);
  }

  NNPA_reg->nnpa_trig_control.word = CONFIG_CHECK_MASK;
  while (NNPA_reg->nnpa_int_status.fields.evt_config_check_end == 0) ;

  if (!board_init_cfg.no_printf) {
    printf("CRC_TRACE_POST status=0x%08x int=0x%08x trig=0x%08x control=0x%08x config_ok=%u event_end=%u\n",
           NNPA_reg->nnpa_status.word,
           NNPA_reg->nnpa_int_status.word,
           NNPA_reg->nnpa_trig_control.word,
           NNPA_reg->nnpa_control.word,
           NNPA_reg->nnpa_status.fields.config_check_ok,
           NNPA_reg->nnpa_int_status.fields.evt_config_check_end);
  }
  if (NNPA_reg->nnpa_status.fields.config_check_ok != 1) {
    if (!board_init_cfg.no_printf){
      printf("ERROR: Bitstream CRC not correct. Exiting\n");
    }
    return 1;
  }
  else {
    if (!board_init_cfg.no_printf){
      printf("Bitstream loading: OK\n");
    }
    return 0;
  }
}

void ColibryBoard::nnpa_allocate_buffers() {
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
  NNPA_reg->input_start_maddr.word = get_next_buff_addr(NNPA_reg->rd_model_size.word);
  NOP();
  NNPA_reg->data_start_maddr.word = get_next_buff_addr(NNPA_reg->input_start_maddr.word + NNPA_reg->rd_input_size.word);
}

void ColibryBoard::get_image_to_nnpa() {
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 0;
  NNPA_reg->nnpa_int_clear.word = EVT_DATA_LOADED_MASK;
  _CAMERA_API_CALL(camera_model, trigSingleFrame());  // Snapshot with 1 frame
  
  while (NNPA_reg->nnpa_int_status.fields.evt_data_loaded == 0) {
    wait_for_interrupt();  // Wait EVT_EXEC_END event
  }
  
  NNPA_reg->nnpa_int_clear.word = 0xFF;
  NNPA_reg->nnpa_control.fields.cpu_mem_access = 1;
}

void ColibryBoard::extflash_power_enable() {
#if defined(__BOARD_IOT_REV0)
  colibryPORT->dio_cfg_out(VDD_FLASH_ENB_DIO);
  colibryPORT->dio_out_val_set(VDD_FLASH_ENB_DIO, 0);
#elif defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  colibryPORT->dio_cfg_out(VDD_FLASH_ENB_DIO);
  colibryPORT->dio_out_val_set(VDD_FLASH_ENB_DIO, 0);
#endif
}

void ColibryBoard::extflash_power_disable() {
#if defined(__BOARD_IOT_REV0)
  colibryPORT->dio_cfg_out(VDD_FLASH_ENB_DIO);
  colibryPORT->dio_out_val_set(VDD_FLASH_ENB_DIO, 1);
#elif defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  colibryPORT->dio_cfg_out(VDD_FLASH_ENB_DIO);
  colibryPORT->dio_out_val_set(VDD_FLASH_ENB_DIO, 1);
#endif
}

void ColibryBoard::sensors_power_enable() {
#if defined(__BOARD_IOT_REV0)
  colibryPORT->port_dio_function_set(GND_SENSORS_EN_DIO, 0);
  colibryPORT->port_dio_out_en_set(GND_SENSORS_EN_DIO, 1);
  colibryPORT->dio_out_val_set(GND_SENSORS_EN_DIO, 1);
#elif defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  colibryPORT->port_dio_function_set(VDD_SENSORS_ENB_DIO, 0);
  colibryPORT->port_dio_out_en_set(VDD_SENSORS_ENB_DIO, 1);
  colibryPORT->dio_out_val_set(VDD_SENSORS_ENB_DIO, 0);
#endif
  colibrySCU->delay_ms(50);
}

void ColibryBoard::sensors_power_disable() {
#if defined(__BOARD_IOT_REV0)
  colibryPORT->port_dio_function_set(GND_SENSORS_EN_DIO, 0);
#elif defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  colibryPORT->port_dio_function_set(VDD_SENSORS_ENB_DIO, 0);
  colibryPORT->port_dio_out_en_set(VDD_SENSORS_ENB_DIO, 0);
  colibryPORT->dio_pull_res_en(SPIM_ACC_CSN_DIO, PULL_CFG_HIGH);  // Ensure no conflict with SPIM0 comm.
#endif
}

void ColibryBoard::spim0_exchange(uint8_t nbyte, uint8_t csn_sel) {
  if (csn_sel <= 3) {
    colibrySPIM0->regmap->spim_ctrl.fields.spim_csn_sel = csn_sel;
  }
  
  if (board_init_cfg.spim0_csn_margin == 0) {
    colibrySPIM0->spim_transfer(spim0_tx_buff, spim0_rx_buff, nbyte);
  }
  else {
    colibrySPIM0->regmap->spim_ctrl.fields.spim_csn_keep_low = 1;
    colibrySCU->delay_us(board_init_cfg.spim0_csn_margin);
    colibrySPIM0->spim_transfer(spim0_tx_buff, spim0_rx_buff, nbyte);
    colibrySCU->delay_us(10);
    colibrySPIM0->regmap->spim_ctrl.fields.spim_csn_keep_low = 0;
  }
}

void ColibryBoard::spim0_exchange(uint8_t nbyte) {
  spim0_exchange(nbyte, 0xFF);
}

void ColibryBoard::spim1_exchange(uint8_t nbyte, uint8_t csn_sel) {
  if (csn_sel <= 3) {
    colibrySPIM1->regmap->spim_ctrl.fields.spim_csn_sel = csn_sel;
  }
  
  if (board_init_cfg.spim1_csn_margin == 0) {
    colibrySPIM1->spim_transfer(spim1_tx_buff, spim1_rx_buff, nbyte);
  }
  else {
    colibrySPIM1->regmap->spim_ctrl.fields.spim_csn_keep_low = 1;
    colibrySCU->delay_us(board_init_cfg.spim1_csn_margin);
    colibrySPIM1->spim_transfer(spim1_tx_buff, spim1_rx_buff, nbyte);
    colibrySCU->delay_us(10);
    colibrySPIM1->regmap->spim_ctrl.fields.spim_csn_keep_low = 0;
  }
}

void ColibryBoard::spim1_exchange(uint8_t nbyte) {
  spim1_exchange(nbyte, 0xFF);
}

void ColibryBoard::configure_lowpower_io() {
  colibryPORT->dio_cfg_none(1);
  colibryPORT->dio_cfg_none(SPIMF_SCK_DIO);
  colibryPORT->dio_cfg_none(SPIMF_CSN0_DIO);
  colibryPORT->dio_cfg_none(SPIMF_MOSI_DIO);
  colibryPORT->dio_cfg_none(SPIMF_MISO_DIO);
  colibryPORT->dio_cfg_none(JTAG_TCK_DIO);
  colibryPORT->dio_cfg_none(JTAG_TMS_DIO);
  colibryPORT->dio_cfg_none(JTAG_TDI_DIO);
  colibryPORT->regmap->port_dio_cfg[UART_RX_DIO].word |= PORT_DIO_PULL_EN_MASK | PORT_DIO_PULL_CFG_MASK;
#if defined(__BOARD_IOT_REV0)
  colibryPORT->dio_cfg_none(SPIM_ACC_MISO_DIO);
  colibryPORT->dio_cfg_none(SPIM_ACC_SCK_DIO);
  colibryPORT->regmap->port_dio_cfg[MIC_I2S_WS_DIO].word = PORT_DIO_PULL_EN_MASK | PORT_DIO_PULL_CFG_MASK;
  colibryPORT->regmap->port_dio_cfg[SPIM_ACC_CSN_DIO].word = PORT_DIO_PULL_EN_MASK | PORT_DIO_PULL_CFG_MASK;
#endif
#if defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  colibryPORT->dio_cfg_none(MIC_I2S_WS_DIO);
  colibryPORT->dio_cfg_none(MIC_I2S_SD_DIO);
  colibryPORT->dio_cfg_none(MIC_I2S_SCK_DIO);
#endif
}

void ColibryBoard::enter_low_power(uint8_t mode) {
  uint8_t pll_div;
  uint8_t axi_div;
  uint32_t spim_acc_csn_dio_cfg;
  
  NNPA_reg->mem_buff_sleep.word = 1;

#if defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  spim_acc_csn_dio_cfg = colibryPORT->regmap->port_dio_cfg[SPIM_ACC_CSN_DIO].word;
  colibryPORT->dio_pull_res_dis(SPIM_ACC_CSN_DIO);  // To save some power
#endif
  
  /// Enable sleep mode with PLL shutdown
  if (mode == LOW_POWER_CLOCK_OFF_MODE) {
    // Keep only the model in NNPA memory
    NNPA_reg->nb_data_buff.word = 1;
    wfi_clock_off();
    // Restore the regular number of data buffers
    NNPA_reg->nb_data_buff.word = 0;
  }
  else if (mode == LOW_POWER_CLOCK_SLOW_MODE) {
    pll_div = colibrySCU->regmap->pll_control.fields.pll_post_div;
    colibrySCU->regmap->pll_control.fields.pll_post_div = 3;
    wfi_clock_on();
    colibrySCU->regmap->pll_control.fields.pll_post_div = pll_div;
  }
  else if (mode == LOW_POWER_INFERENCE_MODE) {
    axi_div = colibrySCU->regmap->scu_axi_clk_cfg.word;
    colibrySCU->regmap->scu_axi_clk_cfg.word = 31;
    wfi_clock_on();
    colibrySCU->regmap->scu_axi_clk_cfg.word = axi_div;
  }
  else {
    wfi_clock_on();
  }

#if defined(__BOARD_EVBTINY_REV0) || defined(__BOARD_EVBTINY_REV1)
  colibryPORT->regmap->port_dio_cfg[SPIM_ACC_CSN_DIO].word = spim_acc_csn_dio_cfg; // Restore config
#endif
}
