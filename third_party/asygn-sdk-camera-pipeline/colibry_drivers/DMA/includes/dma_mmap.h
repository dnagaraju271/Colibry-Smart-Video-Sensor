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



#define DMA_VERSION_MINOR_MASK     0x0000ffff
#define DMA_VERSION_MINOR_OFFSET   0
#define DMA_VERSION_MAJOR_MASK     0xffff0000
#define DMA_VERSION_MAJOR_OFFSET   16
#define DMA_EVT_DONE_MASK          0x000000ff
#define DMA_EVT_DONE_OFFSET        0
#define DMA_EVT_READ_ERROR_MASK    0x0000ff00
#define DMA_EVT_READ_ERROR_OFFSET  8
#define DMA_EVT_WRITE_ERROR_MASK   0x00ff0000
#define DMA_EVT_WRITE_ERROR_OFFSET 16
#define DMA_EVT_THRESHOLD_MASK     0xff000000
#define DMA_EVT_THRESHOLD_OFFSET   24
#define DMA_INT_EN_MASK            0xffffffff
#define DMA_INT_EN_OFFSET          0
#define DMA_INT_SET_MASK           0xffffffff
#define DMA_INT_SET_OFFSET         0
#define DMA_INT_CLEAR_MASK         0xffffffff
#define DMA_INT_CLEAR_OFFSET       0
#define DMA_START_MASK             0x00000001
#define DMA_START_OFFSET           0
#define DMA_STOP_MASK              0x00000002
#define DMA_STOP_OFFSET            1
#define DMA_RUNNING_MASK           0x00000004
#define DMA_RUNNING_OFFSET         2
#define DMA_RESTART_EN_MASK        0x00000008
#define DMA_RESTART_EN_OFFSET      3
#define DMA_PRIORITY_MASK          0x00000030
#define DMA_PRIORITY_OFFSET        4
#define DMA_STRIDE_EN_MASK         0x00000040
#define DMA_STRIDE_EN_OFFSET       6
#define DMA_PERIPH_SRC_ID_MASK     0x00000f00
#define DMA_PERIPH_SRC_ID_OFFSET   8
#define DMA_SRC_STATIC_MASK        0x00002000
#define DMA_SRC_STATIC_OFFSET      13
#define DMA_SRC_WIDTH_MASK         0x0000c000
#define DMA_SRC_WIDTH_OFFSET       14
#define DMA_PERIPH_DST_ID_MASK     0x000f0000
#define DMA_PERIPH_DST_ID_OFFSET   16
#define DMA_DST_STATIC_MASK        0x00200000
#define DMA_DST_STATIC_OFFSET      21
#define DMA_DST_WIDTH_MASK         0x00c00000
#define DMA_DST_WIDTH_OFFSET       22
#define DMA_EXT_TRIG_EN_MASK       0x01000000
#define DMA_EXT_TRIG_EN_OFFSET     24
#define DMA_EXT_TRIG_ID_MASK       0x06000000
#define DMA_EXT_TRIG_ID_OFFSET     25
#define DMA_LEN_MASK               0xffffffff
#define DMA_LEN_OFFSET             0
#define DMA_SRC_MASK               0xffffffff
#define DMA_SRC_OFFSET             0
#define DMA_DST_MASK               0xffffffff
#define DMA_DST_OFFSET             0
#define DMA_SDL_MASK               0xffffffff
#define DMA_SDL_OFFSET             0
#define DMA_SKL_MASK               0xffffffff
#define DMA_SKL_OFFSET             0
#define DMA_THRESHOLD_MASK         0x0000ffff
#define DMA_THRESHOLD_OFFSET       0

typedef struct
{
  uint32_t dma_version_minor:16;
  uint32_t dma_version_major:16;
} s_dma_version_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_version_type fields;
} u_dma_version_type;

typedef struct
{
  uint32_t dma_evt_done:8;
  uint32_t dma_evt_read_error:8;
  uint32_t dma_evt_write_error:8;
  uint32_t dma_evt_threshold:8;
} s_dma_int_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_int_status_type fields;
} u_dma_int_status_type;

typedef struct
{
  uint32_t dma_int_en:32;
} s_dma_int_en_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_int_en_type fields;
} u_dma_int_en_type;

typedef struct
{
  uint32_t dma_int_set:32;
} s_dma_int_set_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_int_set_type fields;
} u_dma_int_set_type;

typedef struct
{
  uint32_t dma_int_clear:32;
} s_dma_int_clear_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_int_clear_type fields;
} u_dma_int_clear_type;

typedef struct
{
  uint32_t dma_start:1;
  uint32_t dma_stop:1;
  uint32_t dma_running:1;
  uint32_t dma_restart_en:1;
  uint32_t dma_priority:2;
  uint32_t dma_stride_en:1;
  uint32_t reserved0:1;
  uint32_t dma_periph_src_id:4;
  uint32_t reserved1:1;
  uint32_t dma_src_static:1;
  uint32_t dma_src_width:2;
  uint32_t dma_periph_dst_id:4;
  uint32_t reserved2:1;
  uint32_t dma_dst_static:1;
  uint32_t dma_dst_width:2;
  uint32_t dma_ext_trig_en:1;
  uint32_t dma_ext_trig_id:2;
  uint32_t reserved3:5;
} s_dma_chan_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_chan_ctrl_type fields;
} u_dma_chan_ctrl_type;

typedef struct
{
  uint32_t dma_len:32;
} s_dma_len_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_len_type fields;
} u_dma_len_type;

typedef struct
{
  uint32_t dma_src:32;
} s_dma_src_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_src_type fields;
} u_dma_src_type;

typedef struct
{
  uint32_t dma_dst:32;
} s_dma_dst_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_dst_type fields;
} u_dma_dst_type;

typedef struct
{
  uint32_t dma_sdl:32;
} s_dma_sdl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_sdl_type fields;
} u_dma_sdl_type;

typedef struct
{
  uint32_t dma_skl:32;
} s_dma_skl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_skl_type fields;
} u_dma_skl_type;

typedef struct
{
  uint32_t dma_threshold:16;
  uint32_t reserved0:16;
} s_dma_threshold_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_dma_threshold_type fields;
} u_dma_threshold_type;

typedef struct
{
  u_dma_version_type dma_version;
  u_dma_int_status_type dma_int_status;
  u_dma_int_en_type dma_int_en;
  u_dma_int_set_type dma_int_set;
  u_dma_int_clear_type dma_int_clear;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
  union {
    struct {
      u_dma_chan_ctrl_type dma_chan_ctrl_0;
      u_dma_chan_ctrl_type dma_chan_ctrl_1;
      u_dma_chan_ctrl_type dma_chan_ctrl_2;
      u_dma_chan_ctrl_type dma_chan_ctrl_3;
      u_dma_chan_ctrl_type dma_chan_ctrl_4;
      u_dma_chan_ctrl_type dma_chan_ctrl_5;
      u_dma_chan_ctrl_type dma_chan_ctrl_6;
      u_dma_chan_ctrl_type dma_chan_ctrl_7;
    };
    u_dma_chan_ctrl_type dma_chan_ctrl[8];
  };
  union {
    struct {
      u_dma_len_type dma_len_0;
      u_dma_len_type dma_len_1;
      u_dma_len_type dma_len_2;
      u_dma_len_type dma_len_3;
      u_dma_len_type dma_len_4;
      u_dma_len_type dma_len_5;
      u_dma_len_type dma_len_6;
      u_dma_len_type dma_len_7;
    };
    u_dma_len_type dma_len[8];
  };
  union {
    struct {
      u_dma_src_type dma_src_0;
      u_dma_src_type dma_src_1;
      u_dma_src_type dma_src_2;
      u_dma_src_type dma_src_3;
      u_dma_src_type dma_src_4;
      u_dma_src_type dma_src_5;
      u_dma_src_type dma_src_6;
      u_dma_src_type dma_src_7;
    };
    u_dma_src_type dma_src[8];
  };
  union {
    struct {
      u_dma_dst_type dma_dst_0;
      u_dma_dst_type dma_dst_1;
      u_dma_dst_type dma_dst_2;
      u_dma_dst_type dma_dst_3;
      u_dma_dst_type dma_dst_4;
      u_dma_dst_type dma_dst_5;
      u_dma_dst_type dma_dst_6;
      u_dma_dst_type dma_dst_7;
    };
    u_dma_dst_type dma_dst[8];
  };
  union {
    struct {
      u_dma_sdl_type dma_sdl_0;
      u_dma_sdl_type dma_sdl_1;
      u_dma_sdl_type dma_sdl_2;
      u_dma_sdl_type dma_sdl_3;
      u_dma_sdl_type dma_sdl_4;
      u_dma_sdl_type dma_sdl_5;
      u_dma_sdl_type dma_sdl_6;
      u_dma_sdl_type dma_sdl_7;
    };
    u_dma_sdl_type dma_sdl[8];
  };
  union {
    struct {
      u_dma_skl_type dma_skl_0;
      u_dma_skl_type dma_skl_1;
      u_dma_skl_type dma_skl_2;
      u_dma_skl_type dma_skl_3;
      u_dma_skl_type dma_skl_4;
      u_dma_skl_type dma_skl_5;
      u_dma_skl_type dma_skl_6;
      u_dma_skl_type dma_skl_7;
    };
    u_dma_skl_type dma_skl[8];
  };
  union {
    struct {
      u_dma_threshold_type dma_threshold_0;
      u_dma_threshold_type dma_threshold_1;
      u_dma_threshold_type dma_threshold_2;
      u_dma_threshold_type dma_threshold_3;
      u_dma_threshold_type dma_threshold_4;
      u_dma_threshold_type dma_threshold_5;
      u_dma_threshold_type dma_threshold_6;
      u_dma_threshold_type dma_threshold_7;
    };
    u_dma_threshold_type dma_threshold[8];
  };
} s_dma_reg_type;

typedef s_dma_reg_type* p_dma_reg_type;

