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



#define SCU_HF_CLK_SRC_MASK         0x00000001
#define SCU_HF_CLK_SRC_OFFSET       0
#define SCU_HF_CLK_ENFORCE_MASK     0x00000002
#define SCU_HF_CLK_ENFORCE_OFFSET   1
#define SCU_AXI_CLK_DIV_MASK        0x0000003f
#define SCU_AXI_CLK_DIV_OFFSET      0
#define SCU_APB_CLK_DIV_MASK        0x0000003f
#define SCU_APB_CLK_DIV_OFFSET      0
#define SCU_ISP_CLK_DIV_MASK        0x0000003f
#define SCU_ISP_CLK_DIV_OFFSET      0
#define SCU_ISP_CLK_SRC_MASK        0x00000100
#define SCU_ISP_CLK_SRC_OFFSET      8
#define SCU_ISP_CLK_ENFORCE_MASK    0x00000200
#define SCU_ISP_CLK_ENFORCE_OFFSET  9
#define SCU_NNPA_CLK_DIV_MASK       0x0000003f
#define SCU_NNPA_CLK_DIV_OFFSET     0
#define SCU_APB_CLK_EN_MASK         0x00000001
#define SCU_APB_CLK_EN_OFFSET       0
#define SCU_ISP_CLK_EN_MASK         0x00000002
#define SCU_ISP_CLK_EN_OFFSET       1
#define SCU_NNPA_CLK_EN_MASK        0x00000004
#define SCU_NNPA_CLK_EN_OFFSET      2
#define SCU_SPID_CLK_EN_MASK        0x00000008
#define SCU_SPID_CLK_EN_OFFSET      3
#define SCU_FFT_CLK_EN_MASK         0x00000010
#define SCU_FFT_CLK_EN_OFFSET       4
#define SCU_DBGM_CLK_EN_MASK        0x00000020
#define SCU_DBGM_CLK_EN_OFFSET      5
#define SCU_DMA_CLK_EN_MASK         0x00000040
#define SCU_DMA_CLK_EN_OFFSET       6
#define SCU_CAMIF_CLK_EN_MASK       0x00000080
#define SCU_CAMIF_CLK_EN_OFFSET     7
#define SCU_PORT_CLK_EN_MASK        0x00000100
#define SCU_PORT_CLK_EN_OFFSET      8
#define SCU_SPIS_CLK_EN_MASK        0x00000200
#define SCU_SPIS_CLK_EN_OFFSET      9
#define SCU_SPIMF_CLK_EN_MASK       0x00000400
#define SCU_SPIMF_CLK_EN_OFFSET     10
#define SCU_SPIM0_CLK_EN_MASK       0x00000800
#define SCU_SPIM0_CLK_EN_OFFSET     11
#define SCU_SPIM1_CLK_EN_MASK       0x00001000
#define SCU_SPIM1_CLK_EN_OFFSET     12
#define SCU_UART_CLK_EN_MASK        0x00002000
#define SCU_UART_CLK_EN_OFFSET      13
#define SCU_I2CM_CLK_EN_MASK        0x00004000
#define SCU_I2CM_CLK_EN_OFFSET      14
#define SCU_TIMER0_CLK_EN_MASK      0x00008000
#define SCU_TIMER0_CLK_EN_OFFSET    15
#define SCU_TIMER1_CLK_EN_MASK      0x00010000
#define SCU_TIMER1_CLK_EN_OFFSET    16
#define SCU_RTC_CLK_EN_MASK         0x00020000
#define SCU_RTC_CLK_EN_OFFSET       17
#define SCU_APB_RESET_MASK          0x00000001
#define SCU_APB_RESET_OFFSET        0
#define SCU_ISP_RESET_MASK          0x00000002
#define SCU_ISP_RESET_OFFSET        1
#define SCU_NNPA_RESET_MASK         0x00000004
#define SCU_NNPA_RESET_OFFSET       2
#define SCU_SPID_RESET_MASK         0x00000008
#define SCU_SPID_RESET_OFFSET       3
#define SCU_FFT_RESET_MASK          0x00000010
#define SCU_FFT_RESET_OFFSET        4
#define SCU_DBGM_RESET_MASK         0x00000020
#define SCU_DBGM_RESET_OFFSET       5
#define SCU_DMA_RESET_MASK          0x00000040
#define SCU_DMA_RESET_OFFSET        6
#define SCU_CAMIF_RESET_MASK        0x00000080
#define SCU_CAMIF_RESET_OFFSET      7
#define SCU_PORT_RESET_MASK         0x00000100
#define SCU_PORT_RESET_OFFSET       8
#define SCU_SPIS_RESET_MASK         0x00000200
#define SCU_SPIS_RESET_OFFSET       9
#define SCU_SPIMF_RESET_MASK        0x00000400
#define SCU_SPIMF_RESET_OFFSET      10
#define SCU_SPIM0_RESET_MASK        0x00000800
#define SCU_SPIM0_RESET_OFFSET      11
#define SCU_SPIM1_RESET_MASK        0x00001000
#define SCU_SPIM1_RESET_OFFSET      12
#define SCU_UART_RESET_MASK         0x00002000
#define SCU_UART_RESET_OFFSET       13
#define SCU_I2CM_RESET_MASK         0x00004000
#define SCU_I2CM_RESET_OFFSET       14
#define SCU_TIMER0_RESET_MASK       0x00008000
#define SCU_TIMER0_RESET_OFFSET     15
#define SCU_TIMER1_RESET_MASK       0x00010000
#define SCU_TIMER1_RESET_OFFSET     16
#define SCU_RTC_RESET_MASK          0x00020000
#define SCU_RTC_RESET_OFFSET        17
#define SCU_CLK_OUT0_SRC_MASK       0x00000003
#define SCU_CLK_OUT0_SRC_OFFSET     0
#define SCU_CLK_OUT0_DIV_MASK       0x0000ff00
#define SCU_CLK_OUT0_DIV_OFFSET     8
#define SCU_CLK_OUT1_SRC_MASK       0x00000003
#define SCU_CLK_OUT1_SRC_OFFSET     0
#define SCU_CLK_OUT1_DIV_MASK       0x0000ff00
#define SCU_CLK_OUT1_DIV_OFFSET     8
#define SCU_GLOBAL_RESET_MASK       0x00000001
#define SCU_GLOBAL_RESET_OFFSET     0
#define SCU_SLEEP_REQ_MASK          0x00000002
#define SCU_SLEEP_REQ_OFFSET        1
#define SCU_SLEEP_ENABLE_MASK       0x00000001
#define SCU_SLEEP_ENABLE_OFFSET     0
#define SCU_PLL_AUTO_SLEEP_MASK     0x00000002
#define SCU_PLL_AUTO_SLEEP_OFFSET   1
#define SCU_WDT_RESET_MODE_MASK     0x00000030
#define SCU_WDT_RESET_MODE_OFFSET   4
#define SCU_CORE_RUN_EN_MASK        0x00000001
#define SCU_CORE_RUN_EN_OFFSET      0
#define SCU_CORE_RESET_MASK         0x00000002
#define SCU_CORE_RESET_OFFSET       1
#define SCU_BOOT_ADDR_MASK          0xffffffff
#define SCU_BOOT_ADDR_OFFSET        0
#define SCU_BOOT_ADDR_OVW_MASK      0xffffffff
#define SCU_BOOT_ADDR_OVW_OFFSET    0
#define SCU_BOOT_ADDR_SET_MASK      0x00000001
#define SCU_BOOT_ADDR_SET_OFFSET    0
#define SCU_CORE_STATUS_MASK        0xffffffff
#define SCU_CORE_STATUS_OFFSET      0
#define SCU_CORE_MSG_MASK           0xffffffff
#define SCU_CORE_MSG_OFFSET         0
#define SCU_EXT_MSG_MASK            0xffffffff
#define SCU_EXT_MSG_OFFSET          0
#define SCU_GENERATE_IRQ_MASK       0x00000001
#define SCU_GENERATE_IRQ_OFFSET     0
#define SCU_WR_PROT_START_MASK      0xffffffff
#define SCU_WR_PROT_START_OFFSET    0
#define SCU_WR_PROT_END_MASK        0xffffffff
#define SCU_WR_PROT_END_OFFSET      0
#define SRAM_CUTS_TEST1_MASK        0x00000001
#define SRAM_CUTS_TEST1_OFFSET      0
#define SRAM_CUTS_RME_MASK          0x00000002
#define SRAM_CUTS_RME_OFFSET        1
#define SRAM_CUTS_RM_MASK           0x0000003c
#define SRAM_CUTS_RM_OFFSET         2
#define SRAM_CUTS_RA_MASK           0x000000c0
#define SRAM_CUTS_RA_OFFSET         6
#define SRAM_CUTS_WA_MASK           0x00000700
#define SRAM_CUTS_WA_OFFSET         8
#define SRAM_CUTS_WPULSE_MASK       0x00003800
#define SRAM_CUTS_WPULSE_OFFSET     11
#define SRAM_CUTS_PD_CTRL_MASK      0xffffffff
#define SRAM_CUTS_PD_CTRL_OFFSET    0
#define SRAM_FORCE_CLK_EN_MASK      0x00000001
#define SRAM_FORCE_CLK_EN_OFFSET    0
#define SRAM_MBIST_RUN_MASK         0x0000000f
#define SRAM_MBIST_RUN_OFFSET       0
#define SRAM_MBIST_DONE_MASK        0x0000000f
#define SRAM_MBIST_DONE_OFFSET      0
#define SRAM_MBIST_ANY_FAIL_MASK    0x80000000
#define SRAM_MBIST_ANY_FAIL_OFFSET  31
#define SRAM_MBIST_FAIL_MASK        0xffffffff
#define SRAM_MBIST_FAIL_OFFSET      0
#define PLL_EN_MODE_MASK            0x00000003
#define PLL_EN_MODE_OFFSET          0
#define PLL_AUTO_CAL_MASK           0x00000008
#define PLL_AUTO_CAL_OFFSET         3
#define PLL_FREQ_MULT_MASK          0x000007f0
#define PLL_FREQ_MULT_OFFSET        4
#define PLL_POST_DIV_MASK           0x0000f000
#define PLL_POST_DIV_OFFSET         12
#define PLL_RESTART_MASK            0x00010000
#define PLL_RESTART_OFFSET          16
#define PLL_CAL_PERIODS_MASK        0x000003ff
#define PLL_CAL_PERIODS_OFFSET      0
#define PLL_CAL_TARGET_MASK         0x000ffc00
#define PLL_CAL_TARGET_OFFSET       10
#define PLL_CAL_START_DLY_MASK      0x3ff00000
#define PLL_CAL_START_DLY_OFFSET    20
#define PLL_CAL_BYPASS_EN_MASK      0x00000001
#define PLL_CAL_BYPASS_EN_OFFSET    0
#define PLL_CAL_BYPASS_VAL_MASK     0x00000ff0
#define PLL_CAL_BYPASS_VAL_OFFSET   4
#define PLL_GAIN_BYPASS_VAL_MASK    0x00003000
#define PLL_GAIN_BYPASS_VAL_OFFSET  12
#define PLL_ANA_CONFIG_MASK         0x000000ff
#define PLL_ANA_CONFIG_OFFSET       0
#define PLL_CTRL_IN_EN_MASK         0x00000100
#define PLL_CTRL_IN_EN_OFFSET       8
#define PLL_FORCE_EN_MASK           0x00000200
#define PLL_FORCE_EN_OFFSET         9
#define PLL_FORCE_CLOSEDLOOP_MASK   0x00000400
#define PLL_FORCE_CLOSEDLOOP_OFFSET 10
#define PLL_CAL_STATE_MASK          0x00000007
#define PLL_CAL_STATE_OFFSET        0
#define PLL_CAL_RUNNING_MASK        0x00000010
#define PLL_CAL_RUNNING_OFFSET      4
#define PLL_LOCKED_MASK             0x00000020
#define PLL_LOCKED_OFFSET           5
#define PLL_VVCO_FLAG_L_MASK        0x00000040
#define PLL_VVCO_FLAG_L_OFFSET      6
#define PLL_VVCO_FLAG_H_MASK        0x00000080
#define PLL_VVCO_FLAG_H_OFFSET      7
#define PLL_COARSE_TUNE_MASK        0x0000ff00
#define PLL_COARSE_TUNE_OFFSET      8
#define PLL_CAL_MEAS_MASK           0x03ff0000
#define PLL_CAL_MEAS_OFFSET         16
#define CHIP_REV_MINOR_MASK         0x0000ffff
#define CHIP_REV_MINOR_OFFSET       0
#define CHIP_REV_MAJOR_MASK         0xffff0000
#define CHIP_REV_MAJOR_OFFSET       16

typedef struct
{
  uint32_t scu_hf_clk_src:1;
  uint32_t scu_hf_clk_enforce:1;
  uint32_t reserved0:30;
} s_scu_hf_clk_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_hf_clk_cfg_type fields;
} u_scu_hf_clk_cfg_type;

typedef struct
{
  uint32_t scu_axi_clk_div:6;
  uint32_t reserved0:26;
} s_scu_axi_clk_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_axi_clk_cfg_type fields;
} u_scu_axi_clk_cfg_type;

typedef struct
{
  uint32_t scu_apb_clk_div:6;
  uint32_t reserved0:26;
} s_scu_apb_clk_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_apb_clk_cfg_type fields;
} u_scu_apb_clk_cfg_type;

typedef struct
{
  uint32_t scu_isp_clk_div:6;
  uint32_t reserved0:2;
  uint32_t scu_isp_clk_src:1;
  uint32_t scu_isp_clk_enforce:1;
  uint32_t reserved1:22;
} s_scu_isp_clk_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_isp_clk_cfg_type fields;
} u_scu_isp_clk_cfg_type;

typedef struct
{
  uint32_t scu_nnpa_clk_div:6;
  uint32_t reserved0:26;
} s_scu_nnpa_clk_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_nnpa_clk_cfg_type fields;
} u_scu_nnpa_clk_cfg_type;

typedef struct
{
  uint32_t scu_apb_clk_en:1;
  uint32_t scu_isp_clk_en:1;
  uint32_t scu_nnpa_clk_en:1;
  uint32_t scu_spid_clk_en:1;
  uint32_t scu_fft_clk_en:1;
  uint32_t scu_dbgm_clk_en:1;
  uint32_t scu_dma_clk_en:1;
  uint32_t scu_camif_clk_en:1;
  uint32_t scu_port_clk_en:1;
  uint32_t scu_spis_clk_en:1;
  uint32_t scu_spimf_clk_en:1;
  uint32_t scu_spim0_clk_en:1;
  uint32_t scu_spim1_clk_en:1;
  uint32_t scu_uart_clk_en:1;
  uint32_t scu_i2cm_clk_en:1;
  uint32_t scu_timer0_clk_en:1;
  uint32_t scu_timer1_clk_en:1;
  uint32_t scu_rtc_clk_en:1;
  uint32_t reserved0:14;
} s_scu_periph_clk_en_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_periph_clk_en_type fields;
} u_scu_periph_clk_en_type;

typedef struct
{
  uint32_t scu_apb_reset:1;
  uint32_t scu_isp_reset:1;
  uint32_t scu_nnpa_reset:1;
  uint32_t scu_spid_reset:1;
  uint32_t scu_fft_reset:1;
  uint32_t scu_dbgm_reset:1;
  uint32_t scu_dma_reset:1;
  uint32_t scu_camif_reset:1;
  uint32_t scu_port_reset:1;
  uint32_t scu_spis_reset:1;
  uint32_t scu_spimf_reset:1;
  uint32_t scu_spim0_reset:1;
  uint32_t scu_spim1_reset:1;
  uint32_t scu_uart_reset:1;
  uint32_t scu_i2cm_reset:1;
  uint32_t scu_timer0_reset:1;
  uint32_t scu_timer1_reset:1;
  uint32_t scu_rtc_reset:1;
  uint32_t reserved0:14;
} s_scu_periph_reset_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_periph_reset_type fields;
} u_scu_periph_reset_type;

typedef struct
{
  uint32_t scu_clk_out0_src:2;
  uint32_t reserved0:6;
  uint32_t scu_clk_out0_div:8;
  uint32_t reserved1:16;
} s_scu_clk_out0_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_clk_out0_cfg_type fields;
} u_scu_clk_out0_cfg_type;

typedef struct
{
  uint32_t scu_clk_out1_src:2;
  uint32_t reserved0:6;
  uint32_t scu_clk_out1_div:8;
  uint32_t reserved1:16;
} s_scu_clk_out1_cfg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_clk_out1_cfg_type fields;
} u_scu_clk_out1_cfg_type;

typedef struct
{
  uint32_t scu_global_reset:1;
  uint32_t scu_sleep_req:1;
  uint32_t reserved0:30;
} s_scu_system_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_system_ctrl_type fields;
} u_scu_system_ctrl_type;

typedef struct
{
  uint32_t scu_sleep_enable:1;
  uint32_t scu_pll_auto_sleep:1;
  uint32_t reserved0:2;
  uint32_t scu_wdt_reset_mode:2;
  uint32_t reserved1:26;
} s_scu_system_config_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_system_config_type fields;
} u_scu_system_config_type;

typedef struct
{
  uint32_t scu_core_run_en:1;
  uint32_t scu_core_reset:1;
  uint32_t reserved0:30;
} s_scu_core_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_core_ctrl_type fields;
} u_scu_core_ctrl_type;

typedef struct
{
  uint32_t scu_boot_addr:32;
} s_scu_boot_addr_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_boot_addr_type fields;
} u_scu_boot_addr_type;

typedef struct
{
  uint32_t scu_boot_addr_ovw:32;
} s_scu_boot_addr_ovw_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_boot_addr_ovw_type fields;
} u_scu_boot_addr_ovw_type;

typedef struct
{
  uint32_t scu_boot_addr_set:1;
  uint32_t reserved0:31;
} s_scu_boot_addr_set_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_boot_addr_set_type fields;
} u_scu_boot_addr_set_type;

typedef struct
{
  uint32_t scu_core_status:32;
} s_scu_core_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_core_status_type fields;
} u_scu_core_status_type;

typedef struct
{
  uint32_t scu_core_msg:32;
} s_scu_core_msg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_core_msg_type fields;
} u_scu_core_msg_type;

typedef struct
{
  uint32_t scu_ext_msg:32;
} s_scu_ext_msg_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_ext_msg_type fields;
} u_scu_ext_msg_type;

typedef struct
{
  uint32_t scu_generate_irq:1;
  uint32_t reserved0:31;
} s_scu_generate_irq_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_generate_irq_type fields;
} u_scu_generate_irq_type;

typedef struct
{
  uint32_t scu_wr_prot_start:32;
} s_scu_wr_prot_start_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_wr_prot_start_type fields;
} u_scu_wr_prot_start_type;

typedef struct
{
  uint32_t scu_wr_prot_end:32;
} s_scu_wr_prot_end_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_scu_wr_prot_end_type fields;
} u_scu_wr_prot_end_type;

typedef struct
{
  uint32_t sram_cuts_test1:1;
  uint32_t sram_cuts_rme:1;
  uint32_t sram_cuts_rm:4;
  uint32_t sram_cuts_ra:2;
  uint32_t sram_cuts_wa:3;
  uint32_t sram_cuts_wpulse:3;
  uint32_t reserved0:18;
} s_sram_cuts_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_sram_cuts_ctrl_type fields;
} u_sram_cuts_ctrl_type;

typedef struct
{
  uint32_t sram_cuts_pd_ctrl:32;
} s_sram_cuts_pd_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_sram_cuts_pd_ctrl_type fields;
} u_sram_cuts_pd_ctrl_type;

typedef struct
{
  uint32_t sram_force_clk_en:1;
  uint32_t reserved0:31;
} s_sram_force_clk_en_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_sram_force_clk_en_type fields;
} u_sram_force_clk_en_type;

typedef struct
{
  uint32_t sram_mbist_run:4;
  uint32_t reserved0:28;
} s_sram_mbist_ctrl_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_sram_mbist_ctrl_type fields;
} u_sram_mbist_ctrl_type;

typedef struct
{
  uint32_t sram_mbist_done:4;
  uint32_t reserved0:27;
  uint32_t sram_mbist_any_fail:1;
} s_sram_mbist_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_sram_mbist_status_type fields;
} u_sram_mbist_status_type;

typedef struct
{
  uint32_t sram_mbist_fail:32;
} s_sram_mbist_fail_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_sram_mbist_fail_type fields;
} u_sram_mbist_fail_type;

typedef struct
{
  uint32_t pll_en_mode:2;
  uint32_t reserved0:1;
  uint32_t pll_auto_cal:1;
  uint32_t pll_freq_mult:7;
  uint32_t reserved1:1;
  uint32_t pll_post_div:4;
  uint32_t pll_restart:1;
  uint32_t reserved2:15;
} s_pll_control_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_pll_control_type fields;
} u_pll_control_type;

typedef struct
{
  uint32_t pll_cal_periods:10;
  uint32_t pll_cal_target:10;
  uint32_t pll_cal_start_dly:10;
  uint32_t reserved0:2;
} s_pll_cal_config_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_pll_cal_config_type fields;
} u_pll_cal_config_type;

typedef struct
{
  uint32_t pll_cal_bypass_en:1;
  uint32_t reserved0:3;
  uint32_t pll_cal_bypass_val:8;
  uint32_t pll_gain_bypass_val:2;
  uint32_t reserved1:18;
} s_pll_cal_bypass_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_pll_cal_bypass_type fields;
} u_pll_cal_bypass_type;

typedef struct
{
  uint32_t pll_ana_config:8;
  uint32_t pll_ctrl_in_en:1;
  uint32_t pll_force_en:1;
  uint32_t pll_force_closedloop:1;
  uint32_t reserved0:21;
} s_pll_ana_control_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_pll_ana_control_type fields;
} u_pll_ana_control_type;

typedef struct
{
  uint32_t pll_cal_state:3;
  uint32_t reserved0:1;
  uint32_t pll_cal_running:1;
  uint32_t pll_locked:1;
  uint32_t pll_vvco_flag_l:1;
  uint32_t pll_vvco_flag_h:1;
  uint32_t pll_coarse_tune:8;
  uint32_t pll_cal_meas:10;
  uint32_t reserved1:6;
} s_pll_status_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_pll_status_type fields;
} u_pll_status_type;

typedef struct
{
  uint32_t chip_rev_minor:16;
  uint32_t chip_rev_major:16;
} s_chip_version_type;

typedef union
{
  uint32_t word;
  uint16_t hword[2];
  uint8_t  byte[4];
  s_chip_version_type fields;
} u_chip_version_type;

typedef struct
{
  u_scu_hf_clk_cfg_type scu_hf_clk_cfg;
  u_scu_axi_clk_cfg_type scu_axi_clk_cfg;
  u_scu_apb_clk_cfg_type scu_apb_clk_cfg;
  u_scu_isp_clk_cfg_type scu_isp_clk_cfg;
  u_scu_nnpa_clk_cfg_type scu_nnpa_clk_cfg;
  u_scu_periph_clk_en_type scu_periph_clk_en;
  u_scu_periph_reset_type scu_periph_reset;
  u_scu_clk_out0_cfg_type scu_clk_out0_cfg;
  u_scu_clk_out1_cfg_type scu_clk_out1_cfg;
  u_scu_system_ctrl_type scu_system_ctrl;
  u_scu_system_config_type scu_system_config;
  uint32_t reserved0;
  u_scu_core_ctrl_type scu_core_ctrl;
  u_scu_boot_addr_type scu_boot_addr;
  u_scu_boot_addr_ovw_type scu_boot_addr_ovw;
  u_scu_boot_addr_set_type scu_boot_addr_set;
  u_scu_core_status_type scu_core_status;
  uint32_t reserved1;
  uint32_t reserved2;
  uint32_t reserved3;
  union {
    struct {
      u_scu_core_msg_type scu_core_msg_0;
      u_scu_core_msg_type scu_core_msg_1;
      u_scu_core_msg_type scu_core_msg_2;
      u_scu_core_msg_type scu_core_msg_3;
    };
    u_scu_core_msg_type scu_core_msg[4];
  };
  uint32_t reserved4;
  uint32_t reserved5;
  uint32_t reserved6;
  uint32_t reserved7;
  union {
    struct {
      u_scu_ext_msg_type scu_ext_msg_0;
      u_scu_ext_msg_type scu_ext_msg_1;
      u_scu_ext_msg_type scu_ext_msg_2;
      u_scu_ext_msg_type scu_ext_msg_3;
    };
    u_scu_ext_msg_type scu_ext_msg[4];
  };
  uint32_t reserved8;
  uint32_t reserved9;
  uint32_t reserved10;
  uint32_t reserved11;
  u_scu_generate_irq_type scu_generate_irq;
  u_scu_wr_prot_start_type scu_wr_prot_start;
  u_scu_wr_prot_end_type scu_wr_prot_end;
  u_sram_cuts_ctrl_type sram_cuts_ctrl;
  u_sram_cuts_pd_ctrl_type sram_cuts_pd_ctrl;
  u_sram_force_clk_en_type sram_force_clk_en;
  uint32_t reserved12;
  uint32_t reserved13;
  u_sram_mbist_ctrl_type sram_mbist_ctrl;
  u_sram_mbist_status_type sram_mbist_status;
  u_sram_mbist_fail_type sram_mbist_fail;
  uint32_t reserved14;
  u_pll_control_type pll_control;
  u_pll_cal_config_type pll_cal_config;
  u_pll_cal_bypass_type pll_cal_bypass;
  u_pll_ana_control_type pll_ana_control;
  u_pll_status_type pll_status;
  uint32_t reserved15;
  uint32_t reserved16;
  uint32_t reserved17;
  uint32_t reserved18;
  uint32_t reserved19;
  uint32_t reserved20;
  uint32_t reserved21;
  u_chip_version_type chip_version;
} s_scu_reg_type;

typedef s_scu_reg_type* p_scu_reg_type;

