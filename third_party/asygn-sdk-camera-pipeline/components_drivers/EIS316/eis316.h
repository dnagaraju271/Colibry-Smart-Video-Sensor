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
#ifndef __EIS316_H__
#define __EIS316_H__

// define this as extern for c++
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>


#define EIS316_IMAGE_FORMAT_VGA   0x00
#define EIS316_IMAGE_FORMAT_QVGA  0x01
#define EIS316_IMAGE_FORMAT_QQVGA 0x02
  
  
class CameraEIS316 {
 private:
  uint8_t i2cm_txbuffer[128];
  uint8_t i2cm_rxbuffer[128];
  I2CmSw *m_i2cmsw;
  
  uint8_t bitdepth;		// 1-> 8b images
  uint8_t video;		  // 0-> single frame
  
  void enableAutoExposure(uint32_t target, uint32_t min, uint32_t max, bool fast);
  void disableAutoExposure(void);
  uint32_t cclkAutoCalib(uint16_t target);
  uint32_t rcManCalib(uint32_t mclk_cal_freq);
  uint32_t rcAutoCalib();
  uint32_t testRegister(void);
  uint8_t getMCLK_SEL();
  void setMCLK_SEL(bool value);
  void setRSTB(bool value);
  void sleep(int ms);
  
 public:
  uint32_t i2c_slave_7baddr = 0x42; // Or 0x43
  
  typedef enum {
     VERSION_OK           = 0x00,
     VERSION_ERROR        = 0x01,
  } version_status_t;

  CameraEIS316();
  CameraEIS316(I2CmSw *i2cmsw);
  
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
  
  void setMode(uint8_t mode);
  void setExposure(uint32_t target);
  uint32_t getExposure(void);
  void waitIrqEvent(uint8_t eventMask);
};
  

// =======================
// Mode definitions
// =======================
#define EIS316_REG_SO_MODE           0
#define EIS316_REG_DRS_MODE          1
#define EIS316_REG_CAL_SO_MODE       2
#define EIS316_REG_CAL_DRS_MODE      3
#define EIS316_REG_TEST0_MODE        4
#define EIS316_REG_TEST1_MODE        5
#define EIS316_REG_CAL_CLK_MODE      6
#define EIS316_REG_CAL_CLK_AUTO_MODE 7
#define EIS316_REG_CAL_ABB_MODE      8
#define EIS316_REG_CAL_RC_MODE       9
#define EIS316_REG_CAL_RC_AUTO_MODE  10
#define EIS316_REG_SO_HDR_MODE       11
#define EIS316_REG_DRS_HDR_MODE      12
#define EIS316_REG_SO_HDR_MAN_MODE   13
#define EIS316_REG_DRS_HDR_MAN_MODE  14

// =======================
// Power mode
// =======================
#define EIS316_REG_PM_IDLE    0
#define EIS316_REG_PM_STANDBY 1
#define EIS316_REG_PM_SLEEP   2

// =======================
// Strobe mode
// =======================
#define EIS316_REG_STROBE_OFF     0
#define EIS316_REG_STROBE_CAPTURE 1
#define EIS316_REG_STROBE_AO      2

// =======================
// Flip modes
// =======================
#define VFLIP_ENABLED  1
#define VFLIP_DISABLED 0
#define HFLIP_ENABLED  1
#define HFLIP_DISABLED 0
  
// =======================
// IRQ masks
// =======================
#define EIS316_REG_CAPTDONE_IRQ_MASK    0x01
#define EIS316_REG_EXPAD_IRQ_MASK       0x02
#define EIS316_REG_AUTOEXPMAX_IRQ_MASK  0x04
#define EIS316_REG_AUTOEXPMIN_IRQ_MASK  0x08
#define EIS316_REG_AUTOEXPOV_IRQ_MASK   0x10
#define EIS316_REG_AUTOEXPST_IRQ_MASK   0x20
#define EIS316_REG_AUTOEXPNTST_IRQ_MASK 0x40
#define EIS316_REG_MD_IRQ_MASK          0x80

// =======================
// Register addresses
// =======================
#define EIS316_REG_CTRL_A       0x00
#define EIS316_REG_CFG_A        0x01
#define EIS316_REG_OPCTRL_A     0x02
#define EIS316_REG_STATUS_A     0x03
#define EIS316_REG_IRQCFG0_A    0x04
#define EIS316_REG_IRQCFG1_A    0x05
#define EIS316_REG_IRQEN_A      0x06
#define EIS316_REG_IRQFLG_A     0x07
#define EIS316_REG_IRQCLR_A     0x08
#define EIS316_REG_PWR_A        0x09
#define EIS316_REG_DCMI_A       0x0A
#define EIS316_REG_EXP0_A       0x0B
#define EIS316_REG_EXP1_A       0x0C
#define EIS316_REG_AUTOEXP0_A   0x0D
#define EIS316_REG_AUTOEXP1_A   0x0E
#define EIS316_REG_AUTOEXP2_A   0x0F
#define EIS316_REG_AUTOEXP3_A   0x10
#define EIS316_REG_AUTOEXP4_A   0x11
#define EIS316_REG_AUTOEXP5_A   0x12
#define EIS316_REG_AUTOEXP6_A   0x13
#define EIS316_REG_FRATE0_A     0x14
#define EIS316_REG_FRATE1_A     0x15
#define EIS316_REG_STROBE_A     0x16
#define EIS316_REG_FLIP_A       0x17
#define EIS316_REG_ROI0_A       0x18
#define EIS316_REG_ROI1_A       0x19
#define EIS316_REG_ROI2_A       0x1A
#define EIS316_REG_ROI3_A       0x1B
#define EIS316_REG_ROI4_A       0x1C
#define EIS316_REG_SAMPLING_A   0x1D
#define EIS316_REG_EDGEMAP0_A   0x1E
#define EIS316_REG_EDGEMAP1_A   0x1F
#define EIS316_REG_EDGEMAP2_A   0x20
#define EIS316_REG_MD0_A        0x21
#define EIS316_REG_MD1_A        0x22
#define EIS316_REG_MD2_A        0x23
#define EIS316_REG_MD3_A        0x24
#define EIS316_REG_MD4_A        0x25
#define EIS316_REG_MD5_A        0x26
#define EIS316_REG_MD6_A        0x27
#define EIS316_REG_MDGRID0_A    0x28
#define EIS316_REG_MDGRID1_A    0x29
#define EIS316_REG_MDGRID2_A    0x2A
#define EIS316_REG_MDGRID3_A    0x2B
#define EIS316_REG_MDGRID4_A    0x2C
#define EIS316_REG_MDGRID5_A    0x2D
#define EIS316_REG_MDGRID6_A    0x2E
#define EIS316_REG_MDGRID7_A    0x2F
#define EIS316_REG_MDGRID8_A    0x30
#define EIS316_REG_MDGRID9_A    0x31
#define EIS316_REG_MDGRID10_A   0x32
#define EIS316_REG_MDGRID11_A   0x33
#define EIS316_REG_MDGRID12_A   0x34
#define EIS316_REG_MDGRID13_A   0x35
#define EIS316_REG_MDGRID14_A   0x36
#define EIS316_REG_MDGRID15_A   0x37
#define EIS316_REG_MDGRID16_A   0x38
#define EIS316_REG_MDGRID17_A   0x39
#define EIS316_REG_MDGRID18_A   0x3A
#define EIS316_REG_MDGRID19_A   0x3B
#define EIS316_REG_MDGRID20_A   0x3C
#define EIS316_REG_MDGRID21_A   0x3D
#define EIS316_REG_MDGRID22_A   0x3E
#define EIS316_REG_MDGRID23_A   0x3F
#define EIS316_REG_MDGRID24_A   0x40
#define EIS316_REG_MDGRID25_A   0x41
#define EIS316_REG_MDGRID26_A   0x42
#define EIS316_REG_MDGRID27_A   0x43
#define EIS316_REG_MDGRID28_A   0x44
#define EIS316_REG_MDGRID29_A   0x45
#define EIS316_REG_MDGRID30_A   0x46
#define EIS316_REG_MDGRID31_A   0x47
#define EIS316_REG_MDGRID32_A   0x48
#define EIS316_REG_MDGRID33_A   0x49
#define EIS316_REG_MDGRID34_A   0x4A
#define EIS316_REG_MDGRID35_A   0x4B
#define EIS316_REG_MDGRID36_A   0x4C
#define EIS316_REG_MDGRID37_A   0x4D
#define EIS316_REG_MDGRID38_A   0x4E
#define EIS316_REG_MDGRID39_A   0x4F
#define EIS316_REG_BRIGHT0_A    0x50
#define EIS316_REG_BRIGHT1_A    0x51
#define EIS316_REG_ABB0_A       0x52
#define EIS316_REG_ABB1_A       0x53
#define EIS316_REG_ABB2_A       0x54
#define EIS316_REG_DPCORR0_A    0x55
#define EIS316_REG_DPCORR1_A    0x56
#define EIS316_REG_DPCORR2_A    0x57
#define EIS316_REG_DPCORR3_A    0x58
#define EIS316_REG_DPCORR4_A    0x59
#define EIS316_REG_DPCORR5_A    0x5A
#define EIS316_REG_DPCORR6_A    0x5B
#define EIS316_REG_DPCORR7_A    0x5C
#define EIS316_REG_DPCORR8_A    0x5D
#define EIS316_REG_HDR0_A       0x5E
#define EIS316_REG_HDR1_A       0x5F
#define EIS316_REG_HDR2_A       0x60
#define EIS316_REG_HDR3_A       0x61
#define EIS316_REG_HDR4_A       0x62
#define EIS316_REG_HDR5_A       0x63
#define EIS316_REG_ANALOG0_A    0x64
#define EIS316_REG_ANALOG1_A    0x65
#define EIS316_REG_RC0_A        0x66
#define EIS316_REG_RC1_A        0x67
#define EIS316_REG_RC2_A        0x68
#define EIS316_REG_CCLK0_A      0x69
#define EIS316_REG_CCLK1_A      0x6A
#define EIS316_REG_CCLK2_A      0x6B
#define EIS316_REG_CCLK3_A      0x6C
#define EIS316_REG_CCLK4_A      0x6D
#define EIS316_REG_OFFSET0_A    0x6E
#define EIS316_REG_OFFSET1_A    0x6F
#define EIS316_REG_OFFSET2_A    0x70
#define EIS316_REG_DBG_A        0x7D
#define EIS316_REG_TEST_A       0x7E
#define EIS316_REG_VERSION_A    0x7F
	 
	 

#ifdef __cplusplus
}
#endif 

#endif // EIS316_REG_H
