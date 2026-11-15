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


#include "hm0360.h"
#include "chip.h"


CameraHM0360::CameraHM0360() {}

CameraHM0360::CameraHM0360(I2CmSw *i2cmsw) : m_i2cmsw(i2cmsw) {
}

uint8_t CameraHM0360::readRegister(uint16_t address) {
  m_i2cmsw->txbuffer[0] = (address >> 8) & 0xFF; // Register address MSB
  m_i2cmsw->txbuffer[1] = address & 0xFF;        // Register address LSB
  m_i2cmsw->transfer(2, 1, i2c_slave_7baddr);
  return m_i2cmsw->rxbuffer[0];
}

uint8_t CameraHM0360::writeRegister(uint16_t address, uint8_t data) {
  m_i2cmsw->txbuffer[0] = (address >> 8) & 0xFF; // Register address MSB
  m_i2cmsw->txbuffer[1] = address & 0xFF;        // Register address LSB
  m_i2cmsw->txbuffer[2] = data;
  m_i2cmsw->transfer(3, 0, i2c_slave_7baddr);
  return 0;
}

uint8_t CameraHM0360::checkVersion() {
  if (readRegister(HM0360_REG_MODEL_ID_H) == 0x03 && readRegister(HM0360_REG_MODEL_ID_L) == 0x60) {
    return VERSION_OK;
  }
  else {
    return VERSION_ERROR;
  }
}

void CameraHM0360::startupSequence(uint8_t mclk_rc_mode, uint32_t mclk_cal_freq, uint8_t max_fps) {
  writeRegister(HM0360_REG_SW_RESET, 0x01);
  
  for (int i = 0; hm0360_default_regs[i][0]; i++) {
    writeRegister(hm0360_default_regs[i][0], hm0360_default_regs[i][1]);
    //printf("%x %x\n", hm0360_default_regs[i][0], hm0360_default_regs[i][1]);
  }
  
  writeRegister(HM0360_REG_PMU_CFG_3, 0x08);  // Disable CTX (context)
  writeRegister(HM0360_REG_MIPI_EN, 0);

  // Auto-exposure
  //writeRegister(HM0360_REG_AE_CTRL, 0x5F);
  writeRegister(HM0360_REG_AE_CTRL, 0x1F);
  //writeRegister(0x30A5, 4);
  
  // PCLK active during frames
  writeRegister(HM0360_REG_OPFM_CTRL, 0);
  writeRegister(HM0360_REG_PCLKO_GATED_EN, 0x02);
  writeRegister(HM0360_REG_COMMAND_UPDATE, 1);
}

void CameraHM0360::setImageFormat(uint8_t image_format) {
  if (image_format == HM0360_IMAGE_FORMAT_QQVGA) {
    for (int i = 0; hm0360_qqvga_regs[i][0]; i++) {
      writeRegister(hm0360_qqvga_regs[i][0], hm0360_qqvga_regs[i][1]);
    }
  }
  else if (image_format == HM0360_IMAGE_FORMAT_QVGA) {
    for (int i = 0; hm0360_qvga_regs[i][0]; i++) {
      writeRegister(hm0360_qvga_regs[i][0], hm0360_qvga_regs[i][1]);
    }
  }
  else {
    for (int i = 0; hm0360_vga_regs[i][0]; i++) {
      writeRegister(hm0360_vga_regs[i][0], hm0360_vga_regs[i][1]);
    }
  }
}

void CameraHM0360::setImageFlip(uint8_t flip_v, uint8_t flip_h) {
	writeRegister(HM0360_REG_IMG_ORIENTATION, (flip_v << 1) | flip_h);
}

void CameraHM0360::trigSingleFrame(void) {
  writeRegister(HM0360_REG_PMU_CFG_7, 1);  // 1 frame
  writeRegister(HM0360_REG_MODE_SELECT, 0x03);  // Snapshot with N frame
}

void CameraHM0360::startVideoMode(uint16_t framerate) {
  writeRegister(HM0360_REG_OPFM_CTRL, 8);
  writeRegister(HM0360_REG_FRAME_LEN_LINES_L, 0);
  writeRegister(HM0360_REG_FRAME_LEN_LINES_H, HM0360_FRAME_LEN_1FPS / framerate);
  writeRegister(HM0360_REG_MODE_SELECT, 0x01);
}

void CameraHM0360::stopVideoMode() {
  writeRegister(HM0360_REG_OPFM_CTRL, 0);
  writeRegister(HM0360_REG_MODE_SELECT, 0x00);
}

void CameraHM0360::clearFrameStartIrq() {
  // VSYNC automatically returns to 0
}
