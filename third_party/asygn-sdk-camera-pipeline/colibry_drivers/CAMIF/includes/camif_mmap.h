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



#define DATA_IN_MODE_MASK         0x0000000f
#define DATA_IN_MODE_OFFSET       0
#define PIXEL_IN_WIDTH_MASK       0x000000f0
#define PIXEL_IN_WIDTH_OFFSET     4
#define RGBSPLIT_R_FIRST_MASK     0x00000100
#define RGBSPLIT_R_FIRST_OFFSET   8
#define NIBBLE_SPLIT_EN_MASK      0x00000200
#define NIBBLE_SPLIT_EN_OFFSET    9
#define NIBBLE_ENDIANNESS_MASK    0x00000400
#define NIBBLE_ENDIANNESS_OFFSET  10
#define PCLK_SAMPLING_CFG_MASK    0x00000800
#define PCLK_SAMPLING_CFG_OFFSET  11
#define CONV_DATA_SHIFT_MASK      0x00007000
#define CONV_DATA_SHIFT_OFFSET    12
#define RGBX_MSB_ALIGNED_MASK     0x00010000
#define RGBX_MSB_ALIGNED_OFFSET   16
#define IMG_WIDTH_MASK            0x00001fff
#define IMG_WIDTH_OFFSET          0
#define IMG_HEIGHT_MASK           0x1fff0000
#define IMG_HEIGHT_OFFSET         16
#define IMG_COL_CNT_MASK          0x00001fff
#define IMG_COL_CNT_OFFSET        0
#define IMG_ROW_CNT_MASK          0x1fff0000
#define IMG_ROW_CNT_OFFSET        16
#define IMG_COUNT_MASK            0x000003ff
#define IMG_COUNT_OFFSET          0
#define IMG_COUNT_RST_MASK        0x00000001
#define IMG_COUNT_RST_OFFSET      0
#define PATGEN_FRAME_START_MASK   0x00000001
#define PATGEN_FRAME_START_OFFSET 0
#define PATGEN_LFSR_RST_MASK      0x00000002
#define PATGEN_LFSR_RST_OFFSET    1
#define PATGEN_DATA_MODE_MASK     0x00000001
#define PATGEN_DATA_MODE_OFFSET   0
#define PATGEN_MUX_SEL_MASK       0x00000002
#define PATGEN_MUX_SEL_OFFSET     1
#define PATGEN_IMG_NCHAN_MASK     0x0000000c
#define PATGEN_IMG_NCHAN_OFFSET   2
#define PATGEN_SLOW_RATE_MASK     0x00000ff0
#define PATGEN_SLOW_RATE_OFFSET   4
#define LFSR_AUTO_RESTART_MASK    0x00001000
#define LFSR_AUTO_RESTART_OFFSET  12
#define PATGEN_RAMP_INIT0_MASK    0x000003ff
#define PATGEN_RAMP_INIT0_OFFSET  0
#define PATGEN_RAMP_INIT1_MASK    0x000ffc00
#define PATGEN_RAMP_INIT1_OFFSET  10
#define PATGEN_RAMP_INIT2_MASK    0x3ff00000
#define PATGEN_RAMP_INIT2_OFFSET  20
#define PATGEN_BUSY_MASK          0x00000001
#define PATGEN_BUSY_OFFSET        0
#define CDC_FIFO_BYPASS_MASK      0x00000001
#define CDC_FIFO_BYPASS_OFFSET    0
#define CAM_DATA_MASK             0x00000fff
#define CAM_DATA_OFFSET           0
#define DATA_FIFO_WORD_NB_MASK    0x0000001f
#define DATA_FIFO_WORD_NB_OFFSET  0
#define DATA_FIFO_THR_MASK        0x0000001f
#define DATA_FIFO_THR_OFFSET      0
#define EVT_IMG_OVERFLOW_MASK     0x00000001
#define EVT_IMG_OVERFLOW_OFFSET   0
#define EVT_IMG_UNDERFLOW_MASK    0x00000002
#define EVT_IMG_UNDERFLOW_OFFSET  1
#define EVT_IMG_START_MASK        0x00000004
#define EVT_IMG_START_OFFSET      2
#define EVT_IMG_END_MASK          0x00000008
#define EVT_IMG_END_OFFSET        3
#define EVT_DATA_FIFO_RDY_MASK    0x00000010
#define EVT_DATA_FIFO_RDY_OFFSET  4
#define EVT_DATA_FIFO_OVF_MASK    0x00000020
#define EVT_DATA_FIFO_OVF_OFFSET  5
#define CAMIF_INT_EN_MASK         0x0000003f
#define CAMIF_INT_EN_OFFSET       0
#define CAMIF_INT_SET_MASK        0x0000003f
#define CAMIF_INT_SET_OFFSET      0
#define CAMIF_INT_CLEAR_MASK      0x0000003f
#define CAMIF_INT_CLEAR_OFFSET    0
#define CAMIF_REV_MINOR_MASK      0x0000ffff
#define CAMIF_REV_MINOR_OFFSET    0
#define CAMIF_REV_MAJOR_MASK      0xffff0000
#define CAMIF_REV_MAJOR_OFFSET    16

typedef struct
{
  uint32_t data_in_mode:4;
  uint32_t pixel_in_width:4;
  uint32_t rgbsplit_r_first:1;
  uint32_t nibble_split_en:1;
  uint32_t nibble_endianness:1;
  uint32_t pclk_sampling_cfg:1;
  uint32_t conv_data_shift:3;
  uint32_t reserved0:1;
  uint32_t rgbx_msb_aligned:1;
  uint32_t reserved1:15;
} s_camif_protocol_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_protocol_cfg_type fields;
} u_camif_protocol_cfg_type;

typedef struct
{
  uint32_t img_width:13;
  uint32_t reserved0:3;
  uint32_t img_height:13;
  uint32_t reserved1:3;
} s_camif_img_size_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_img_size_cfg_type fields;
} u_camif_img_size_cfg_type;

typedef struct
{
  uint32_t img_col_cnt:13;
  uint32_t reserved0:3;
  uint32_t img_row_cnt:13;
  uint32_t reserved1:3;
} s_camif_img_curr_ptr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_img_curr_ptr_type fields;
} u_camif_img_curr_ptr_type;

typedef struct
{
  uint32_t img_count:10;
  uint32_t reserved0:22;
} s_camif_img_count_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_img_count_type fields;
} u_camif_img_count_type;

typedef struct
{
  uint32_t img_count_rst:1;
  uint32_t reserved0:31;
} s_camif_img_count_rst_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_img_count_rst_type fields;
} u_camif_img_count_rst_type;

typedef struct
{
  uint32_t patgen_frame_start:1;
  uint32_t patgen_lfsr_rst:1;
  uint32_t reserved0:30;
} s_camif_patgen_control_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_patgen_control_type fields;
} u_camif_patgen_control_type;

typedef struct
{
  uint32_t patgen_data_mode:1;
  uint32_t patgen_mux_sel:1;
  uint32_t patgen_img_nchan:2;
  uint32_t patgen_slow_rate:8;
  uint32_t lfsr_auto_restart:1;
  uint32_t reserved0:19;
} s_camif_patgen_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_patgen_cfg_type fields;
} u_camif_patgen_cfg_type;

typedef struct
{
  uint32_t patgen_ramp_init0:10;
  uint32_t patgen_ramp_init1:10;
  uint32_t patgen_ramp_init2:10;
  uint32_t reserved0:2;
} s_camif_patgen_ramp_init_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_patgen_ramp_init_type fields;
} u_camif_patgen_ramp_init_type;

typedef struct
{
  uint32_t patgen_busy:1;
  uint32_t reserved0:31;
} s_camif_patgen_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_patgen_status_type fields;
} u_camif_patgen_status_type;

typedef struct
{
  uint32_t cdc_fifo_bypass:1;
  uint32_t reserved0:31;
} s_camif_cdc_fifo_bypass_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_cdc_fifo_bypass_type fields;
} u_camif_cdc_fifo_bypass_type;

typedef struct
{
  uint32_t cam_data:12;
  uint32_t reserved0:20;
} s_camif_cam_data_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_cam_data_type fields;
} u_camif_cam_data_type;

typedef struct
{
  uint32_t data_fifo_word_nb:5;
  uint32_t reserved0:27;
} s_camif_data_fifo_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_data_fifo_status_type fields;
} u_camif_data_fifo_status_type;

typedef struct
{
  uint32_t data_fifo_thr:5;
  uint32_t reserved0:27;
} s_camif_data_fifo_thr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_data_fifo_thr_type fields;
} u_camif_data_fifo_thr_type;

typedef struct
{
  uint32_t evt_img_overflow:1;
  uint32_t evt_img_underflow:1;
  uint32_t evt_img_start:1;
  uint32_t evt_img_end:1;
  uint32_t evt_data_fifo_rdy:1;
  uint32_t evt_data_fifo_ovf:1;
  uint32_t reserved0:26;
} s_camif_int_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_int_status_type fields;
} u_camif_int_status_type;

typedef struct
{
  uint32_t camif_int_en:6;
  uint32_t reserved0:26;
} s_camif_int_en_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_int_en_type fields;
} u_camif_int_en_type;

typedef struct
{
  uint32_t camif_int_set:6;
  uint32_t reserved0:26;
} s_camif_int_set_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_int_set_type fields;
} u_camif_int_set_type;

typedef struct
{
  uint32_t camif_int_clear:6;
  uint32_t reserved0:26;
} s_camif_int_clear_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_int_clear_type fields;
} u_camif_int_clear_type;

typedef struct
{
  uint32_t camif_rev_minor:16;
  uint32_t camif_rev_major:16;
} s_camif_version_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_camif_version_type fields;
} u_camif_version_type;

typedef struct
{
  u_camif_protocol_cfg_type camif_protocol_cfg;
  u_camif_img_size_cfg_type camif_img_size_cfg;
  u_camif_img_curr_ptr_type camif_img_curr_ptr;
  u_camif_img_count_type camif_img_count;
  u_camif_img_count_rst_type camif_img_count_rst;
  u_camif_patgen_control_type camif_patgen_control;
  u_camif_patgen_cfg_type camif_patgen_cfg;
  u_camif_patgen_ramp_init_type camif_patgen_ramp_init;
  u_camif_patgen_status_type camif_patgen_status;
  u_camif_cdc_fifo_bypass_type camif_cdc_fifo_bypass;
  u_camif_cam_data_type camif_cam_data;
  u_camif_data_fifo_status_type camif_data_fifo_status;
  u_camif_data_fifo_thr_type camif_data_fifo_thr;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
  u_camif_int_status_type camif_int_status;
  u_camif_int_en_type camif_int_en;
  u_camif_int_set_type camif_int_set;
  u_camif_int_clear_type camif_int_clear;
  uint32_t reserved3;
  uint32_t reserved4;
  uint32_t reserved5;
  u_camif_version_type camif_version;
} s_camif_reg_type;

typedef s_camif_reg_type* p_camif_reg_type;

