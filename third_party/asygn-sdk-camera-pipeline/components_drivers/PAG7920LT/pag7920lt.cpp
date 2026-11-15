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

#include "pag7920lt.h"
#include "pag7920lt_regs.h"


CameraPAG7920LT::CameraPAG7920LT() {}

CameraPAG7920LT::CameraPAG7920LT(I2CmSw *i2cmsw) : m_i2cmsw(i2cmsw) {
  m_i2cmsw->scl_pulse();  // Needed for first access to be functional
}

void CameraPAG7920LT::setSlaveAddr(uint8_t i2c_address) {
  i2c_slave_7baddr = i2c_address;
}

uint8_t CameraPAG7920LT::readRegister(uint8_t address) {
  m_i2cmsw->txbuffer[0] = address;
  m_i2cmsw->transfer(1, 1, i2c_slave_7baddr);
  return m_i2cmsw->rxbuffer[0];
}

void CameraPAG7920LT::writeRegister(uint8_t address, uint8_t value) {
  m_i2cmsw->txbuffer[0] = address;
  m_i2cmsw->txbuffer[1] = value;
  m_i2cmsw->transfer(2, 0, i2c_slave_7baddr);
}

uint8_t CameraPAG7920LT::checkVersion() {
  if (readRegister(PAG7920LT_REG_PARTID_LSB) == 0x20 && readRegister(PAG7920LT_REG_PARTID_MSB) == 0x79)
    return VERSION_OK;
  else
    return VERSION_ERROR;
}

void CameraPAG7920LT::startupSequence(uint8_t mclk_rc_mode, uint32_t mclk_cal_freq, uint8_t max_fps) {
  writeRegister(PAG7920LT_REG_INTF_POLARITY, 0x31); // HSYNC, VSYNC active high
  // Set GPIO1 to output LED expo signal with active high polarity
  writeRegister(PAG7920LT_REG_GPIO1_CONTROL, (1 << 7)); // GPIO1 out
  writeRegister(PAG7920LT_REG_GPIO1_SEL, PAG7920LT_GPIO1_SEL_LED);
  writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x02);
  writeRegister(PAG7920LT_REG_LED_POLARITY, 1);
  writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
    
  // TODO: Exposure settings if different from default
}

void CameraPAG7920LT::setImageFormat(uint8_t image_format) {
  if (image_format == PAG7920LT_IMAGE_FORMAT_QVGA) {
    for (int i = 0; pag7920lt_qvga_common_settings[i][0]; i++) {
      writeRegister(pag7920lt_qvga_common_settings[i][0], pag7920lt_qvga_common_settings[i][1]);
    }
  }
  else if (image_format == PAG7920LT_IMAGE_FORMAT_QQVGA) {
    for (int i = 0; pag7920lt_qvga_common_settings[i][0]; i++) {
      writeRegister(pag7920lt_qvga_common_settings[i][0], pag7920lt_qvga_common_settings[i][1]);
    }
    for (int i = 0; pag7920lt_qqvga_2xavg_addon_settings[i][0]; i++) {
      writeRegister(pag7920lt_qqvga_2xavg_addon_settings[i][0], pag7920lt_qqvga_2xavg_addon_settings[i][1]);
    }
  }
  else if (image_format == PAG7920LT_IMAGE_FORMAT_MOTION) {
    for (int i = 0; pag7920lt_motion_detect_settings[i][0]; i++) {
      writeRegister(pag7920lt_motion_detect_settings[i][0], pag7920lt_motion_detect_settings[i][1]);
    }
  }
}

void CameraPAG7920LT::setImageFlip(uint8_t flip_v, uint8_t flip_h) {
    writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
    writeRegister(0x09, 0x00);
    writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x01);
    writeRegister(0xC2, 0x00);
    writeRegister(0xCB, 0x04);
    writeRegister(0xCE, 0x00);
    writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x02);
    writeRegister(0xD9, 0x00);
    writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
    writeRegister(PAG7920LT_REG_UPDATE_FLAG, 0x80);

    if (flip_h) {
      writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
      writeRegister(0x09, 0x00);
      writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x02);
      writeRegister(0xD9, 0x02);
      writeRegister(PAG7920LT_REG_UPDATE_FLAG, 0x80);
    }
  
    if (flip_v) {
      writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x01);
      writeRegister(0xC2, 0xF7);
      writeRegister(0xCB, 0xF3);
      writeRegister(0xCE, 0x04);
      writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
      writeRegister(PAG7920LT_REG_UPDATE_FLAG, 0x80);
    }
}

void CameraPAG7920LT::trigSingleFrame() {
  if (! trigger_mode_enabled) {
    writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
    writeRegister(0x2E, 1);
    writeRegister(PAG7920LT_REG_R_TG_EN, 0x00);
    writeRegister(PAG7920LT_REG_R_TRIGGER_EN, 0x01);
    writeRegister(PAG7920LT_REG_UPDATE_FLAG, 0x80);
    trigger_mode_enabled = 1;
    writeRegister(PAG7920LT_REG_TRIGGER_CMD, 0x01);
    colibrySCU->delay_ms(5);
  }
  writeRegister(PAG7920LT_REG_TRIGGER_CMD, 0x01);
}

void CameraPAG7920LT::startVideoMode(uint16_t framerate) {
  uint32_t frame_time = 12000000 / framerate;
  trigger_mode_enabled = 0;
  writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
  writeRegister(PAG7920LT_REG_FRAME_TIME_0, frame_time & 0xFF);
  writeRegister(PAG7920LT_REG_FRAME_TIME_1, (frame_time >> 8) & 0xFF);
  writeRegister(PAG7920LT_REG_FRAME_TIME_2, (frame_time >> 16) & 0xFF);
  writeRegister(PAG7920LT_REG_FRAME_TIME_3, (frame_time >> 24) & 0xFF);
  
  writeRegister(PAG7920LT_REG_R_TRIGGER_EN, 0x00);
  writeRegister(PAG7920LT_REG_R_TG_EN, 0x01);
  writeRegister(PAG7920LT_REG_UPDATE_FLAG, 0x80);
}

void CameraPAG7920LT::stopVideoMode() {
  trigger_mode_enabled = 0;
  writeRegister(PAG7920LT_REG_BANK_NUMBER, 0x00);
  writeRegister(PAG7920LT_REG_R_TG_EN, 0x00);
  writeRegister(PAG7920LT_REG_UPDATE_FLAG, 0x80);
}

void CameraPAG7920LT::clearFrameStartIrq() {
}
