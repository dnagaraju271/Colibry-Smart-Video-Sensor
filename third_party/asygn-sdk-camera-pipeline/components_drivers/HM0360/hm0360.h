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


#ifndef __HM0360_H_
#define __HM0360_H_

#include <stdint.h>
#include "hm0360_regs.h"
#include "i2cm_sw.h"

#define HM0360_LINE_LEN_PCK_FULL  0x300
#define HM0360_FRAME_LENGTH_FULL  0x214
#define HM0360_LINE_LEN_PCK_VGA   0x300
#define HM0360_FRAME_LENGTH_VGA   0x214
#define HM0360_LINE_LEN_PCK_QVGA  0x178
#define HM0360_FRAME_LENGTH_QVGA  0x109
#define HM0360_LINE_LEN_PCK_QQVGA 0x178
#define HM0360_FRAME_LENGTH_QQVGA 0x084

#define HM0360_IMAGE_FORMAT_VGA   0x00
#define HM0360_IMAGE_FORMAT_QVGA  0x01
#define HM0360_IMAGE_FORMAT_QQVGA 0x02

#define HM0360_FRAME_LEN_1FPS       80


class CameraHM0360 {
 private:
  I2CmSw *m_i2cmsw;
  
 public:
  uint32_t i2c_slave_7baddr = 0x24;
  
  typedef enum {
     VERSION_OK           = 0x00,
     VERSION_ERROR        = 0x01,
  } version_status_t;
  
  
  CameraHM0360();
  CameraHM0360(I2CmSw *i2cmsw);

  uint8_t readRegister(uint16_t address);
  uint8_t writeRegister(uint16_t address, uint8_t data);
  uint8_t checkVersion();
  void startupSequence(uint8_t mclk_rc_mode, uint32_t mclk_cal_freq, uint8_t max_fps);
  void setImageFormat(uint8_t image_format);
  void setImageFlip(uint8_t flip_v, uint8_t flip_h);
  void startVideoMode(uint16_t framerate);
  void stopVideoMode();
  void trigSingleFrame(void);
  void clearFrameStartIrq(void);
};


const uint16_t hm0360_qqvga_regs[][2] = {
    {HM0360_REG_PLL1_CONFIG, 0x05},
    {HM0360_REG_H_SUBSAMPLE, 0x02},
    {HM0360_REG_V_SUBSAMPLE, 0x02},
    {HM0360_REG_BINNING_MODE, 0x00},
    {HM0360_REG_WIN_MODE, 0x01},
    {HM0360_REG_MAX_INTG_H, (HM0360_FRAME_LENGTH_QQVGA*4 - 4) >> 8},
    {HM0360_REG_MAX_INTG_L, (HM0360_FRAME_LENGTH_QQVGA*4 - 4) & 0xFF},
    {HM0360_REG_FRAME_LEN_LINES_H, (HM0360_FRAME_LENGTH_QQVGA >> 8)},
    {HM0360_REG_FRAME_LEN_LINES_L, (HM0360_FRAME_LENGTH_QQVGA & 0xFF)},
    {HM0360_REG_LINE_LEN_PCK_H, (HM0360_LINE_LEN_PCK_QQVGA >> 8)},
    {HM0360_REG_LINE_LEN_PCK_L, (HM0360_LINE_LEN_PCK_QQVGA & 0xFF)},
    {HM0360_REG_COMMAND_UPDATE, 0x01},
    {0x0000, 0x00},
};


const uint16_t hm0360_qvga_regs[][2] = {
    {HM0360_REG_PLL1_CONFIG, 0x09},
    {HM0360_REG_H_SUBSAMPLE, 0x01},
    {HM0360_REG_V_SUBSAMPLE, 0x01},
    {HM0360_REG_BINNING_MODE, 0x00},
    {HM0360_REG_WIN_MODE, 0x01},
    {HM0360_REG_MAX_INTG_H, (HM0360_FRAME_LENGTH_QVGA - 4) >> 8},
    {HM0360_REG_MAX_INTG_L, (HM0360_FRAME_LENGTH_QVGA - 4) & 0xFF},
    {HM0360_REG_FRAME_LEN_LINES_H, (HM0360_FRAME_LENGTH_QVGA >> 8)},
    {HM0360_REG_FRAME_LEN_LINES_L, (HM0360_FRAME_LENGTH_QVGA & 0xFF)},
    {HM0360_REG_LINE_LEN_PCK_H, (HM0360_LINE_LEN_PCK_QVGA >> 8)},
    {HM0360_REG_LINE_LEN_PCK_L, (HM0360_LINE_LEN_PCK_QVGA & 0xFF)},
    {HM0360_REG_COMMAND_UPDATE, 0x01},
    {0x0000, 0x00},
};


const uint16_t hm0360_vga_regs[][2] = {
    {HM0360_REG_PLL1_CONFIG, 0x08}, // Core = 24MHz PCLKO = 24MHz I2C = 12MHz
    {HM0360_REG_H_SUBSAMPLE, 0x00},
    {HM0360_REG_V_SUBSAMPLE, 0x00},
    {HM0360_REG_BINNING_MODE, 0x00},
    {HM0360_REG_WIN_MODE, 0x01},
    {HM0360_REG_MAX_INTG_H, (HM0360_FRAME_LENGTH_VGA - 4) >> 8},
    {HM0360_REG_MAX_INTG_L, (HM0360_FRAME_LENGTH_VGA - 4) & 0xFF},
    {HM0360_REG_FRAME_LEN_LINES_H, (HM0360_FRAME_LENGTH_VGA >> 8)},
    {HM0360_REG_FRAME_LEN_LINES_L, (HM0360_FRAME_LENGTH_VGA & 0xFF)},
    {HM0360_REG_LINE_LEN_PCK_H, (HM0360_LINE_LEN_PCK_VGA >> 8)},
    {HM0360_REG_LINE_LEN_PCK_L, (HM0360_LINE_LEN_PCK_VGA & 0xFF)},
    {HM0360_REG_ROI_START_END_H, 0xF0},
    {HM0360_REG_ROI_START_END_V, 0xE0},
    {0x310F, 0x00}, // puts it in 8bit mode
    {0x3112, 0x04}, // was 0x0c
    {HM0360_REG_COMMAND_UPDATE, 0x01},
    {0x0000, 0x00},
};


const uint16_t hm0360_default_regs[][2] = {
    {HM0360_REG_SW_RESET, 0x00},
    {HM0360_REG_MONO_MODE, 0x00},
    {HM0360_REG_MONO_MODE_ISP, 0x01},
    {HM0360_REG_MONO_MODE_SEL, 0x01},

    // BLC control
    {0x1000, 0x01},
    {0x1003, 0x04},
    {HM0360_REG_BLC_TGT, 0x04},
    {0x1007, 0x01},
    {0x1008, 0x04},
    {HM0360_REG_BLC2_TGT, 0x04},
    {HM0360_REG_MONO_CTRL, 0x01},

    // Output format control
    {HM0360_REG_OPFM_CTRL, 0x08},

    // Reserved regs
    {0x101D, 0x00},
    {0x101E, 0x01},
    {0x101F, 0x00},
    {0x1020, 0x01},
    {0x1021, 0x00},

    {HM0360_REG_CMPRS_CTRL, 0x00},
    {HM0360_REG_CMPRS_01, 0x09},
    {HM0360_REG_CMPRS_02, 0x12},
    {HM0360_REG_CMPRS_03, 0x23},
    {HM0360_REG_CMPRS_04, 0x31},
    {HM0360_REG_CMPRS_05, 0x3E},
    {HM0360_REG_CMPRS_06, 0x4B},
    {HM0360_REG_CMPRS_07, 0x56},
    {HM0360_REG_CMPRS_08, 0x5E},
    {HM0360_REG_CMPRS_09, 0x65},
    {HM0360_REG_CMPRS_10, 0x72},
    {HM0360_REG_CMPRS_11, 0x7F},
    {HM0360_REG_CMPRS_12, 0x8C},
    {HM0360_REG_CMPRS_13, 0x98},
    {HM0360_REG_CMPRS_14, 0xB2},
    {HM0360_REG_CMPRS_15, 0xCC},
    {HM0360_REG_CMPRS_16, 0xE6},

    {0x3112, 0x00}, // PCLKO_polarity falling

    {HM0360_REG_PLL1_CONFIG, 0x08}, // Core = 24MHz PCLKO = 24MHz I2C = 12MHz
    {HM0360_REG_PLL2_CONFIG, 0x0A}, // MIPI pre-dev (default)
    {HM0360_REG_PLL3_CONFIG, 0x77}, // PMU/MIPI pre-dev (default)

    {HM0360_REG_PMU_CFG_3, 0x08},       // Disable context switching
    {HM0360_REG_PAD_REGISTER_07, 0x00}, // PCLKO_polarity falling

    {HM0360_REG_AE_CTRL, 0x5F}, // Automatic Exposure (NOTE: Auto framerate enabled)
    {HM0360_REG_AE_CTRL1, 0x00},
    {HM0360_REG_T_DAMPING, 0x20},       // AE T damping factor
    {HM0360_REG_N_DAMPING, 0x00},       // AE N damping factor
    {HM0360_REG_AE_TARGET_MEAN, 0x64},  // AE target
    {HM0360_REG_AE_MIN_MEAN, 0x0A},     // AE min target mean
    {HM0360_REG_AE_TARGET_ZONE, 0x23},  // AE target zone
    {HM0360_REG_CONVERGE_IN_TH, 0x03},  // AE converge in threshold
    {HM0360_REG_CONVERGE_OUT_TH, 0x05}, // AE converge out threshold
    {HM0360_REG_MAX_INTG_H, (HM0360_FRAME_LENGTH_QVGA - 4) >> 8},
    {HM0360_REG_MAX_INTG_L, (HM0360_FRAME_LENGTH_QVGA - 4) & 0xFF},

    {HM0360_REG_MAX_AGAIN, 0x05}, // Maximum analog gain
    {HM0360_REG_MAX_DGAIN_H, 0x02},
    {HM0360_REG_MAX_DGAIN_L, 0x00},
    {HM0360_REG_INTEGRATION_H, 0x15},
    {HM0360_REG_INTEGRATION_L, 0x50},

    {HM0360_REG_MD_CTRL, 0x00},
    {HM0360_REG_MD_TH_MIN, 0x01},
    {HM0360_REG_MD_BLOCK_NUM_TH, 0x01},
    {HM0360_REG_MD_CTRL1, 0x06},
    {HM0360_REG_PULSE_MODE, 0x00}, // Interrupt in level mode.
    {HM0360_REG_ROI_START_END_V, 0xF0},
    {HM0360_REG_ROI_START_END_H, 0xF0},

    /// SYNC function config.
    {0x3010, 0x00},
    {0x3013, 0x01},
    {0x3019, 0x00},
    {0x301A, 0x00},
    {0x301B, 0x20},
    {0x301C, 0xFF},
    
    // PREMETER config.
    {0x3026, 0x03},
    {0x3027, 0x81},
    {0x3028, 0x01},
    {0x3029, 0x00},
    {0x302A, 0x30},
    {0x302E, 0x00},
    {0x302F, 0x00},

    // Magic regs 🪄.
    {0x302B, 0x2A},
    {0x302C, 0x00},
    {0x302D, 0x03},
    {0x3031, 0x01},
    {0x3051, 0x00},
    {0x305C, 0x03},
    {0x3060, 0x00},
    {0x3061, 0xFA},
    {0x3062, 0xFF},
    {0x3063, 0xFF},
    {0x3064, 0xFF},
    {0x3065, 0xFF},
    {0x3066, 0xFF},
    {0x3067, 0xFF},
    {0x3068, 0xFF},
    {0x3069, 0xFF},
    {0x306A, 0xFF},
    {0x306B, 0xFF},
    {0x306C, 0xFF},
    {0x306D, 0xFF},
    {0x306E, 0xFF},
    {0x306F, 0xFF},
    {0x3070, 0xFF},
    {0x3071, 0xFF},
    {0x3072, 0xFF},
    {0x3073, 0xFF},
    {0x3074, 0xFF},
    {0x3075, 0xFF},
    {0x3076, 0xFF},
    {0x3077, 0xFF},
    {0x3078, 0xFF},
    {0x3079, 0xFF},
    {0x307A, 0xFF},
    {0x307B, 0xFF},
    {0x307C, 0xFF},
    {0x307D, 0xFF},
    {0x307E, 0xFF},
    {0x307F, 0xFF},
    {0x3080, 0x01},
    {0x3081, 0x01},
    {0x3082, 0x03},
    {0x3083, 0x20},
    {0x3084, 0x00},
    {0x3085, 0x20},
    {0x3086, 0x00},
    {0x3087, 0x20},
    {0x3088, 0x00},
    {0x3089, 0x04},
    {0x3094, 0x02},
    {0x3095, 0x02},
    {0x3096, 0x00},
    {0x3097, 0x00},
    {0x3098, 0x00},
    {0x3099, 0x00},
    {0x309E, 0x05},
    {0x309F, 0x02},
    {0x30A0, 0x02},
    {0x30A1, 0x00},
    {0x30A2, 0x08},
    {0x30A3, 0x00},
    {0x30A4, 0x20},
    {0x30A5, 0x04},
    {0x30A6, 0x02},
    {0x30A7, 0x02},
    {0x30A8, 0x01},
    {0x30A9, 0x00},
    {0x30AA, 0x02},
    {0x30AB, 0x34},
    {0x30B0, 0x03},
    {0x30C4, 0x10},
    {0x30C5, 0x01},
    {0x30C6, 0xBF},
    {0x30C7, 0x00},
    {0x30C8, 0x00},
    {0x30CB, 0xFF},
    {0x30CC, 0xFF},
    {0x30CD, 0x7F},
    {0x30CE, 0x7F},
    {0x30D3, 0x01},
    {0x30D4, 0xFF},
    {0x30D5, 0x00},
    {0x30D6, 0x40},
    {0x30D7, 0x00},
    {0x30D8, 0xA7},
    {0x30D9, 0x05},
    {0x30DA, 0x01},
    {0x30DB, 0x40},
    {0x30DC, 0x00},
    {0x30DD, 0x27},
    {0x30DE, 0x05},
    {0x30DF, 0x07},
    {0x30E0, 0x40},
    {0x30E1, 0x00},
    {0x30E2, 0x27},
    {0x30E3, 0x05},
    {0x30E4, 0x47},
    {0x30E5, 0x30},
    {0x30E6, 0x00},
    {0x30E7, 0x27},
    {0x30E8, 0x05},
    {0x30E9, 0x87},
    {0x30EA, 0x30},
    {0x30EB, 0x00},
    {0x30EC, 0x27},
    {0x30ED, 0x05},
    {0x30EE, 0x00},
    {0x30EF, 0x40},
    {0x30F0, 0x00},
    {0x30F1, 0xA7},
    {0x30F2, 0x05},
    {0x30F3, 0x01},
    {0x30F4, 0x40},
    {0x30F5, 0x00},
    {0x30F6, 0x27},
    {0x30F7, 0x05},
    {0x30F8, 0x07},
    {0x30F9, 0x40},
    {0x30FA, 0x00},
    {0x30FB, 0x27},
    {0x30FC, 0x05},
    {0x30FD, 0x47},
    {0x30FE, 0x30},
    {0x30FF, 0x00},
    {0x3100, 0x27},
    {0x3101, 0x05},
    {0x3102, 0x87},
    {0x3103, 0x30},
    {0x3104, 0x00},
    {0x3105, 0x27},
    {0x3106, 0x05},
    {0x310B, 0x10},
    {0x3113, 0xA0},
    {0x3114, 0x67},
    {0x3115, 0x42},
    {0x3116, 0x10},
    {0x3117, 0x0A},
    {0x3118, 0x3F},
    {0x311C, 0x10},
    {0x311D, 0x06},
    {0x311E, 0x0F},
    {0x311F, 0x0E},
    {0x3120, 0x0D},
    {0x3121, 0x0F},
    {0x3122, 0x00},
    {0x3123, 0x1D},
    {0x3126, 0x03},
    {0x3128, 0x57},
    {0x312A, 0x11},
    {0x312B, 0x41},
    {0x312E, 0x00},
    {0x312F, 0x00},
    {0x3130, 0x0C},
    {0x3141, 0x2A},
    {0x3142, 0x9F},
    {0x3147, 0x18},
    {0x3149, 0x18},
    {0x314B, 0x01},
    {0x3150, 0x50},
    {0x3152, 0x00},
    {0x3156, 0x2C},
    {0x315A, 0x0A},
    {0x315B, 0x2F},
    {0x315C, 0xE0},
    {0x315F, 0x02},
    {0x3160, 0x1F},
    {0x3163, 0x1F},
    {0x3164, 0x7F},
    {0x3165, 0x7F},
    {0x317B, 0x94},
    {0x317C, 0x00},
    {0x317D, 0x02},
    {0x318C, 0x00},
    {HM0360_REG_COMMAND_UPDATE, 0x01},
    {0x0000, 0x00},
};

#endif
