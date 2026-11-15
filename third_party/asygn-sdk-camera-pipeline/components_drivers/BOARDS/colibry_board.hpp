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



#pragma once

#include <stdint.h>

#include "chip.h"
#include "i2cm_sw.h"
#include "tps6286x.h"
#include "spis_protocol.h"
#include "eis316.h"
#include "hm0360.h"
#include "hm0360.h"
#include "pag7920lt.h"
#include "lis2dw12.h"
#include "nrf_spi_opcodes.h"
#include "formatted_printf.h"


#if defined(BOARD_IOT_REV0)
  #include "colibry_iotboard.h"
#elif defined(BOARD_EVB_REV0)
  #include "colibry_evalboard_rev0.h"
#elif defined(BOARD_EVB_REV1)
  #include "colibry_evalboard_rev1.h"
#elif defined(BOARD_EVBTINY_REV0)
  #include "colibry_evbtiny_rev0.h"
#elif defined(BOARD_EVBTINY_REV1)
  #include "colibry_evbtiny_rev1.h"
#else
  #error "No board defined!"
#endif


#define CAMERA_API_CALL(model, func) (model == EIS316) ? colibryBoard.cameraEIS316.func :  \
                                     (model == HM0360) ? colibryBoard.cameraHM0360.func : \
                                                         colibryBoard.cameraPAG7920LT.func

#define _CAMERA_API_CALL(model, func) (model == EIS316) ? cameraEIS316.func : \
                                      (model == HM0360) ? cameraHM0360.func : \
                                                          cameraPAG7920LT.func

#define I2C_SW_HALF_PERIOD_US         2

#define IMAGE_FORMAT_VGA    0x00
#define IMAGE_FORMAT_QVGA   0x01
#define IMAGE_FORMAT_QQVGA  0x02
#define IMAGE_FORMAT_MOTION 0x03


enum camera_model_t {EIS316 = 0,
                     HM0360 = 1,
                     PAG7920LT = 2,
                    };

enum low_power_modes {LOW_POWER_CLOCK_OFF_MODE = 0,   // Lowest power mode, clocks and PLL turned off
                      LOW_POWER_CLOCK_SLOW_MODE = 1,  // CPU halted, HFCLK decreased to the minimum for image reception
                      LOW_POWER_INFERENCE_MODE = 2};  // CPU halted, only NNPA inference running, AXI clk slowed down

typedef struct
{
  float vdd_setting;   // VDD power setting between 0.7 and 1.3 (V)
  uint32_t hfclk_freq_mhz;
  uint8_t apb_clk_fdiv = 1;
  uint8_t nnpa_clk_fdiv = 1;
  uint8_t spis_enable = 0;     // 0 in lora
  uint8_t spim_rf_enable = 0;  // Only for iot or Tiny boards
  uint8_t spim_acc_enable = 0; 
  uint8_t spis_dma_channel = 0;
  uint16_t spim0_csn_margin = 0;
  uint16_t spim1_csn_margin = 0;
  uint8_t no_printf = 0;
  uint16_t setup_delay = 1000;
} board_init_cfg_t;


class ColibryBoard {
 private:
  
 public:
  const uint8_t STATUS_OK = 0;
  const uint8_t STATUS_PARAM_ERR = 1;

  I2CmSw i2cm_sw;
  Tps6286x tps6286x;
  Lis2dw12 lis2dw12;
  
  CameraEIS316 cameraEIS316;
  CameraHM0360 cameraHM0360;
  CameraPAG7920LT cameraPAG7920LT;
  camera_model_t camera_model;
  board_init_cfg_t board_init_cfg;
  
  uint8_t spim0_tx_buff[16];
  uint8_t spim0_rx_buff[16];
  uint8_t spim1_tx_buff[16];
  uint8_t spim1_rx_buff[16];

  
  ColibryBoard();
  
  /** @brief Convert address expressed in NNPA memory words to global MMAP address
   *
   * @param mem_word_addr Word address
   *
   */
  inline uint32_t nnpa_mem_addr_to_global(uint32_t mem_word_addr) {
    return NNPAMEM_BASE_ADDR + 16 * mem_word_addr;
  }


  /** @brief Provides the starting word address of the next buffer which comes after the given word address
   *
   * @param mem_word_addr Word address
   *
   */
  inline uint32_t get_next_buff_addr(uint32_t mem_word_addr) {
    uint32_t rounded;
    rounded = (mem_word_addr + 1023) / 1024;
    return 1024 * rounded;
  }

  
  /** @brief Initialize the board and ColibryNPU according to input configuration.
   *
   * @param board_init_cfg The init configuration structure.
   * 
   */
  void global_init(board_init_cfg_t board_init_cfg);

  
  /** @brief Initialize the camera present on the board with model auto-detection.
   *         The camera can be an EIS316 or HM0360.
   *
   * @param image_width The expected width of the image after the ISP.
   * @param image_width The expected height of the image after the ISP.
   * @param onchip_mclk Set to 1 if the camera input clock is generated internally, else 0 if provided.
   * @param expected_video_fps The expected FPS from the sensor.
   * 
   */
  void camera_init(uint16_t image_width, uint16_t image_height, uint8_t onchip_mclk, uint8_t expected_video_fps);

  
  /** @brief Start the video mode of the camera
   *
   * @param expected_video_fps The expected FPS from the sensor.
   * 
   */
  void startVideoMode(uint8_t expected_video_fps);

  
  /** @brief Copy NNPA bitstream from the external flash to the NNPA memory.
   *
   * @param flash_addr The start address in the flash memory space of the bitstream location.
   * @param dest_addr The destination address within Colibry memory map.
   *
   */
  void copy_bitstream_to_mem(uint32_t flash_addr, uint32_t dest_addr);

  
  /** @brief Check if a bitstream is correctly loaded from first NNPA memory address.
   *
   * @retval The status of the check (0 if OK, else 1).
   *
   */
  uint8_t check_nnpa_bitstream();

  
  /** @brief Allocate input and data buffers of the NNPA from the bitstream information.
   *
   * @retval The status of the check (0 if OK, else 1).
   *
   */
  void nnpa_allocate_buffers();

  
  /** @brief Get an image from the camera to the NNPA memory manually.
   *
   */
  void get_image_to_nnpa();

  
  /** @brief Enable the external flash power VDD.
   *
   */
  void extflash_power_enable();
  

  /** @brief Disable the external flash power VDD.
   *
   */
  void extflash_power_disable();

  
  /** @brief Enable the sensors power supply.
   *
   */
  void sensors_power_enable();
  

  /** @brief Disable the sensors power supply.
   *
   */
  void sensors_power_disable();
  

  /** @brief Initiate a SPIM0 transfer, sending nbyte bytes from the spim0_tx_buff array and
   *         receiving the same amount of bytes into spim0_rx_buff.
   *
   * @param nbyte The number of bytes to transfer.
   * @param csn_sel Selection of CSn signal to use. If > 3, keep the last used.
   *
   */
  void spim0_exchange(uint8_t nbyte, uint8_t csn_sel);
  
  void spim0_exchange(uint8_t nbyte);
  

  /** @brief Initiate a SPIM1 transfer, sending nbyte bytes from the spim1_tx_buff array and
   *         receiving the same amount of bytes into spim1_rx_buff.
   *
   * @param nbyte The number of bytes to transfer.
   * @param csn_sel Selection of CSn signal to use. If > 3, keep the last used.
   *
   */
  void spim1_exchange(uint8_t nbyte, uint8_t csn_sel);
  
  void spim1_exchange(uint8_t nbyte);


  /** @brief Configure some I/Os to reach lowest power. The I/O functions can still be changed 
   *         again after calling this method.
   *
   */
  void configure_lowpower_io();
  

  /** @brief Put the system in a low-power mode.
   *
   * @param mode See low_power_modes.
   * 
   */
  void enter_low_power(uint8_t mode);
};
  
