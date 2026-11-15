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



#define NNPA_REV_MINOR_MASK         0x0000ffff
#define NNPA_REV_MINOR_OFFSET       0
#define NNPA_REV_MAJOR_MASK         0xffff0000
#define NNPA_REV_MAJOR_OFFSET       16
#define CONFIG_RELOAD_MASK          0x00000001
#define CONFIG_RELOAD_OFFSET        0
#define CONFIG_CHECK_MASK           0x00000002
#define CONFIG_CHECK_OFFSET         1
#define INPUT_LOADER_RST_MASK       0x00000004
#define INPUT_LOADER_RST_OFFSET     2
#define RUN_START_MASK              0x00000008
#define RUN_START_OFFSET            3
#define INF_COUNTER_CLEAR_MASK      0x00000010
#define INF_COUNTER_CLEAR_OFFSET    4
#define XTEN_PAUSE_RESUME_MASK      0x00000020
#define XTEN_PAUSE_RESUME_OFFSET    5
#define BLOCK_RESET_MASK            0x00000001
#define BLOCK_RESET_OFFSET          0
#define MANUAL_BUFF_PD_MASK         0x00000002
#define MANUAL_BUFF_PD_OFFSET       1
#define AUTO_START_EN_MASK          0x00000004
#define AUTO_START_EN_OFFSET        2
#define CPU_MEM_ACCESS_MASK         0x00000008
#define CPU_MEM_ACCESS_OFFSET       3
#define NNPA_CLK_FORCE_EN_MASK      0x00000010
#define NNPA_CLK_FORCE_EN_OFFSET    4
#define MEM_CLK_FORCE_EN_MASK       0x00000020
#define MEM_CLK_FORCE_EN_OFFSET     5
#define NNPA_BUSY_MASK              0x00000001
#define NNPA_BUSY_OFFSET            0
#define CONFIG_CHECK_OK_MASK        0x00000002
#define CONFIG_CHECK_OK_OFFSET      1
#define INPUT_LOADING_RDY_MASK      0x00000004
#define INPUT_LOADING_RDY_OFFSET    2
#define INPUT_LOADER_FULL_MASK      0x00000008
#define INPUT_LOADER_FULL_OFFSET    3
#define XTEN_PAUSE_STATE_MASK       0x00000010
#define XTEN_PAUSE_STATE_OFFSET     4
#define RUN_LAYER_IDX_MASK          0x0003ff00
#define RUN_LAYER_IDX_OFFSET        8
#define NB_DATA_BUFF_MASK           0x000000ff
#define NB_DATA_BUFF_OFFSET         0
#define CONFIG_START_MADDR_MASK     0x000fffff
#define CONFIG_START_MADDR_OFFSET   0
#define INPUT_START_MADDR_MASK      0x000fffff
#define INPUT_START_MADDR_OFFSET    0
#define DATA_START_MADDR_MASK       0x000fffff
#define DATA_START_MADDR_OFFSET     0
#define MEM_BUFF_PD0_MASK           0xffffffff
#define MEM_BUFF_PD0_OFFSET         0
#define MEM_BUFF_PD1_MASK           0xffffffff
#define MEM_BUFF_PD1_OFFSET         0
#define MEM_BUFF_SLEEP_AUTO_MASK    0x00000001
#define MEM_BUFF_SLEEP_AUTO_OFFSET  0
#define MEM_BUFF_SLEEP_EN_MASK      0x00000002
#define MEM_BUFF_SLEEP_EN_OFFSET    1
#define CUTS_CTRL_TEST1_MASK        0x00000001
#define CUTS_CTRL_TEST1_OFFSET      0
#define CUTS_CTRL_RME_MASK          0x00000002
#define CUTS_CTRL_RME_OFFSET        1
#define CUTS_CTRL_RM_MASK           0x0000003c
#define CUTS_CTRL_RM_OFFSET         2
#define CUTS_CTRL_RA_MASK           0x000000c0
#define CUTS_CTRL_RA_OFFSET         6
#define CUTS_CTRL_WA_MASK           0x00000700
#define CUTS_CTRL_WA_OFFSET         8
#define CUTS_CTRL_WPULSE_MASK       0x00003800
#define CUTS_CTRL_WPULSE_OFFSET     11
#define INPUT_LOADER_DATA_MASK      0xffffffff
#define INPUT_LOADER_DATA_OFFSET    0
#define LOADER_NBYTE_VALID_MASK     0x00000003
#define LOADER_NBYTE_VALID_OFFSET   0
#define LOADER_NBIT_MODE_MASK       0x0000001c
#define LOADER_NBIT_MODE_OFFSET     2
#define LOADER_SIGNED_DATA_MASK     0x00000040
#define LOADER_SIGNED_DATA_OFFSET   6
#define INPUT_LOADER_LEVEL_MASK     0x00ffffff
#define INPUT_LOADER_LEVEL_OFFSET   0
#define INPUT_SCALING_FXP_MASK      0x0000ffff
#define INPUT_SCALING_FXP_OFFSET    0
#define INPUT_OFFSET_INT_MASK       0xffff0000
#define INPUT_OFFSET_INT_OFFSET     16
#define RD_MODEL_ID_MASK            0x0000ffff
#define RD_MODEL_ID_OFFSET          0
#define RD_INPUT_SCALING_MASK       0x0000ffff
#define RD_INPUT_SCALING_OFFSET     0
#define RD_INPUT_OFFSET_MASK        0x0000ffff
#define RD_INPUT_OFFSET_OFFSET      0
#define RD_OUTPUT_SCALING_MASK      0x0000ffff
#define RD_OUTPUT_SCALING_OFFSET    0
#define RD_OUTPUT_OFFSET_MASK       0x0000ffff
#define RD_OUTPUT_OFFSET_OFFSET     0
#define RD_MODEL_SIZE_MASK          0x000fffff
#define RD_MODEL_SIZE_OFFSET        0
#define RD_INPUT_SIZE_MASK          0x000fffff
#define RD_INPUT_SIZE_OFFSET        0
#define RD_DATA_SIZE_MASK           0x000fffff
#define RD_DATA_SIZE_OFFSET         0
#define IFMAP_FLATTENED_MASK        0x00000001
#define IFMAP_FLATTENED_OFFSET      0
#define IFMAP_PACK_LEVEL_MASK       0x00000070
#define IFMAP_PACK_LEVEL_OFFSET     4
#define IFMAP_WIDTH_MASK            0x000003ff
#define IFMAP_WIDTH_OFFSET          0
#define IFMAP_HEIGHT_MASK           0x000ffc00
#define IFMAP_HEIGHT_OFFSET         10
#define IFMAP_DEPTH_MASK            0xfff00000
#define IFMAP_DEPTH_OFFSET          20
#define OFMAP_FLATTENED_MASK        0x00000001
#define OFMAP_FLATTENED_OFFSET      0
#define OFMAP_PACK_LEVEL_MASK       0x00000070
#define OFMAP_PACK_LEVEL_OFFSET     4
#define OFMAP_WIDTH_MASK            0x000003ff
#define OFMAP_WIDTH_OFFSET          0
#define OFMAP_HEIGHT_MASK           0x000ffc00
#define OFMAP_HEIGHT_OFFSET         10
#define OFMAP_DEPTH_MASK            0xfff00000
#define OFMAP_DEPTH_OFFSET          20
#define DATA_END_MADDR_MASK         0x000fffff
#define DATA_END_MADDR_OFFSET       0
#define OFMAP_START_MADDR_MASK      0x000fffff
#define OFMAP_START_MADDR_OFFSET    0
#define INFERENCE_COUNTER_MASK      0x00ffffff
#define INFERENCE_COUNTER_OFFSET    0
#define MEM_CONFLICT_SOURCE_MASK    0x0000000f
#define MEM_CONFLICT_SOURCE_OFFSET  0
#define MEM_CONFLICT_BUFFER_MASK    0x00000ff0
#define MEM_CONFLICT_BUFFER_OFFSET  4
#define XTEN_IFMAP_MADDR_MASK       0x0000ffff
#define XTEN_IFMAP_MADDR_OFFSET     0
#define XTEN_OFMAP_MADDR_MASK       0x0000ffff
#define XTEN_OFMAP_MADDR_OFFSET     0
#define XTEN_WEIGHT_MADDR_MASK      0x0000ffff
#define XTEN_WEIGHT_MADDR_OFFSET    0
#define CONFIG_CHECK_CRC_MASK       0x0000ffff
#define CONFIG_CHECK_CRC_OFFSET     0
#define NNPA_MBIST_RUN_MASK         0x000003ff
#define NNPA_MBIST_RUN_OFFSET       0
#define NNPA_MBIST_MODE_MASK        0x00010000
#define NNPA_MBIST_MODE_OFFSET      16
#define NNPA_MBIST_DONE_MASK        0x000003ff
#define NNPA_MBIST_DONE_OFFSET      0
#define NNPA_MBIST_ANY_FAIL_MASK    0x80000000
#define NNPA_MBIST_ANY_FAIL_OFFSET  31
#define NNPA_MBIST_FAIL0_MASK       0xffffffff
#define NNPA_MBIST_FAIL0_OFFSET     0
#define NNPA_MBIST_FAIL1_MASK       0xffffffff
#define NNPA_MBIST_FAIL1_OFFSET     0
#define NNPA_MBIST_FAIL2_MASK       0x0000ffff
#define NNPA_MBIST_FAIL2_OFFSET     0
#define EVT_DATA_LOADED_MASK        0x00000001
#define EVT_DATA_LOADED_OFFSET      0
#define EVT_CONFIG_CHECK_END_MASK   0x00000002
#define EVT_CONFIG_CHECK_END_OFFSET 1
#define EVT_EXEC_START_MASK         0x00000004
#define EVT_EXEC_START_OFFSET       2
#define EVT_EXEC_END_MASK           0x00000008
#define EVT_EXEC_END_OFFSET         3
#define EVT_XTEN_PAUSE_MASK         0x00000010
#define EVT_XTEN_PAUSE_OFFSET       4
#define EVT_MEM_CONFLICT_MASK       0x00000020
#define EVT_MEM_CONFLICT_OFFSET     5
#define EVT_INPUT_LOADER_OVF_MASK   0x00000040
#define EVT_INPUT_LOADER_OVF_OFFSET 6
#define EVT_ISP_SIZE_ERR_MASK       0x00000080
#define EVT_ISP_SIZE_ERR_OFFSET     7
#define NNPA_INT_EN_MASK            0x000000ff
#define NNPA_INT_EN_OFFSET          0
#define NNPA_INT_SET_MASK           0x000000ff
#define NNPA_INT_SET_OFFSET         0
#define NNPA_INT_CLEAR_MASK         0x000000ff
#define NNPA_INT_CLEAR_OFFSET       0

typedef struct
{
  uint32_t nnpa_rev_minor:16;
  uint32_t nnpa_rev_major:16;
} s_nnpa_version_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_version_type fields;
} u_nnpa_version_type;

typedef struct
{
  uint32_t config_reload:1;
  uint32_t config_check:1;
  uint32_t input_loader_rst:1;
  uint32_t run_start:1;
  uint32_t inf_counter_clear:1;
  uint32_t xten_pause_resume:1;
  uint32_t reserved0:26;
} s_nnpa_trig_control_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_trig_control_type fields;
} u_nnpa_trig_control_type;

typedef struct
{
  uint32_t block_reset:1;
  uint32_t manual_buff_pd:1;
  uint32_t auto_start_en:1;
  uint32_t cpu_mem_access:1;
  uint32_t nnpa_clk_force_en:1;
  uint32_t mem_clk_force_en:1;
  uint32_t reserved0:26;
} s_nnpa_control_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_control_type fields;
} u_nnpa_control_type;

typedef struct
{
  uint32_t nnpa_busy:1;
  uint32_t config_check_ok:1;
  uint32_t input_loading_rdy:1;
  uint32_t input_loader_full:1;
  uint32_t xten_pause_state:1;
  uint32_t reserved0:3;
  uint32_t run_layer_idx:10;
  uint32_t reserved1:14;
} s_nnpa_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_status_type fields;
} u_nnpa_status_type;

typedef struct
{
  uint32_t nb_data_buff:8;
  uint32_t reserved0:24;
} s_nb_data_buff_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nb_data_buff_type fields;
} u_nb_data_buff_type;

typedef struct
{
  uint32_t config_start_maddr:20;
  uint32_t reserved0:12;
} s_config_start_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_config_start_maddr_type fields;
} u_config_start_maddr_type;

typedef struct
{
  uint32_t input_start_maddr:20;
  uint32_t reserved0:12;
} s_input_start_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_input_start_maddr_type fields;
} u_input_start_maddr_type;

typedef struct
{
  uint32_t data_start_maddr:20;
  uint32_t reserved0:12;
} s_data_start_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_data_start_maddr_type fields;
} u_data_start_maddr_type;

typedef struct
{
  uint32_t mem_buff_pd0:32;
} s_mem_buff_pd0_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_mem_buff_pd0_type fields;
} u_mem_buff_pd0_type;

typedef struct
{
  uint32_t mem_buff_pd1:32;
} s_mem_buff_pd1_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_mem_buff_pd1_type fields;
} u_mem_buff_pd1_type;

typedef struct
{
  uint32_t mem_buff_sleep_auto:1;
  uint32_t mem_buff_sleep_en:1;
  uint32_t reserved0:30;
} s_mem_buff_sleep_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_mem_buff_sleep_type fields;
} u_mem_buff_sleep_type;

typedef struct
{
  uint32_t cuts_ctrl_test1:1;
  uint32_t cuts_ctrl_rme:1;
  uint32_t cuts_ctrl_rm:4;
  uint32_t cuts_ctrl_ra:2;
  uint32_t cuts_ctrl_wa:3;
  uint32_t cuts_ctrl_wpulse:3;
  uint32_t reserved0:18;
} s_mem_buff_cuts_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_mem_buff_cuts_ctrl_type fields;
} u_mem_buff_cuts_ctrl_type;

typedef struct
{
  uint32_t input_loader_data:32;
} s_input_loader_data_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_input_loader_data_type fields;
} u_input_loader_data_type;

typedef struct
{
  uint32_t loader_nbyte_valid:2;
  uint32_t loader_nbit_mode:3;
  uint32_t reserved0:1;
  uint32_t loader_signed_data:1;
  uint32_t reserved1:25;
} s_input_loader_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_input_loader_cfg_type fields;
} u_input_loader_cfg_type;

typedef struct
{
  uint32_t input_loader_level:24;
  uint32_t reserved0:8;
} s_input_loader_level_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_input_loader_level_type fields;
} u_input_loader_level_type;

typedef struct
{
  uint32_t input_scaling_fxp:16;
  uint32_t input_offset_int:16;
} s_input_data_rescale_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_input_data_rescale_type fields;
} u_input_data_rescale_type;

typedef struct
{
  uint32_t rd_model_id:16;
  uint32_t reserved0:16;
} s_rd_model_id_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_model_id_type fields;
} u_rd_model_id_type;

typedef struct
{
  uint32_t rd_input_scaling:16;
  uint32_t reserved0:16;
} s_rd_input_scaling_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_input_scaling_type fields;
} u_rd_input_scaling_type;

typedef struct
{
  uint32_t rd_input_offset:16;
  uint32_t reserved0:16;
} s_rd_input_offset_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_input_offset_type fields;
} u_rd_input_offset_type;

typedef struct
{
  uint32_t rd_output_scaling:16;
  uint32_t reserved0:16;
} s_rd_output_scaling_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_output_scaling_type fields;
} u_rd_output_scaling_type;

typedef struct
{
  uint32_t rd_output_offset:16;
  uint32_t reserved0:16;
} s_rd_output_offset_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_output_offset_type fields;
} u_rd_output_offset_type;

typedef struct
{
  uint32_t rd_model_size:20;
  uint32_t reserved0:12;
} s_rd_model_size_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_model_size_type fields;
} u_rd_model_size_type;

typedef struct
{
  uint32_t rd_input_size:20;
  uint32_t reserved0:12;
} s_rd_input_size_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_input_size_type fields;
} u_rd_input_size_type;

typedef struct
{
  uint32_t rd_data_size:20;
  uint32_t reserved0:12;
} s_rd_data_size_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_data_size_type fields;
} u_rd_data_size_type;

typedef struct
{
  uint32_t ifmap_flattened:1;
  uint32_t reserved0:3;
  uint32_t ifmap_pack_level:3;
  uint32_t reserved1:25;
} s_rd_ifmap_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_ifmap_info_type fields;
} u_rd_ifmap_info_type;

typedef struct
{
  uint32_t ifmap_width:10;
  uint32_t ifmap_height:10;
  uint32_t ifmap_depth:12;
} s_rd_ifmap_size_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_ifmap_size_type fields;
} u_rd_ifmap_size_type;

typedef struct
{
  uint32_t ofmap_flattened:1;
  uint32_t reserved0:3;
  uint32_t ofmap_pack_level:3;
  uint32_t reserved1:25;
} s_rd_ofmap_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_ofmap_info_type fields;
} u_rd_ofmap_info_type;

typedef struct
{
  uint32_t ofmap_width:10;
  uint32_t ofmap_height:10;
  uint32_t ofmap_depth:12;
} s_rd_ofmap_size_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_rd_ofmap_size_type fields;
} u_rd_ofmap_size_type;

typedef struct
{
  uint32_t data_end_maddr:20;
  uint32_t reserved0:12;
} s_data_end_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_data_end_maddr_type fields;
} u_data_end_maddr_type;

typedef struct
{
  uint32_t ofmap_start_maddr:20;
  uint32_t reserved0:12;
} s_ofmap_start_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_ofmap_start_maddr_type fields;
} u_ofmap_start_maddr_type;

typedef struct
{
  uint32_t inference_counter:24;
  uint32_t reserved0:8;
} s_inference_counter_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_inference_counter_type fields;
} u_inference_counter_type;

typedef struct
{
  uint32_t mem_conflict_source:4;
  uint32_t mem_conflict_buffer:8;
  uint32_t reserved0:20;
} s_mem_conflict_info_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_mem_conflict_info_type fields;
} u_mem_conflict_info_type;

typedef struct
{
  uint32_t xten_ifmap_maddr:16;
  uint32_t reserved0:16;
} s_xten_ifmap_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_xten_ifmap_maddr_type fields;
} u_xten_ifmap_maddr_type;

typedef struct
{
  uint32_t xten_ofmap_maddr:16;
  uint32_t reserved0:16;
} s_xten_ofmap_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_xten_ofmap_maddr_type fields;
} u_xten_ofmap_maddr_type;

typedef struct
{
  uint32_t xten_weight_maddr:16;
  uint32_t reserved0:16;
} s_xten_weight_maddr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_xten_weight_maddr_type fields;
} u_xten_weight_maddr_type;

typedef struct
{
  uint32_t config_check_crc:16;
  uint32_t reserved0:16;
} s_config_check_crc_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_config_check_crc_type fields;
} u_config_check_crc_type;

typedef struct
{
  uint32_t nnpa_mbist_run:10;
  uint32_t reserved0:6;
  uint32_t nnpa_mbist_mode:1;
  uint32_t reserved1:15;
} s_nnpa_mbist_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_mbist_ctrl_type fields;
} u_nnpa_mbist_ctrl_type;

typedef struct
{
  uint32_t nnpa_mbist_done:10;
  uint32_t reserved0:21;
  uint32_t nnpa_mbist_any_fail:1;
} s_nnpa_mbist_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_mbist_status_type fields;
} u_nnpa_mbist_status_type;

typedef struct
{
  uint32_t nnpa_mbist_fail0:32;
} s_nnpa_mbist_fail0_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_mbist_fail0_type fields;
} u_nnpa_mbist_fail0_type;

typedef struct
{
  uint32_t nnpa_mbist_fail1:32;
} s_nnpa_mbist_fail1_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_mbist_fail1_type fields;
} u_nnpa_mbist_fail1_type;

typedef struct
{
  uint32_t nnpa_mbist_fail2:16;
  uint32_t reserved0:16;
} s_nnpa_mbist_fail2_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_mbist_fail2_type fields;
} u_nnpa_mbist_fail2_type;

typedef struct
{
  uint32_t evt_data_loaded:1;
  uint32_t evt_config_check_end:1;
  uint32_t evt_exec_start:1;
  uint32_t evt_exec_end:1;
  uint32_t evt_xten_pause:1;
  uint32_t evt_mem_conflict:1;
  uint32_t evt_input_loader_ovf:1;
  uint32_t evt_isp_size_err:1;
  uint32_t reserved0:24;
} s_nnpa_int_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_int_status_type fields;
} u_nnpa_int_status_type;

typedef struct
{
  uint32_t nnpa_int_en:8;
  uint32_t reserved0:24;
} s_nnpa_int_en_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_int_en_type fields;
} u_nnpa_int_en_type;

typedef struct
{
  uint32_t nnpa_int_set:8;
  uint32_t reserved0:24;
} s_nnpa_int_set_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_int_set_type fields;
} u_nnpa_int_set_type;

typedef struct
{
  uint32_t nnpa_int_clear:8;
  uint32_t reserved0:24;
} s_nnpa_int_clear_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_nnpa_int_clear_type fields;
} u_nnpa_int_clear_type;

typedef struct
{
  u_nnpa_version_type nnpa_version;
  u_nnpa_trig_control_type nnpa_trig_control;
  u_nnpa_control_type nnpa_control;
  u_nnpa_status_type nnpa_status;
  u_nb_data_buff_type nb_data_buff;
  u_config_start_maddr_type config_start_maddr;
  u_input_start_maddr_type input_start_maddr;
  u_data_start_maddr_type data_start_maddr;
  u_mem_buff_pd0_type mem_buff_pd0;
  u_mem_buff_pd1_type mem_buff_pd1;
  u_mem_buff_sleep_type mem_buff_sleep;
  u_mem_buff_cuts_ctrl_type mem_buff_cuts_ctrl;
  u_input_loader_data_type input_loader_data;
  u_input_loader_cfg_type input_loader_cfg;
  u_input_loader_level_type input_loader_level;
  u_input_data_rescale_type input_data_rescale;
  u_rd_model_id_type rd_model_id;
  u_rd_input_scaling_type rd_input_scaling;
  u_rd_input_offset_type rd_input_offset;
  u_rd_output_scaling_type rd_output_scaling;
  u_rd_output_offset_type rd_output_offset;
  u_rd_model_size_type rd_model_size;
  u_rd_input_size_type rd_input_size;
  u_rd_data_size_type rd_data_size;
  u_rd_ifmap_info_type rd_ifmap_info;
  u_rd_ifmap_size_type rd_ifmap_size;
  u_rd_ofmap_info_type rd_ofmap_info;
  u_rd_ofmap_size_type rd_ofmap_size;
  u_data_end_maddr_type data_end_maddr;
  u_ofmap_start_maddr_type ofmap_start_maddr;
  u_inference_counter_type inference_counter;
  u_mem_conflict_info_type mem_conflict_info;
  u_xten_ifmap_maddr_type xten_ifmap_maddr;
  u_xten_ofmap_maddr_type xten_ofmap_maddr;
  u_xten_weight_maddr_type xten_weight_maddr;
  u_config_check_crc_type config_check_crc;
  u_nnpa_mbist_ctrl_type nnpa_mbist_ctrl;
  u_nnpa_mbist_status_type nnpa_mbist_status;
  u_nnpa_mbist_fail0_type nnpa_mbist_fail0;
  u_nnpa_mbist_fail1_type nnpa_mbist_fail1;
  u_nnpa_mbist_fail2_type nnpa_mbist_fail2;
  uint32_t reserved0;
  uint32_t reserved1;
  uint32_t reserved2;
  u_nnpa_int_status_type nnpa_int_status;
  u_nnpa_int_en_type nnpa_int_en;
  u_nnpa_int_set_type nnpa_int_set;
  u_nnpa_int_clear_type nnpa_int_clear;
} s_nnpa_reg_type;

typedef s_nnpa_reg_type* p_nnpa_reg_type;

