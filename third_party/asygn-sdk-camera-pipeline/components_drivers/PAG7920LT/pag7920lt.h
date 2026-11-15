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


#include "chip.h"
#include "i2cm_sw.h"

// include guard
#ifndef __PAG7920_H__
#define __PAG7920_H__

#include <stdint.h>
#include <stdbool.h>


//#define PAG7920LT_IMAGE_FORMAT_VGA   0x00
#define PAG7920LT_IMAGE_FORMAT_QVGA   0x01
#define PAG7920LT_IMAGE_FORMAT_QQVGA  0x02
#define PAG7920LT_IMAGE_FORMAT_MOTION 0x03
  
  
class CameraPAG7920LT {
 private:
  uint8_t i2cm_txbuffer[128];
  uint8_t i2cm_rxbuffer[128];
  I2CmSw *m_i2cmsw;
  uint8_t trigger_mode_enabled = 0;
  
 public:
  uint32_t i2c_slave_7baddr = 0x35; // GPIO2 high: 0x25, low: 0x40, floating: 0x35 
  
  typedef enum {
     VERSION_OK           = 0x00,
     VERSION_ERROR        = 0x01,
  } version_status_t;

  CameraPAG7920LT();
  CameraPAG7920LT(I2CmSw *i2cmsw);
  
  void setSlaveAddr(uint8_t i2c_address);
  uint8_t readRegister(uint8_t address);
  void writeRegister(uint8_t address, uint8_t value);
  uint8_t checkVersion();
  void startupSequence(uint8_t mclk_rc_mode, uint32_t mclk_cal_freq, uint8_t max_fps);
  void setImageFormat(uint8_t image_format);
  void setImageFlip(uint8_t flip_v, uint8_t flip_h);
  void startVideoMode(uint16_t framerate);
  void stopVideoMode();
  void trigSingleFrame(void);
  void clearFrameStartIrq(void);
};
  
  
  
#define PAG7920LT_I2CADDR_GPIO2_HIGH   0x25
#define PAG7920LT_I2CADDR_GPIO2_HZ     0x35
#define PAG7920LT_I2CADDR_GPIO2_LOW    0x40


#endif
