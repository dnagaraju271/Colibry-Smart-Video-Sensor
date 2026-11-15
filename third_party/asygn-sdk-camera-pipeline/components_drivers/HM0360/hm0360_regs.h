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


#ifndef __HM0360_REGISTERS_H_
#define __HM0360_REGISTERS_H_

// Read only registers
#define HM0360_REG_MODEL_ID_H        0x0000
#define HM0360_REG_MODEL_ID_L        0x0001
#define HM0360_REG_SILICON_REV       0x0002
#define HM0360_REG_FRAME_COUNT_H     0x0005
#define HM0360_REG_FRAME_COUNT_L     0x0006
#define HM0360_REG_PIXEL_ORDER       0x0007
// Sensor mode control
#define HM0360_REG_MODE_SELECT       0x0100
#define HM0360_REG_IMG_ORIENTATION   0x0101
#define HM0360_REG_EMBEDDED_LINE_EN  0x0102
#define HM0360_REG_SW_RESET          0x0103
#define HM0360_REG_COMMAND_UPDATE    0x0104
// Sensor exposure gain control
#define HM0360_REG_INTEGRATION_H     0x0202
#define HM0360_REG_INTEGRATION_L     0x0203
#define HM0360_REG_ANALOG_GAIN       0x0205
#define HM0360_REG_DIGITAL_GAIN_H    0x020E
#define HM0360_REG_DIGITAL_GAIN_L    0x020F
// Clock control
#define HM0360_REG_PLL1_CONFIG       0x0300
#define HM0360_REG_PLL2_CONFIG       0x0301
#define HM0360_REG_PLL3_CONFIG       0x0302
// Frame timing control
#define HM0360_REG_FRAME_LEN_LINES_H 0x0340
#define HM0360_REG_FRAME_LEN_LINES_L 0x0341
#define HM0360_REG_LINE_LEN_PCK_H    0x0342
#define HM0360_REG_LINE_LEN_PCK_L    0x0343
// Monochrome programming
#define HM0360_REG_MONO_MODE         0x0370
#define HM0360_REG_MONO_MODE_ISP     0x0371
#define HM0360_REG_MONO_MODE_SEL     0x0372
// Binning mode control
#define HM0360_REG_H_SUBSAMPLE       0x0380
#define HM0360_REG_V_SUBSAMPLE       0x0381
#define HM0360_REG_BINNING_MODE      0x0382
// Test pattern control
#define HM0360_REG_TEST_PATTERN_MODE 0x0601
// Black level control
#define HM0360_REG_BLC_TGT           0x1004
#define HM0360_REG_BLC2_TGT          0x1009
#define HM0360_REG_MONO_CTRL         0x100A
// VSYNC / HSYNC / pixel shift registers
#define HM0360_REG_OPFM_CTRL         0x1014
// Tone mapping registers
#define HM0360_REG_CMPRS_CTRL        0x102F
#define HM0360_REG_CMPRS_01          0x1030
#define HM0360_REG_CMPRS_02          0x1031
#define HM0360_REG_CMPRS_03          0x1032
#define HM0360_REG_CMPRS_04          0x1033
#define HM0360_REG_CMPRS_05          0x1034
#define HM0360_REG_CMPRS_06          0x1035
#define HM0360_REG_CMPRS_07          0x1036
#define HM0360_REG_CMPRS_08          0x1037
#define HM0360_REG_CMPRS_09          0x1038
#define HM0360_REG_CMPRS_10          0x1039
#define HM0360_REG_CMPRS_11          0x103A
#define HM0360_REG_CMPRS_12          0x103B
#define HM0360_REG_CMPRS_13          0x103C
#define HM0360_REG_CMPRS_14          0x103D
#define HM0360_REG_CMPRS_15          0x103E
#define HM0360_REG_CMPRS_16          0x103F
// Automatic exposure control
#define HM0360_REG_AE_CTRL           0x2000
#define HM0360_REG_AE_CTRL1          0x2001
#define HM0360_REG_CNT_ORGH_H        0x2002
#define HM0360_REG_CNT_ORGH_L        0x2003
#define HM0360_REG_CNT_ORGV_H        0x2004
#define HM0360_REG_CNT_ORGV_L        0x2005
#define HM0360_REG_CNT_STH_H         0x2006
#define HM0360_REG_CNT_STH_L         0x2007
#define HM0360_REG_CNT_STV_H         0x2008
#define HM0360_REG_CNT_STV_L         0x2009
#define HM0360_REG_CTRL_PG_SKIPCNT   0x200A
#define HM0360_REG_BV_WIN_WEIGHT_EN  0x200D
#define HM0360_REG_MAX_INTG_H        0x2029
#define HM0360_REG_MAX_INTG_L        0x202A
#define HM0360_REG_MAX_AGAIN         0x202B
#define HM0360_REG_MAX_DGAIN_H       0x202C
#define HM0360_REG_MAX_DGAIN_L       0x202D
#define HM0360_REG_MIN_INTG          0x202E
#define HM0360_REG_MIN_AGAIN         0x202F
#define HM0360_REG_MIN_DGAIN         0x2030
#define HM0360_REG_T_DAMPING         0x2031
#define HM0360_REG_N_DAMPING         0x2032
#define HM0360_REG_ALC_TH            0x2033
#define HM0360_REG_AE_TARGET_MEAN    0x2034
#define HM0360_REG_AE_MIN_MEAN       0x2035
#define HM0360_REG_AE_TARGET_ZONE    0x2036
#define HM0360_REG_CONVERGE_IN_TH    0x2037
#define HM0360_REG_CONVERGE_OUT_TH   0x2038
#define HM0360_REG_FS_CTRL           0x203B
#define HM0360_REG_FS_60HZ_H         0x203C
#define HM0360_REG_FS_60HZ_L         0x203D
#define HM0360_REG_FS_50HZ_H         0x203E
#define HM0360_REG_FS_50HZ_L         0x203F
#define HM0360_REG_FRAME_CNT_TH      0x205B
#define HM0360_REG_AE_MEAN           0x205D
#define HM0360_REG_AE_CONVERGE       0x2060
#define HM0360_REG_AE_BLI_TGT        0x2070
// Interrupt control
#define HM0360_REG_PULSE_MODE        0x2061
#define HM0360_REG_PULSE_TH_H        0x2062
#define HM0360_REG_PULSE_TH_L        0x2063
#define HM0360_REG_INT_INDIC         0x2064
#define HM0360_REG_INT_CLEAR         0x2065
// Motion detection control
#define HM0360_REG_MD_CTRL           0x2080
#define HM0360_REG_ROI_START_END_V   0x2081
#define HM0360_REG_ROI_START_END_H   0x2082
#define HM0360_REG_MD_TH_MIN         0x2083
#define HM0360_REG_MD_TH_STR_L       0x2084
#define HM0360_REG_MD_TH_STR_H       0x2085
#define HM0360_REG_MD_LIGHT_COEF     0x2099
#define HM0360_REG_MD_BLOCK_NUM_TH   0x209B
#define HM0360_REG_MD_LATENCY        0x209C
#define HM0360_REG_MD_LATENCY_TH     0x209D
#define HM0360_REG_MD_CTRL1          0x209E
// MIPI
#define HM0360_REG_MIPI_EN           0x2800
#define HM0360_REG_LANE_CFG          0x2821
#define HM0360_REG_EMB_DATA_CFG      0x2822
// Context switch control registers
#define HM0360_REG_PMU_CFG_3         0x3024
#define HM0360_REG_PMU_CFG_4         0x3025
#define HM0360_REG_PMU_CFG_5         0x3026
#define HM0360_REG_PMU_CFG_6         0x3027
#define HM0360_REG_PMU_CFG_7         0x3028
#define HM0360_REG_PMU_CFG_8         0x3029
#define HM0360_REG_PMU_CFG_9         0x302A
// Operation mode control
#define HM0360_REG_WIN_MODE          0x3030
// IO and clock control
#define HM0360_REG_PCLKO_GATED_EN    0x309E
#define HM0360_REG_PAD_REGISTER_07   0x3112


// Register values


#endif
