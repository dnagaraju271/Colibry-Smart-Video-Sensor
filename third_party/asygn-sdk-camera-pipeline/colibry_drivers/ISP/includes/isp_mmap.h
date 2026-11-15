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



#define ISP_CROPPER_BYPASS_MASK      0x00000001
#define ISP_CROPPER_BYPASS_OFFSET    0
#define ISP_BAYER2RGB_BYPASS_MASK    0x00000002
#define ISP_BAYER2RGB_BYPASS_OFFSET  1
#define ISP_RGB2GRAY_BYPASS_MASK     0x00000004
#define ISP_RGB2GRAY_BYPASS_OFFSET   2
#define ISP_SCALER_BYPASS_MASK       0x00000008
#define ISP_SCALER_BYPASS_OFFSET     3
#define ISP_SOFT_RESET_MASK          0x00000100
#define ISP_SOFT_RESET_OFFSET        8
#define ISP_CROPPER_BUSY_MASK        0x00000001
#define ISP_CROPPER_BUSY_OFFSET      0
#define ISP_BAYER2RGB_BUSY_MASK      0x00000002
#define ISP_BAYER2RGB_BUSY_OFFSET    1
#define ISP_RGB2GRAY_BUSY_MASK       0x00000004
#define ISP_RGB2GRAY_BUSY_OFFSET     2
#define ISP_SCALER_BUSY_MASK         0x00000008
#define ISP_SCALER_BUSY_OFFSET       3
#define ISP_CROPPER_PSB_INFO_MASK    0x00ffffff
#define ISP_CROPPER_PSB_INFO_OFFSET  0
#define ISP_CROPPER_ROI_TL_X_MASK    0x000007ff
#define ISP_CROPPER_ROI_TL_X_OFFSET  0
#define ISP_CROPPER_ROI_TL_Y_MASK    0x07ff0000
#define ISP_CROPPER_ROI_TL_Y_OFFSET  16
#define ISP_CROPPER_ROI_BR_X_MASK    0x000007ff
#define ISP_CROPPER_ROI_BR_X_OFFSET  0
#define ISP_CROPPER_ROI_BR_Y_MASK    0x07ff0000
#define ISP_CROPPER_ROI_BR_Y_OFFSET  16
#define ISP_B2RGB_PSB_INFO_MASK      0x00ffffff
#define ISP_B2RGB_PSB_INFO_OFFSET    0
#define ISP_B2RGB_RGGB_MODE_MASK     0x00000001
#define ISP_B2RGB_RGGB_MODE_OFFSET   0
#define ISP_RGB2GRAY_PSB_INFO_MASK   0x00ffffff
#define ISP_RGB2GRAY_PSB_INFO_OFFSET 0
#define ISP_SCALER_PSB_INFO_MASK     0x00ffffff
#define ISP_SCALER_PSB_INFO_OFFSET   0
#define ISP_SCALER_SCALING_X_MASK    0x00000fff
#define ISP_SCALER_SCALING_X_OFFSET  0
#define ISP_SCALER_SCALING_Y_MASK    0x0fff0000
#define ISP_SCALER_SCALING_Y_OFFSET  16
#define ISP_SCALER_SIZE_OUT_X_MASK   0x00000fff
#define ISP_SCALER_SIZE_OUT_X_OFFSET 0
#define ISP_SCALER_SIZE_OUT_Y_MASK   0x0fff0000
#define ISP_SCALER_SIZE_OUT_Y_OFFSET 16
#define ISP_CROPPER_PSB_ERR_MASK     0x00000001
#define ISP_CROPPER_PSB_ERR_OFFSET   0
#define ISP_B2RGB_PSB_ERR_MASK       0x00000002
#define ISP_B2RGB_PSB_ERR_OFFSET     1
#define ISP_RGB2GRAY_PSB_ERR_MASK    0x00000004
#define ISP_RGB2GRAY_PSB_ERR_OFFSET  2
#define ISP_SCALER_PSB_ERR_MASK      0x00000008
#define ISP_SCALER_PSB_ERR_OFFSET    3
#define ISP_SCALER_PARAM_ERR_MASK    0x00000010
#define ISP_SCALER_PARAM_ERR_OFFSET  4
#define ISP_FRAME_START_MASK         0x00000020
#define ISP_FRAME_START_OFFSET       5
#define ISP_INT_EN_MASK              0x0000003f
#define ISP_INT_EN_OFFSET            0
#define ISP_INT_SET_MASK             0x0000003f
#define ISP_INT_SET_OFFSET           0
#define ISP_INT_CLEAR_MASK           0x0000003f
#define ISP_INT_CLEAR_OFFSET         0
#define ISP_MBIST_RUN_MASK           0x00000001
#define ISP_MBIST_RUN_OFFSET         0
#define ISP_MBIST_DONE_MASK          0x00000001
#define ISP_MBIST_DONE_OFFSET        0
#define ISP_MBIST_ANY_FAIL_MASK      0x00000002
#define ISP_MBIST_ANY_FAIL_OFFSET    1
#define ISP_MBIST_FAIL_MASK          0x00000003
#define ISP_MBIST_FAIL_OFFSET        0

typedef struct
{
  uint32_t isp_cropper_bypass:1;
  uint32_t isp_bayer2rgb_bypass:1;
  uint32_t isp_rgb2gray_bypass:1;
  uint32_t isp_scaler_bypass:1;
  uint32_t reserved0:4;
  uint32_t isp_soft_reset:1;
  uint32_t reserved1:23;
} s_isp_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_ctrl_type fields;
} u_isp_ctrl_type;

typedef struct
{
  uint32_t isp_cropper_busy:1;
  uint32_t isp_bayer2rgb_busy:1;
  uint32_t isp_rgb2gray_busy:1;
  uint32_t isp_scaler_busy:1;
  uint32_t reserved0:28;
} s_isp_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_status_type fields;
} u_isp_status_type;

typedef struct
{
  uint32_t isp_cropper_psb_info:24;
  uint32_t reserved0:8;
} s_isp_cropper_psb_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_cropper_psb_info_type fields;
} u_isp_cropper_psb_info_type;

typedef struct
{
  uint32_t isp_cropper_roi_tl_x:11;
  uint32_t reserved0:5;
  uint32_t isp_cropper_roi_tl_y:11;
  uint32_t reserved1:5;
} s_isp_cropper_roi_tl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_cropper_roi_tl_type fields;
} u_isp_cropper_roi_tl_type;

typedef struct
{
  uint32_t isp_cropper_roi_br_x:11;
  uint32_t reserved0:5;
  uint32_t isp_cropper_roi_br_y:11;
  uint32_t reserved1:5;
} s_isp_cropper_roi_br_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_cropper_roi_br_type fields;
} u_isp_cropper_roi_br_type;

typedef struct
{
  uint32_t isp_b2rgb_psb_info:24;
  uint32_t reserved0:8;
} s_isp_b2rgb_psb_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_b2rgb_psb_info_type fields;
} u_isp_b2rgb_psb_info_type;

typedef struct
{
  uint32_t isp_b2rgb_rggb_mode:1;
  uint32_t reserved0:31;
} s_isp_b2rgb_rggb_mode_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_b2rgb_rggb_mode_type fields;
} u_isp_b2rgb_rggb_mode_type;

typedef struct
{
  uint32_t isp_rgb2gray_psb_info:24;
  uint32_t reserved0:8;
} s_isp_rgb2gray_psb_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_rgb2gray_psb_info_type fields;
} u_isp_rgb2gray_psb_info_type;

typedef struct
{
  uint32_t isp_scaler_psb_info:24;
  uint32_t reserved0:8;
} s_isp_scaler_psb_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_scaler_psb_info_type fields;
} u_isp_scaler_psb_info_type;

typedef struct
{
  uint32_t isp_scaler_scaling_x:12;
  uint32_t reserved0:4;
  uint32_t isp_scaler_scaling_y:12;
  uint32_t reserved1:4;
} s_isp_scaler_scaling_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_scaler_scaling_cfg_type fields;
} u_isp_scaler_scaling_cfg_type;

typedef struct
{
  uint32_t isp_scaler_size_out_x:12;
  uint32_t reserved0:4;
  uint32_t isp_scaler_size_out_y:12;
  uint32_t reserved1:4;
} s_isp_scaler_size_out_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_scaler_size_out_type fields;
} u_isp_scaler_size_out_type;

typedef struct
{
  uint32_t isp_cropper_psb_err:1;
  uint32_t isp_b2rgb_psb_err:1;
  uint32_t isp_rgb2gray_psb_err:1;
  uint32_t isp_scaler_psb_err:1;
  uint32_t isp_scaler_param_err:1;
  uint32_t isp_frame_start:1;
  uint32_t reserved0:26;
} s_isp_int_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_int_status_type fields;
} u_isp_int_status_type;

typedef struct
{
  uint32_t isp_int_en:6;
  uint32_t reserved0:26;
} s_isp_int_en_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_int_en_type fields;
} u_isp_int_en_type;

typedef struct
{
  uint32_t isp_int_set:6;
  uint32_t reserved0:26;
} s_isp_int_set_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_int_set_type fields;
} u_isp_int_set_type;

typedef struct
{
  uint32_t isp_int_clear:6;
  uint32_t reserved0:26;
} s_isp_int_clear_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_int_clear_type fields;
} u_isp_int_clear_type;

typedef struct
{
  uint32_t isp_mbist_run:1;
  uint32_t reserved0:31;
} s_isp_mbist_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_mbist_ctrl_type fields;
} u_isp_mbist_ctrl_type;

typedef struct
{
  uint32_t isp_mbist_done:1;
  uint32_t isp_mbist_any_fail:1;
  uint32_t reserved0:30;
} s_isp_mbist_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_mbist_status_type fields;
} u_isp_mbist_status_type;

typedef struct
{
  uint32_t isp_mbist_fail:2;
  uint32_t reserved0:30;
} s_isp_mbist_fail_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_isp_mbist_fail_type fields;
} u_isp_mbist_fail_type;

typedef struct
{
  u_isp_ctrl_type isp_ctrl;
  u_isp_status_type isp_status;
  uint32_t reserved0;
  uint32_t reserved1;
  u_isp_cropper_psb_info_type isp_cropper_psb_info;
  u_isp_cropper_roi_tl_type isp_cropper_roi_tl;
  u_isp_cropper_roi_br_type isp_cropper_roi_br;
  uint32_t reserved2;
  u_isp_b2rgb_psb_info_type isp_b2rgb_psb_info;
  u_isp_b2rgb_rggb_mode_type isp_b2rgb_rggb_mode;
  u_isp_rgb2gray_psb_info_type isp_rgb2gray_psb_info;
  uint32_t reserved3;
  u_isp_scaler_psb_info_type isp_scaler_psb_info;
  u_isp_scaler_scaling_cfg_type isp_scaler_scaling_cfg;
  u_isp_scaler_size_out_type isp_scaler_size_out;
  uint32_t reserved4;
  u_isp_int_status_type isp_int_status;
  u_isp_int_en_type isp_int_en;
  u_isp_int_set_type isp_int_set;
  u_isp_int_clear_type isp_int_clear;
  uint32_t reserved5;
  uint32_t reserved6;
  uint32_t reserved7;
  uint32_t reserved8;
  u_isp_mbist_ctrl_type isp_mbist_ctrl;
  u_isp_mbist_status_type isp_mbist_status;
  u_isp_mbist_fail_type isp_mbist_fail;
} s_isp_reg_type;

typedef s_isp_reg_type* p_isp_reg_type;

