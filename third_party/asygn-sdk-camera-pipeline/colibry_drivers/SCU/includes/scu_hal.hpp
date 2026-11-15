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



#pragma once

#include <stdint.h>

#include "scu_mmap.h"


/// @brief HAL script-generated layer of scu register bank.
/// @note This code needs a C++ compiler to be used.
class ScuHal {
 public:
  volatile s_scu_reg_type* regmap;

  ScuHal() {
  }

  void inline set_periph_addr(uint32_t address) {
    regmap = (s_scu_reg_type*) address;
  }

  /** @brief HAL Getter of signal SCU_HF_CLK_SRC from register SCU_HF_CLK_CFG
  *
  * High-frequency clock source:
  * 0: PLL
  * 1: External
  *
  * @return Register value
  */
  uint32_t inline scu_hf_clk_src_get() {
    return regmap->scu_hf_clk_cfg.fields.scu_hf_clk_src;
  }

  /** @brief HAL Setter of signal SCU_HF_CLK_SRC from register SCU_HF_CLK_CFG
  *
  * High-frequency clock source:
  * 0: PLL
  * 1: External
  *
  * @param scu_hf_clk_src Value to be set
  */
  void inline scu_hf_clk_src_set(uint32_t scu_hf_clk_src) {
    regmap->scu_hf_clk_cfg.fields.scu_hf_clk_src = scu_hf_clk_src;
  }

  /** @brief HAL Getter of signal SCU_HF_CLK_ENFORCE from register SCU_HF_CLK_CFG
  *
  * Bypass clock switcher and force selected clock at output
  *
  * @return Register value
  */
  uint32_t inline scu_hf_clk_enforce_get() {
    return regmap->scu_hf_clk_cfg.fields.scu_hf_clk_enforce;
  }

  /** @brief HAL Setter of signal SCU_HF_CLK_ENFORCE from register SCU_HF_CLK_CFG
  *
  * Bypass clock switcher and force selected clock at output
  *
  * @param scu_hf_clk_enforce Value to be set
  */
  void inline scu_hf_clk_enforce_set(uint32_t scu_hf_clk_enforce) {
    regmap->scu_hf_clk_cfg.fields.scu_hf_clk_enforce = scu_hf_clk_enforce;
  }

  /** @brief HAL Getter of signal SCU_AXI_CLK_DIV from register SCU_AXI_CLK_CFG
  *
  * AXI clock frequency division ratio minus 1
  *
  * @return Register value
  */
  uint32_t inline scu_axi_clk_div_get() {
    return regmap->scu_axi_clk_cfg.fields.scu_axi_clk_div;
  }

  /** @brief HAL Setter of signal SCU_AXI_CLK_DIV from register SCU_AXI_CLK_CFG
  *
  * AXI clock frequency division ratio minus 1
  *
  * @param scu_axi_clk_div Value to be set
  */
  void inline scu_axi_clk_div_set(uint32_t scu_axi_clk_div) {
    regmap->scu_axi_clk_cfg.fields.scu_axi_clk_div = scu_axi_clk_div;
  }

  /** @brief HAL Getter of signal SCU_APB_CLK_DIV from register SCU_APB_CLK_CFG
  *
  * APB clock frequency division ratio minus 1
  *
  * @return Register value
  */
  uint32_t inline scu_apb_clk_div_get() {
    return regmap->scu_apb_clk_cfg.fields.scu_apb_clk_div;
  }

  /** @brief HAL Setter of signal SCU_APB_CLK_DIV from register SCU_APB_CLK_CFG
  *
  * APB clock frequency division ratio minus 1
  *
  * @param scu_apb_clk_div Value to be set
  */
  void inline scu_apb_clk_div_set(uint32_t scu_apb_clk_div) {
    regmap->scu_apb_clk_cfg.fields.scu_apb_clk_div = scu_apb_clk_div;
  }

  /** @brief HAL Getter of signal SCU_ISP_CLK_DIV from register SCU_ISP_CLK_CFG
  *
  * ISP clock frequency division ratio minus 1
  *
  * @return Register value
  */
  uint32_t inline scu_isp_clk_div_get() {
    return regmap->scu_isp_clk_cfg.fields.scu_isp_clk_div;
  }

  /** @brief HAL Setter of signal SCU_ISP_CLK_DIV from register SCU_ISP_CLK_CFG
  *
  * ISP clock frequency division ratio minus 1
  *
  * @param scu_isp_clk_div Value to be set
  */
  void inline scu_isp_clk_div_set(uint32_t scu_isp_clk_div) {
    regmap->scu_isp_clk_cfg.fields.scu_isp_clk_div = scu_isp_clk_div;
  }

  /** @brief HAL Getter of signal SCU_ISP_CLK_SRC from register SCU_ISP_CLK_CFG
  *
  * ISP clock source:
  * 0: CAM_PCLK
  * 1: HF_CLK
  *
  * @return Register value
  */
  uint32_t inline scu_isp_clk_src_get() {
    return regmap->scu_isp_clk_cfg.fields.scu_isp_clk_src;
  }

  /** @brief HAL Setter of signal SCU_ISP_CLK_SRC from register SCU_ISP_CLK_CFG
  *
  * ISP clock source:
  * 0: CAM_PCLK
  * 1: HF_CLK
  *
  * @param scu_isp_clk_src Value to be set
  */
  void inline scu_isp_clk_src_set(uint32_t scu_isp_clk_src) {
    regmap->scu_isp_clk_cfg.fields.scu_isp_clk_src = scu_isp_clk_src;
  }

  /** @brief HAL Getter of signal SCU_ISP_CLK_ENFORCE from register SCU_ISP_CLK_CFG
  *
  * Bypass clock switcher and force selected clock at output
  *
  * @return Register value
  */
  uint32_t inline scu_isp_clk_enforce_get() {
    return regmap->scu_isp_clk_cfg.fields.scu_isp_clk_enforce;
  }

  /** @brief HAL Setter of signal SCU_ISP_CLK_ENFORCE from register SCU_ISP_CLK_CFG
  *
  * Bypass clock switcher and force selected clock at output
  *
  * @param scu_isp_clk_enforce Value to be set
  */
  void inline scu_isp_clk_enforce_set(uint32_t scu_isp_clk_enforce) {
    regmap->scu_isp_clk_cfg.fields.scu_isp_clk_enforce = scu_isp_clk_enforce;
  }

  /** @brief HAL Getter of signal SCU_NNPA_CLK_DIV from register SCU_NNPA_CLK_CFG
  *
  * NNPA clock frequency division ratio minus 1
  *
  * @return Register value
  */
  uint32_t inline scu_nnpa_clk_div_get() {
    return regmap->scu_nnpa_clk_cfg.fields.scu_nnpa_clk_div;
  }

  /** @brief HAL Setter of signal SCU_NNPA_CLK_DIV from register SCU_NNPA_CLK_CFG
  *
  * NNPA clock frequency division ratio minus 1
  *
  * @param scu_nnpa_clk_div Value to be set
  */
  void inline scu_nnpa_clk_div_set(uint32_t scu_nnpa_clk_div) {
    regmap->scu_nnpa_clk_cfg.fields.scu_nnpa_clk_div = scu_nnpa_clk_div;
  }

  /** @brief HAL Getter of signal SCU_APB_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * APB soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_apb_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_apb_clk_en;
  }

  /** @brief HAL Setter of signal SCU_APB_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * APB soft reset
  *
  * @param scu_apb_clk_en Value to be set
  */
  void inline scu_apb_clk_en_set(uint32_t scu_apb_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_apb_clk_en = scu_apb_clk_en;
  }

  /** @brief HAL Getter of signal SCU_ISP_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * ISP clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_isp_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_isp_clk_en;
  }

  /** @brief HAL Setter of signal SCU_ISP_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * ISP clock enable
  *
  * @param scu_isp_clk_en Value to be set
  */
  void inline scu_isp_clk_en_set(uint32_t scu_isp_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_isp_clk_en = scu_isp_clk_en;
  }

  /** @brief HAL Getter of signal SCU_NNPA_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * NNPA clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_nnpa_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_nnpa_clk_en;
  }

  /** @brief HAL Setter of signal SCU_NNPA_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * NNPA clock enable
  *
  * @param scu_nnpa_clk_en Value to be set
  */
  void inline scu_nnpa_clk_en_set(uint32_t scu_nnpa_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_nnpa_clk_en = scu_nnpa_clk_en;
  }

  /** @brief HAL Getter of signal SCU_SPID_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPID clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_spid_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_spid_clk_en;
  }

  /** @brief HAL Setter of signal SCU_SPID_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPID clock enable
  *
  * @param scu_spid_clk_en Value to be set
  */
  void inline scu_spid_clk_en_set(uint32_t scu_spid_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_spid_clk_en = scu_spid_clk_en;
  }

  /** @brief HAL Getter of signal SCU_FFT_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * FFT clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_fft_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_fft_clk_en;
  }

  /** @brief HAL Setter of signal SCU_FFT_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * FFT clock enable
  *
  * @param scu_fft_clk_en Value to be set
  */
  void inline scu_fft_clk_en_set(uint32_t scu_fft_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_fft_clk_en = scu_fft_clk_en;
  }

  /** @brief HAL Getter of signal SCU_DBGM_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * DBGM clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_dbgm_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_dbgm_clk_en;
  }

  /** @brief HAL Setter of signal SCU_DBGM_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * DBGM clock enable
  *
  * @param scu_dbgm_clk_en Value to be set
  */
  void inline scu_dbgm_clk_en_set(uint32_t scu_dbgm_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_dbgm_clk_en = scu_dbgm_clk_en;
  }

  /** @brief HAL Getter of signal SCU_DMA_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * DMA clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_dma_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_dma_clk_en;
  }

  /** @brief HAL Setter of signal SCU_DMA_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * DMA clock enable
  *
  * @param scu_dma_clk_en Value to be set
  */
  void inline scu_dma_clk_en_set(uint32_t scu_dma_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_dma_clk_en = scu_dma_clk_en;
  }

  /** @brief HAL Getter of signal SCU_CAMIF_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * CAMIF clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_camif_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_camif_clk_en;
  }

  /** @brief HAL Setter of signal SCU_CAMIF_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * CAMIF clock enable
  *
  * @param scu_camif_clk_en Value to be set
  */
  void inline scu_camif_clk_en_set(uint32_t scu_camif_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_camif_clk_en = scu_camif_clk_en;
  }

  /** @brief HAL Getter of signal SCU_PORT_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * PORT clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_port_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_port_clk_en;
  }

  /** @brief HAL Setter of signal SCU_PORT_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * PORT clock enable
  *
  * @param scu_port_clk_en Value to be set
  */
  void inline scu_port_clk_en_set(uint32_t scu_port_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_port_clk_en = scu_port_clk_en;
  }

  /** @brief HAL Getter of signal SCU_SPIS_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIS clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_spis_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_spis_clk_en;
  }

  /** @brief HAL Setter of signal SCU_SPIS_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIS clock enable
  *
  * @param scu_spis_clk_en Value to be set
  */
  void inline scu_spis_clk_en_set(uint32_t scu_spis_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_spis_clk_en = scu_spis_clk_en;
  }

  /** @brief HAL Getter of signal SCU_SPIMF_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIMF clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_spimf_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_spimf_clk_en;
  }

  /** @brief HAL Setter of signal SCU_SPIMF_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIMF clock enable
  *
  * @param scu_spimf_clk_en Value to be set
  */
  void inline scu_spimf_clk_en_set(uint32_t scu_spimf_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_spimf_clk_en = scu_spimf_clk_en;
  }

  /** @brief HAL Getter of signal SCU_SPIM0_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIM0 clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_spim0_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_spim0_clk_en;
  }

  /** @brief HAL Setter of signal SCU_SPIM0_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIM0 clock enable
  *
  * @param scu_spim0_clk_en Value to be set
  */
  void inline scu_spim0_clk_en_set(uint32_t scu_spim0_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_spim0_clk_en = scu_spim0_clk_en;
  }

  /** @brief HAL Getter of signal SCU_SPIM1_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIM1 clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_spim1_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_spim1_clk_en;
  }

  /** @brief HAL Setter of signal SCU_SPIM1_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * SPIM1 clock enable
  *
  * @param scu_spim1_clk_en Value to be set
  */
  void inline scu_spim1_clk_en_set(uint32_t scu_spim1_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_spim1_clk_en = scu_spim1_clk_en;
  }

  /** @brief HAL Getter of signal SCU_UART_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * UART clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_uart_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_uart_clk_en;
  }

  /** @brief HAL Setter of signal SCU_UART_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * UART clock enable
  *
  * @param scu_uart_clk_en Value to be set
  */
  void inline scu_uart_clk_en_set(uint32_t scu_uart_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_uart_clk_en = scu_uart_clk_en;
  }

  /** @brief HAL Getter of signal SCU_I2CM_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * I2CM clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_i2cm_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_i2cm_clk_en;
  }

  /** @brief HAL Setter of signal SCU_I2CM_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * I2CM clock enable
  *
  * @param scu_i2cm_clk_en Value to be set
  */
  void inline scu_i2cm_clk_en_set(uint32_t scu_i2cm_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_i2cm_clk_en = scu_i2cm_clk_en;
  }

  /** @brief HAL Getter of signal SCU_TIMER0_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * TIMER0 clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_timer0_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_timer0_clk_en;
  }

  /** @brief HAL Setter of signal SCU_TIMER0_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * TIMER0 clock enable
  *
  * @param scu_timer0_clk_en Value to be set
  */
  void inline scu_timer0_clk_en_set(uint32_t scu_timer0_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_timer0_clk_en = scu_timer0_clk_en;
  }

  /** @brief HAL Getter of signal SCU_TIMER1_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * TIMER1 clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_timer1_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_timer1_clk_en;
  }

  /** @brief HAL Setter of signal SCU_TIMER1_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * TIMER1 clock enable
  *
  * @param scu_timer1_clk_en Value to be set
  */
  void inline scu_timer1_clk_en_set(uint32_t scu_timer1_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_timer1_clk_en = scu_timer1_clk_en;
  }

  /** @brief HAL Getter of signal SCU_RTC_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * RTC clock enable
  *
  * @return Register value
  */
  uint32_t inline scu_rtc_clk_en_get() {
    return regmap->scu_periph_clk_en.fields.scu_rtc_clk_en;
  }

  /** @brief HAL Setter of signal SCU_RTC_CLK_EN from register SCU_PERIPH_CLK_EN
  *
  * RTC clock enable
  *
  * @param scu_rtc_clk_en Value to be set
  */
  void inline scu_rtc_clk_en_set(uint32_t scu_rtc_clk_en) {
    regmap->scu_periph_clk_en.fields.scu_rtc_clk_en = scu_rtc_clk_en;
  }

  /** @brief HAL Getter of signal SCU_APB_RESET from register SCU_PERIPH_RESET
  *
  * APB soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_apb_reset_get() {
    return regmap->scu_periph_reset.fields.scu_apb_reset;
  }

  /** @brief HAL Setter of signal SCU_APB_RESET from register SCU_PERIPH_RESET
  *
  * APB soft reset
  *
  * @param scu_apb_reset Value to be set
  */
  void inline scu_apb_reset_set(uint32_t scu_apb_reset) {
    regmap->scu_periph_reset.fields.scu_apb_reset = scu_apb_reset;
  }

  /** @brief HAL Getter of signal SCU_ISP_RESET from register SCU_PERIPH_RESET
  *
  * ISP soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_isp_reset_get() {
    return regmap->scu_periph_reset.fields.scu_isp_reset;
  }

  /** @brief HAL Setter of signal SCU_ISP_RESET from register SCU_PERIPH_RESET
  *
  * ISP soft reset
  *
  * @param scu_isp_reset Value to be set
  */
  void inline scu_isp_reset_set(uint32_t scu_isp_reset) {
    regmap->scu_periph_reset.fields.scu_isp_reset = scu_isp_reset;
  }

  /** @brief HAL Getter of signal SCU_NNPA_RESET from register SCU_PERIPH_RESET
  *
  * NNPA soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_nnpa_reset_get() {
    return regmap->scu_periph_reset.fields.scu_nnpa_reset;
  }

  /** @brief HAL Setter of signal SCU_NNPA_RESET from register SCU_PERIPH_RESET
  *
  * NNPA soft reset
  *
  * @param scu_nnpa_reset Value to be set
  */
  void inline scu_nnpa_reset_set(uint32_t scu_nnpa_reset) {
    regmap->scu_periph_reset.fields.scu_nnpa_reset = scu_nnpa_reset;
  }

  /** @brief HAL Getter of signal SCU_SPID_RESET from register SCU_PERIPH_RESET
  *
  * SPID soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_spid_reset_get() {
    return regmap->scu_periph_reset.fields.scu_spid_reset;
  }

  /** @brief HAL Setter of signal SCU_SPID_RESET from register SCU_PERIPH_RESET
  *
  * SPID soft reset
  *
  * @param scu_spid_reset Value to be set
  */
  void inline scu_spid_reset_set(uint32_t scu_spid_reset) {
    regmap->scu_periph_reset.fields.scu_spid_reset = scu_spid_reset;
  }

  /** @brief HAL Getter of signal SCU_FFT_RESET from register SCU_PERIPH_RESET
  *
  * FFT soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_fft_reset_get() {
    return regmap->scu_periph_reset.fields.scu_fft_reset;
  }

  /** @brief HAL Setter of signal SCU_FFT_RESET from register SCU_PERIPH_RESET
  *
  * FFT soft reset
  *
  * @param scu_fft_reset Value to be set
  */
  void inline scu_fft_reset_set(uint32_t scu_fft_reset) {
    regmap->scu_periph_reset.fields.scu_fft_reset = scu_fft_reset;
  }

  /** @brief HAL Getter of signal SCU_DBGM_RESET from register SCU_PERIPH_RESET
  *
  * DBGM soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_dbgm_reset_get() {
    return regmap->scu_periph_reset.fields.scu_dbgm_reset;
  }

  /** @brief HAL Setter of signal SCU_DBGM_RESET from register SCU_PERIPH_RESET
  *
  * DBGM soft reset
  *
  * @param scu_dbgm_reset Value to be set
  */
  void inline scu_dbgm_reset_set(uint32_t scu_dbgm_reset) {
    regmap->scu_periph_reset.fields.scu_dbgm_reset = scu_dbgm_reset;
  }

  /** @brief HAL Getter of signal SCU_DMA_RESET from register SCU_PERIPH_RESET
  *
  * DMA soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_dma_reset_get() {
    return regmap->scu_periph_reset.fields.scu_dma_reset;
  }

  /** @brief HAL Setter of signal SCU_DMA_RESET from register SCU_PERIPH_RESET
  *
  * DMA soft reset
  *
  * @param scu_dma_reset Value to be set
  */
  void inline scu_dma_reset_set(uint32_t scu_dma_reset) {
    regmap->scu_periph_reset.fields.scu_dma_reset = scu_dma_reset;
  }

  /** @brief HAL Getter of signal SCU_CAMIF_RESET from register SCU_PERIPH_RESET
  *
  * CAMIF soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_camif_reset_get() {
    return regmap->scu_periph_reset.fields.scu_camif_reset;
  }

  /** @brief HAL Setter of signal SCU_CAMIF_RESET from register SCU_PERIPH_RESET
  *
  * CAMIF soft reset
  *
  * @param scu_camif_reset Value to be set
  */
  void inline scu_camif_reset_set(uint32_t scu_camif_reset) {
    regmap->scu_periph_reset.fields.scu_camif_reset = scu_camif_reset;
  }

  /** @brief HAL Getter of signal SCU_PORT_RESET from register SCU_PERIPH_RESET
  *
  * PORT soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_port_reset_get() {
    return regmap->scu_periph_reset.fields.scu_port_reset;
  }

  /** @brief HAL Setter of signal SCU_PORT_RESET from register SCU_PERIPH_RESET
  *
  * PORT soft reset
  *
  * @param scu_port_reset Value to be set
  */
  void inline scu_port_reset_set(uint32_t scu_port_reset) {
    regmap->scu_periph_reset.fields.scu_port_reset = scu_port_reset;
  }

  /** @brief HAL Getter of signal SCU_SPIS_RESET from register SCU_PERIPH_RESET
  *
  * SPIS soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_spis_reset_get() {
    return regmap->scu_periph_reset.fields.scu_spis_reset;
  }

  /** @brief HAL Setter of signal SCU_SPIS_RESET from register SCU_PERIPH_RESET
  *
  * SPIS soft reset
  *
  * @param scu_spis_reset Value to be set
  */
  void inline scu_spis_reset_set(uint32_t scu_spis_reset) {
    regmap->scu_periph_reset.fields.scu_spis_reset = scu_spis_reset;
  }

  /** @brief HAL Getter of signal SCU_SPIMF_RESET from register SCU_PERIPH_RESET
  *
  * SPIMF soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_spimf_reset_get() {
    return regmap->scu_periph_reset.fields.scu_spimf_reset;
  }

  /** @brief HAL Setter of signal SCU_SPIMF_RESET from register SCU_PERIPH_RESET
  *
  * SPIMF soft reset
  *
  * @param scu_spimf_reset Value to be set
  */
  void inline scu_spimf_reset_set(uint32_t scu_spimf_reset) {
    regmap->scu_periph_reset.fields.scu_spimf_reset = scu_spimf_reset;
  }

  /** @brief HAL Getter of signal SCU_SPIM0_RESET from register SCU_PERIPH_RESET
  *
  * SPIM0 soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_spim0_reset_get() {
    return regmap->scu_periph_reset.fields.scu_spim0_reset;
  }

  /** @brief HAL Setter of signal SCU_SPIM0_RESET from register SCU_PERIPH_RESET
  *
  * SPIM0 soft reset
  *
  * @param scu_spim0_reset Value to be set
  */
  void inline scu_spim0_reset_set(uint32_t scu_spim0_reset) {
    regmap->scu_periph_reset.fields.scu_spim0_reset = scu_spim0_reset;
  }

  /** @brief HAL Getter of signal SCU_SPIM1_RESET from register SCU_PERIPH_RESET
  *
  * SPIM1 soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_spim1_reset_get() {
    return regmap->scu_periph_reset.fields.scu_spim1_reset;
  }

  /** @brief HAL Setter of signal SCU_SPIM1_RESET from register SCU_PERIPH_RESET
  *
  * SPIM1 soft reset
  *
  * @param scu_spim1_reset Value to be set
  */
  void inline scu_spim1_reset_set(uint32_t scu_spim1_reset) {
    regmap->scu_periph_reset.fields.scu_spim1_reset = scu_spim1_reset;
  }

  /** @brief HAL Getter of signal SCU_UART_RESET from register SCU_PERIPH_RESET
  *
  * UART soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_uart_reset_get() {
    return regmap->scu_periph_reset.fields.scu_uart_reset;
  }

  /** @brief HAL Setter of signal SCU_UART_RESET from register SCU_PERIPH_RESET
  *
  * UART soft reset
  *
  * @param scu_uart_reset Value to be set
  */
  void inline scu_uart_reset_set(uint32_t scu_uart_reset) {
    regmap->scu_periph_reset.fields.scu_uart_reset = scu_uart_reset;
  }

  /** @brief HAL Getter of signal SCU_I2CM_RESET from register SCU_PERIPH_RESET
  *
  * I2CM soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_i2cm_reset_get() {
    return regmap->scu_periph_reset.fields.scu_i2cm_reset;
  }

  /** @brief HAL Setter of signal SCU_I2CM_RESET from register SCU_PERIPH_RESET
  *
  * I2CM soft reset
  *
  * @param scu_i2cm_reset Value to be set
  */
  void inline scu_i2cm_reset_set(uint32_t scu_i2cm_reset) {
    regmap->scu_periph_reset.fields.scu_i2cm_reset = scu_i2cm_reset;
  }

  /** @brief HAL Getter of signal SCU_TIMER0_RESET from register SCU_PERIPH_RESET
  *
  * TIMER0 soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_timer0_reset_get() {
    return regmap->scu_periph_reset.fields.scu_timer0_reset;
  }

  /** @brief HAL Setter of signal SCU_TIMER0_RESET from register SCU_PERIPH_RESET
  *
  * TIMER0 soft reset
  *
  * @param scu_timer0_reset Value to be set
  */
  void inline scu_timer0_reset_set(uint32_t scu_timer0_reset) {
    regmap->scu_periph_reset.fields.scu_timer0_reset = scu_timer0_reset;
  }

  /** @brief HAL Getter of signal SCU_TIMER1_RESET from register SCU_PERIPH_RESET
  *
  * TIMER1 soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_timer1_reset_get() {
    return regmap->scu_periph_reset.fields.scu_timer1_reset;
  }

  /** @brief HAL Setter of signal SCU_TIMER1_RESET from register SCU_PERIPH_RESET
  *
  * TIMER1 soft reset
  *
  * @param scu_timer1_reset Value to be set
  */
  void inline scu_timer1_reset_set(uint32_t scu_timer1_reset) {
    regmap->scu_periph_reset.fields.scu_timer1_reset = scu_timer1_reset;
  }

  /** @brief HAL Getter of signal SCU_RTC_RESET from register SCU_PERIPH_RESET
  *
  * RTC soft reset
  *
  * @return Register value
  */
  uint32_t inline scu_rtc_reset_get() {
    return regmap->scu_periph_reset.fields.scu_rtc_reset;
  }

  /** @brief HAL Setter of signal SCU_RTC_RESET from register SCU_PERIPH_RESET
  *
  * RTC soft reset
  *
  * @param scu_rtc_reset Value to be set
  */
  void inline scu_rtc_reset_set(uint32_t scu_rtc_reset) {
    regmap->scu_periph_reset.fields.scu_rtc_reset = scu_rtc_reset;
  }

  /** @brief HAL Getter of signal SCU_CLK_OUT0_SRC from register SCU_CLK_OUT0_CFG
  *
  * CLK_OUT_0  clock source selection
  *
  * @return Register value
  */
  uint32_t inline scu_clk_out0_src_get() {
    return regmap->scu_clk_out0_cfg.fields.scu_clk_out0_src;
  }

  /** @brief HAL Setter of signal SCU_CLK_OUT0_SRC from register SCU_CLK_OUT0_CFG
  *
  * CLK_OUT_0  clock source selection
  *
  * @param scu_clk_out0_src Value to be set
  */
  void inline scu_clk_out0_src_set(uint32_t scu_clk_out0_src) {
    regmap->scu_clk_out0_cfg.fields.scu_clk_out0_src = scu_clk_out0_src;
  }

  /** @brief HAL Getter of signal SCU_CLK_OUT0_DIV from register SCU_CLK_OUT0_CFG
  *
  * CLK_OUT_0  clock division ratio (Freq / (1 + DIV))
  *
  * @return Register value
  */
  uint32_t inline scu_clk_out0_div_get() {
    return regmap->scu_clk_out0_cfg.fields.scu_clk_out0_div;
  }

  /** @brief HAL Setter of signal SCU_CLK_OUT0_DIV from register SCU_CLK_OUT0_CFG
  *
  * CLK_OUT_0  clock division ratio (Freq / (1 + DIV))
  *
  * @param scu_clk_out0_div Value to be set
  */
  void inline scu_clk_out0_div_set(uint32_t scu_clk_out0_div) {
    regmap->scu_clk_out0_cfg.fields.scu_clk_out0_div = scu_clk_out0_div;
  }

  /** @brief HAL Getter of signal SCU_CLK_OUT1_SRC from register SCU_CLK_OUT1_CFG
  *
  * CLK_OUT_1  clock source selection
  *
  * @return Register value
  */
  uint32_t inline scu_clk_out1_src_get() {
    return regmap->scu_clk_out1_cfg.fields.scu_clk_out1_src;
  }

  /** @brief HAL Setter of signal SCU_CLK_OUT1_SRC from register SCU_CLK_OUT1_CFG
  *
  * CLK_OUT_1  clock source selection
  *
  * @param scu_clk_out1_src Value to be set
  */
  void inline scu_clk_out1_src_set(uint32_t scu_clk_out1_src) {
    regmap->scu_clk_out1_cfg.fields.scu_clk_out1_src = scu_clk_out1_src;
  }

  /** @brief HAL Getter of signal SCU_CLK_OUT1_DIV from register SCU_CLK_OUT1_CFG
  *
  * CLK_OUT_1  clock division ratio (Freq / (1 + DIV))
  *
  * @return Register value
  */
  uint32_t inline scu_clk_out1_div_get() {
    return regmap->scu_clk_out1_cfg.fields.scu_clk_out1_div;
  }

  /** @brief HAL Setter of signal SCU_CLK_OUT1_DIV from register SCU_CLK_OUT1_CFG
  *
  * CLK_OUT_1  clock division ratio (Freq / (1 + DIV))
  *
  * @param scu_clk_out1_div Value to be set
  */
  void inline scu_clk_out1_div_set(uint32_t scu_clk_out1_div) {
    regmap->scu_clk_out1_cfg.fields.scu_clk_out1_div = scu_clk_out1_div;
  }

  /** @brief HAL Trigger 'write pulse' of signal SCU_GLOBAL_RESET from register SCU_SYSTEM_CTRL
  *
  * When written 1, a global reset is issued
  *
  */
  void inline scu_global_reset() {
    regmap->scu_system_ctrl.word = SCU_GLOBAL_RESET_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal SCU_SLEEP_REQ from register SCU_SYSTEM_CTRL
  *
  * When written 1, a sleep request is issue, as for a WFI instruction
  *
  */
  void inline scu_sleep_req() {
    regmap->scu_system_ctrl.word = SCU_SLEEP_REQ_MASK;
  }

  /** @brief HAL Getter of signal SCU_SLEEP_ENABLE from register SCU_SYSTEM_CONFIG
  *
  * When ‘1’, sleep requests or WFI will put the chip in sleep mode
  *
  * @return Register value
  */
  uint32_t inline scu_sleep_enable_get() {
    return regmap->scu_system_config.fields.scu_sleep_enable;
  }

  /** @brief HAL Setter of signal SCU_SLEEP_ENABLE from register SCU_SYSTEM_CONFIG
  *
  * When ‘1’, sleep requests or WFI will put the chip in sleep mode
  *
  * @param scu_sleep_enable Value to be set
  */
  void inline scu_sleep_enable_set(uint32_t scu_sleep_enable) {
    regmap->scu_system_config.fields.scu_sleep_enable = scu_sleep_enable;
  }

  /** @brief HAL Getter of signal SCU_PLL_AUTO_SLEEP from register SCU_SYSTEM_CONFIG
  *
  * When ‘1’, the PLL is automatically disabled/re-enabled with sleep requests
  *
  * @return Register value
  */
  uint32_t inline scu_pll_auto_sleep_get() {
    return regmap->scu_system_config.fields.scu_pll_auto_sleep;
  }

  /** @brief HAL Setter of signal SCU_PLL_AUTO_SLEEP from register SCU_SYSTEM_CONFIG
  *
  * When ‘1’, the PLL is automatically disabled/re-enabled with sleep requests
  *
  * @param scu_pll_auto_sleep Value to be set
  */
  void inline scu_pll_auto_sleep_set(uint32_t scu_pll_auto_sleep) {
    regmap->scu_system_config.fields.scu_pll_auto_sleep = scu_pll_auto_sleep;
  }

  /** @brief HAL Getter of signal SCU_WDT_RESET_MODE from register SCU_SYSTEM_CONFIG
  *
  * Reset mode applied on a watchdog event:
  * 0x0 or 0x2: Disabled (no reset)
  * 0x1: MCU core only
  * 0x3: Full chip
  *
  * @return Register value
  */
  uint32_t inline scu_wdt_reset_mode_get() {
    return regmap->scu_system_config.fields.scu_wdt_reset_mode;
  }

  /** @brief HAL Setter of signal SCU_WDT_RESET_MODE from register SCU_SYSTEM_CONFIG
  *
  * Reset mode applied on a watchdog event:
  * 0x0 or 0x2: Disabled (no reset)
  * 0x1: MCU core only
  * 0x3: Full chip
  *
  * @param scu_wdt_reset_mode Value to be set
  */
  void inline scu_wdt_reset_mode_set(uint32_t scu_wdt_reset_mode) {
    regmap->scu_system_config.fields.scu_wdt_reset_mode = scu_wdt_reset_mode;
  }

  /** @brief HAL Getter of signal SCU_CORE_RUN_EN from register SCU_CORE_CTRL
  *
  * Enable instruction fetching from CPU core
  *
  * @return Register value
  */
  uint32_t inline scu_core_run_en_get() {
    return regmap->scu_core_ctrl.fields.scu_core_run_en;
  }

  /** @brief HAL Setter of signal SCU_CORE_RUN_EN from register SCU_CORE_CTRL
  *
  * Enable instruction fetching from CPU core
  *
  * @param scu_core_run_en Value to be set
  */
  void inline scu_core_run_en_set(uint32_t scu_core_run_en) {
    regmap->scu_core_ctrl.fields.scu_core_run_en = scu_core_run_en;
  }

  /** @brief HAL Trigger 'write pulse' of signal SCU_CORE_RESET from register SCU_CORE_CTRL
  *
  * When written ‘1’, a CPU core reset is issued
  *
  */
  void inline scu_core_reset() {
    regmap->scu_core_ctrl.word |= SCU_CORE_RESET_MASK;
  }

  /** @brief HAL Getter of signal SCU_BOOT_ADDR from register SCU_BOOT_ADDR
  *
  * Current code boot address 
  *
  * @return Register value
  */
  uint32_t inline scu_boot_addr_get() {
    return regmap->scu_boot_addr.fields.scu_boot_addr;
  }

  /** @brief HAL Getter of signal SCU_BOOT_ADDR_OVW from register SCU_BOOT_ADDR_OVW
  *
  * Overwrite code boot address 
  *
  * @return Register value
  */
  uint32_t inline scu_boot_addr_ovw_get() {
    return regmap->scu_boot_addr_ovw.fields.scu_boot_addr_ovw;
  }

  /** @brief HAL Setter of signal SCU_BOOT_ADDR_OVW from register SCU_BOOT_ADDR_OVW
  *
  * Overwrite code boot address 
  *
  * @param scu_boot_addr_ovw Value to be set
  */
  void inline scu_boot_addr_ovw_set(uint32_t scu_boot_addr_ovw) {
    regmap->scu_boot_addr_ovw.fields.scu_boot_addr_ovw = scu_boot_addr_ovw;
  }

  /** @brief HAL Trigger 'write pulse' of signal SCU_BOOT_ADDR_SET from register SCU_BOOT_ADDR_SET
  *
  * When written 1, SCU_BOOT_ADDR is set to SCU_BOOT_ADDR_OVW 
  *
  */
  void inline scu_boot_addr_set() {
    regmap->scu_boot_addr_set.word = SCU_BOOT_ADDR_SET_MASK;
  }

  /** @brief HAL Getter of signal SCU_CORE_STATUS from register SCU_CORE_STATUS
  *
  * CPU core user status (shall be written by CPU only) 
  *
  * @return Register value
  */
  uint32_t inline scu_core_status_get() {
    return regmap->scu_core_status.fields.scu_core_status;
  }

  /** @brief HAL Setter of signal SCU_CORE_STATUS from register SCU_CORE_STATUS
  *
  * CPU core user status (shall be written by CPU only) 
  *
  * @param scu_core_status Value to be set
  */
  void inline scu_core_status_set(uint32_t scu_core_status) {
    regmap->scu_core_status.fields.scu_core_status = scu_core_status;
  }

  /** @brief HAL Getter of signal SCU_CORE_MSG from register SCU_CORE_MSG
  *
  * CPU core message (shall be written by CPU only) 
  *
  * @param index Index id (from 0 to 3)
  * @retval 0 if index not found
  * @return Register value
  */
  uint32_t inline scu_core_msg_get(uint32_t index) {
    if (index < 4) return regmap->scu_core_msg[index].fields.scu_core_msg;
    return 0;
  }

  /** @brief HAL Setter of signal SCU_CORE_MSG from register SCU_CORE_MSG
  *
  * CPU core message (shall be written by CPU only) 
  *
  * @param index Index id (from 0 to 3)
  * @param scu_core_msg Value to be set
  */
  void inline scu_core_msg_set(uint32_t index, uint32_t scu_core_msg) {
    if (index < 4) regmap->scu_core_msg[index].fields.scu_core_msg = scu_core_msg;
  }

  /** @brief HAL Getter of signal SCU_EXT_MSG from register SCU_EXT_MSG
  *
  * External message to CPU (shall be written by external device only) 
  *
  * @param index Index id (from 0 to 3)
  * @retval 0 if index not found
  * @return Register value
  */
  uint32_t inline scu_ext_msg_get(uint32_t index) {
    if (index < 4) return regmap->scu_ext_msg[index].fields.scu_ext_msg;
    return 0;
  }

  /** @brief HAL Setter of signal SCU_EXT_MSG from register SCU_EXT_MSG
  *
  * External message to CPU (shall be written by external device only) 
  *
  * @param index Index id (from 0 to 3)
  * @param scu_ext_msg Value to be set
  */
  void inline scu_ext_msg_set(uint32_t index, uint32_t scu_ext_msg) {
    if (index < 4) regmap->scu_ext_msg[index].fields.scu_ext_msg = scu_ext_msg;
  }

  /** @brief HAL Trigger 'write pulse' of signal SCU_GENERATE_IRQ from register SCU_GENERATE_IRQ
  *
  * Generate an SCU interrupt when written ‘1’
  *
  */
  void inline scu_generate_irq() {
    regmap->scu_generate_irq.word = SCU_GENERATE_IRQ_MASK;
  }

  /** @brief HAL Getter of signal SCU_WR_PROT_START from register SCU_WR_PROT_START
  *
  * Write protection start address (included)
  *
  * @return Register value
  */
  uint32_t inline scu_wr_prot_start_get() {
    return regmap->scu_wr_prot_start.fields.scu_wr_prot_start;
  }

  /** @brief HAL Setter of signal SCU_WR_PROT_START from register SCU_WR_PROT_START
  *
  * Write protection start address (included)
  *
  * @param scu_wr_prot_start Value to be set
  */
  void inline scu_wr_prot_start_set(uint32_t scu_wr_prot_start) {
    regmap->scu_wr_prot_start.fields.scu_wr_prot_start = scu_wr_prot_start;
  }

  /** @brief HAL Getter of signal SCU_WR_PROT_END from register SCU_WR_PROT_END
  *
  * Write protection end address (included)
  *
  * @return Register value
  */
  uint32_t inline scu_wr_prot_end_get() {
    return regmap->scu_wr_prot_end.fields.scu_wr_prot_end;
  }

  /** @brief HAL Setter of signal SCU_WR_PROT_END from register SCU_WR_PROT_END
  *
  * Write protection end address (included)
  *
  * @param scu_wr_prot_end Value to be set
  */
  void inline scu_wr_prot_end_set(uint32_t scu_wr_prot_end) {
    regmap->scu_wr_prot_end.fields.scu_wr_prot_end = scu_wr_prot_end;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_TEST1 from register SRAM_CUTS_CTRL
  *
  * Test mode to bypass self-timed circuit.The external clock controls the read and write control signals
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_test1_get() {
    return regmap->sram_cuts_ctrl.fields.sram_cuts_test1;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_TEST1 from register SRAM_CUTS_CTRL
  *
  * Test mode to bypass self-timed circuit.The external clock controls the read and write control signals
  *
  * @param sram_cuts_test1 Value to be set
  */
  void inline sram_cuts_test1_set(uint32_t sram_cuts_test1) {
    regmap->sram_cuts_ctrl.fields.sram_cuts_test1 = sram_cuts_test1;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_RME from register SRAM_CUTS_CTRL
  *
  * Read-Write margin enable
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_rme_get() {
    return regmap->sram_cuts_ctrl.fields.sram_cuts_rme;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_RME from register SRAM_CUTS_CTRL
  *
  * Read-Write margin enable
  *
  * @param sram_cuts_rme Value to be set
  */
  void inline sram_cuts_rme_set(uint32_t sram_cuts_rme) {
    regmap->sram_cuts_ctrl.fields.sram_cuts_rme = sram_cuts_rme;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_RM from register SRAM_CUTS_CTRL
  *
  * Read-Write margin
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_rm_get() {
    return regmap->sram_cuts_ctrl.fields.sram_cuts_rm;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_RM from register SRAM_CUTS_CTRL
  *
  * Read-Write margin
  *
  * @param sram_cuts_rm Value to be set
  */
  void inline sram_cuts_rm_set(uint32_t sram_cuts_rm) {
    regmap->sram_cuts_ctrl.fields.sram_cuts_rm = sram_cuts_rm;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_RA from register SRAM_CUTS_CTRL
  *
  * Read Assist setting
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_ra_get() {
    return regmap->sram_cuts_ctrl.fields.sram_cuts_ra;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_RA from register SRAM_CUTS_CTRL
  *
  * Read Assist setting
  *
  * @param sram_cuts_ra Value to be set
  */
  void inline sram_cuts_ra_set(uint32_t sram_cuts_ra) {
    regmap->sram_cuts_ctrl.fields.sram_cuts_ra = sram_cuts_ra;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_WA from register SRAM_CUTS_CTRL
  *
  * Write Assist setting
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_wa_get() {
    return regmap->sram_cuts_ctrl.fields.sram_cuts_wa;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_WA from register SRAM_CUTS_CTRL
  *
  * Write Assist setting
  *
  * @param sram_cuts_wa Value to be set
  */
  void inline sram_cuts_wa_set(uint32_t sram_cuts_wa) {
    regmap->sram_cuts_ctrl.fields.sram_cuts_wa = sram_cuts_wa;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_WPULSE from register SRAM_CUTS_CTRL
  *
  * Write Assist pulse width setting
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_wpulse_get() {
    return regmap->sram_cuts_ctrl.fields.sram_cuts_wpulse;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_WPULSE from register SRAM_CUTS_CTRL
  *
  * Write Assist pulse width setting
  *
  * @param sram_cuts_wpulse Value to be set
  */
  void inline sram_cuts_wpulse_set(uint32_t sram_cuts_wpulse) {
    regmap->sram_cuts_ctrl.fields.sram_cuts_wpulse = sram_cuts_wpulse;
  }

  /** @brief HAL Getter of signal SRAM_CUTS_PD_CTRL from register SRAM_CUTS_PD_CTRL
  *
  * Each bit controls the power-down mode of SRAM memory cuts
  *
  * @return Register value
  */
  uint32_t inline sram_cuts_pd_ctrl_get() {
    return regmap->sram_cuts_pd_ctrl.fields.sram_cuts_pd_ctrl;
  }

  /** @brief HAL Setter of signal SRAM_CUTS_PD_CTRL from register SRAM_CUTS_PD_CTRL
  *
  * Each bit controls the power-down mode of SRAM memory cuts
  *
  * @param sram_cuts_pd_ctrl Value to be set
  */
  void inline sram_cuts_pd_ctrl_set(uint32_t sram_cuts_pd_ctrl) {
    regmap->sram_cuts_pd_ctrl.fields.sram_cuts_pd_ctrl = sram_cuts_pd_ctrl;
  }

  /** @brief HAL Getter of signal SRAM_FORCE_CLK_EN from register SRAM_FORCE_CLK_EN
  *
  * When ‘1’, all SRAM cuts receive a clock, otherwise only when needed
  *
  * @return Register value
  */
  uint32_t inline sram_force_clk_en_get() {
    return regmap->sram_force_clk_en.fields.sram_force_clk_en;
  }

  /** @brief HAL Setter of signal SRAM_FORCE_CLK_EN from register SRAM_FORCE_CLK_EN
  *
  * When ‘1’, all SRAM cuts receive a clock, otherwise only when needed
  *
  * @param sram_force_clk_en Value to be set
  */
  void inline sram_force_clk_en_set(uint32_t sram_force_clk_en) {
    regmap->sram_force_clk_en.fields.sram_force_clk_en = sram_force_clk_en;
  }

  /** @brief HAL Trigger 'write set' of signal SRAM_MBIST_RUN from register SRAM_MBIST_CTRL
  *
  * Launch memory BIST test
  *
  * @param value Value to apply to signal (4 bits large)
  */
  void inline sram_mbist_run(uint32_t value) {
    regmap->sram_mbist_ctrl.word = SRAM_MBIST_RUN_MASK & (value << SRAM_MBIST_RUN_OFFSET);
  }

  /** @brief HAL Getter of signal SRAM_MBIST_RUN from register SRAM_MBIST_CTRL
  *
  * Launch memory BIST test
  *
  * @return Register value
  */
  uint32_t inline sram_mbist_run_get() {
    return regmap->sram_mbist_ctrl.fields.sram_mbist_run;
  }

  /** @brief HAL Getter of signal SRAM_MBIST_DONE from register SRAM_MBIST_STATUS
  *
  * Set by HW on memory BIST completion. Cleared by HW when BIST start
  *
  * @return Register value
  */
  uint32_t inline sram_mbist_done_get() {
    return regmap->sram_mbist_status.fields.sram_mbist_done;
  }

  /** @brief HAL Getter of signal SRAM_MBIST_ANY_FAIL from register SRAM_MBIST_STATUS
  *
  * Set by HW on any memory BIST failure. Cleared by HW when BIST start
  *
  * @return Register value
  */
  uint32_t inline sram_mbist_any_fail_get() {
    return regmap->sram_mbist_status.fields.sram_mbist_any_fail;
  }

  /** @brief HAL Getter of signal SRAM_MBIST_FAIL from register SRAM_MBIST_FAIL
  *
  * BIST status of SRAM cuts 0 to 31
  *
  * @return Register value
  */
  uint32_t inline sram_mbist_fail_get() {
    return regmap->sram_mbist_fail.fields.sram_mbist_fail;
  }

  /** @brief HAL Getter of signal PLL_EN_MODE from register PLL_CONTROL
  *
  * PLL enable mode:
  * 00: Disabled
  * 01: Open-loop, clock accuracy within +/- 1% range
  * 10: Enabled, PLL with frequency locking
  * 11: Reserved
  *
  * @return Register value
  */
  uint32_t inline pll_en_mode_get() {
    return regmap->pll_control.fields.pll_en_mode;
  }

  /** @brief HAL Setter of signal PLL_EN_MODE from register PLL_CONTROL
  *
  * PLL enable mode:
  * 00: Disabled
  * 01: Open-loop, clock accuracy within +/- 1% range
  * 10: Enabled, PLL with frequency locking
  * 11: Reserved
  *
  * @param pll_en_mode Value to be set
  */
  void inline pll_en_mode_set(uint32_t pll_en_mode) {
    regmap->pll_control.fields.pll_en_mode = pll_en_mode;
  }

  /** @brief HAL Getter of signal PLL_AUTO_CAL from register PLL_CONTROL
  *
  * When ‘1’, recalibration is automatically triggered if PLL is abut
  *
  * @return Register value
  */
  uint32_t inline pll_auto_cal_get() {
    return regmap->pll_control.fields.pll_auto_cal;
  }

  /** @brief HAL Setter of signal PLL_AUTO_CAL from register PLL_CONTROL
  *
  * When ‘1’, recalibration is automatically triggered if PLL is abut
  *
  * @param pll_auto_cal Value to be set
  */
  void inline pll_auto_cal_set(uint32_t pll_auto_cal) {
    regmap->pll_control.fields.pll_auto_cal = pll_auto_cal;
  }

  /** @brief HAL Getter of signal PLL_FREQ_MULT from register PLL_CONTROL
  *
  * PLL frequency multiplier. PLL clock frequency PLL_CLK = 4 MHz * (this + 1). Valid range [14:127]
  *
  * @return Register value
  */
  uint32_t inline pll_freq_mult_get() {
    return regmap->pll_control.fields.pll_freq_mult;
  }

  /** @brief HAL Setter of signal PLL_FREQ_MULT from register PLL_CONTROL
  *
  * PLL frequency multiplier. PLL clock frequency PLL_CLK = 4 MHz * (this + 1). Valid range [14:127]
  *
  * @param pll_freq_mult Value to be set
  */
  void inline pll_freq_mult_set(uint32_t pll_freq_mult) {
    regmap->pll_control.fields.pll_freq_mult = pll_freq_mult;
  }

  /** @brief HAL Getter of signal PLL_POST_DIV from register PLL_CONTROL
  *
  * PLL output post divider. Clock frequencies: PLL_DIV_CLK = PLL_CLK / (this + 1)
  *
  * @return Register value
  */
  uint32_t inline pll_post_div_get() {
    return regmap->pll_control.fields.pll_post_div;
  }

  /** @brief HAL Setter of signal PLL_POST_DIV from register PLL_CONTROL
  *
  * PLL output post divider. Clock frequencies: PLL_DIV_CLK = PLL_CLK / (this + 1)
  *
  * @param pll_post_div Value to be set
  */
  void inline pll_post_div_set(uint32_t pll_post_div) {
    regmap->pll_control.fields.pll_post_div = pll_post_div;
  }

  /** @brief HAL Trigger 'write pulse' of signal PLL_RESTART from register PLL_CONTROL
  *
  * When written 1, the PLL is recalibrated and relocked
  *
  */
  void inline pll_restart() {
    regmap->pll_control.word |= PLL_RESTART_MASK;
  }

  /** @brief HAL Getter of signal PLL_CAL_PERIODS from register PLL_CAL_CONFIG
  *
  * Number of periods of calibration clock when measuring the PLL frequency
  *
  * @return Register value
  */
  uint32_t inline pll_cal_periods_get() {
    return regmap->pll_cal_config.fields.pll_cal_periods;
  }

  /** @brief HAL Setter of signal PLL_CAL_PERIODS from register PLL_CAL_CONFIG
  *
  * Number of periods of calibration clock when measuring the PLL frequency
  *
  * @param pll_cal_periods Value to be set
  */
  void inline pll_cal_periods_set(uint32_t pll_cal_periods) {
    regmap->pll_cal_config.fields.pll_cal_periods = pll_cal_periods;
  }

  /** @brief HAL Getter of signal PLL_CAL_TARGET from register PLL_CAL_CONFIG
  *
  * Calibration target value
  *
  * @return Register value
  */
  uint32_t inline pll_cal_target_get() {
    return regmap->pll_cal_config.fields.pll_cal_target;
  }

  /** @brief HAL Setter of signal PLL_CAL_TARGET from register PLL_CAL_CONFIG
  *
  * Calibration target value
  *
  * @param pll_cal_target Value to be set
  */
  void inline pll_cal_target_set(uint32_t pll_cal_target) {
    regmap->pll_cal_config.fields.pll_cal_target = pll_cal_target;
  }

  /** @brief HAL Getter of signal PLL_CAL_START_DLY from register PLL_CAL_CONFIG
  *
  * Number of periods of calibration clock before measurement
  *
  * @return Register value
  */
  uint32_t inline pll_cal_start_dly_get() {
    return regmap->pll_cal_config.fields.pll_cal_start_dly;
  }

  /** @brief HAL Setter of signal PLL_CAL_START_DLY from register PLL_CAL_CONFIG
  *
  * Number of periods of calibration clock before measurement
  *
  * @param pll_cal_start_dly Value to be set
  */
  void inline pll_cal_start_dly_set(uint32_t pll_cal_start_dly) {
    regmap->pll_cal_config.fields.pll_cal_start_dly = pll_cal_start_dly;
  }

  /** @brief HAL Getter of signal PLL_CAL_BYPASS_EN from register PLL_CAL_BYPASS
  *
  * When ‘1’, the PLL calibration inputs are taken from PLL_CAL_BYPASS_VAL and PLL_GAIN_BYPASS_VAL
  *
  * @return Register value
  */
  uint32_t inline pll_cal_bypass_en_get() {
    return regmap->pll_cal_bypass.fields.pll_cal_bypass_en;
  }

  /** @brief HAL Setter of signal PLL_CAL_BYPASS_EN from register PLL_CAL_BYPASS
  *
  * When ‘1’, the PLL calibration inputs are taken from PLL_CAL_BYPASS_VAL and PLL_GAIN_BYPASS_VAL
  *
  * @param pll_cal_bypass_en Value to be set
  */
  void inline pll_cal_bypass_en_set(uint32_t pll_cal_bypass_en) {
    regmap->pll_cal_bypass.fields.pll_cal_bypass_en = pll_cal_bypass_en;
  }

  /** @brief HAL Getter of signal PLL_CAL_BYPASS_VAL from register PLL_CAL_BYPASS
  *
  * VCO tune value applied when PLL_CAL_BYPASS_EN=1
  *
  * @return Register value
  */
  uint32_t inline pll_cal_bypass_val_get() {
    return regmap->pll_cal_bypass.fields.pll_cal_bypass_val;
  }

  /** @brief HAL Setter of signal PLL_CAL_BYPASS_VAL from register PLL_CAL_BYPASS
  *
  * VCO tune value applied when PLL_CAL_BYPASS_EN=1
  *
  * @param pll_cal_bypass_val Value to be set
  */
  void inline pll_cal_bypass_val_set(uint32_t pll_cal_bypass_val) {
    regmap->pll_cal_bypass.fields.pll_cal_bypass_val = pll_cal_bypass_val;
  }

  /** @brief HAL Getter of signal PLL_GAIN_BYPASS_VAL from register PLL_CAL_BYPASS
  *
  * VCO gain value applied when PLL_CAL_BYPASS_EN=1
  *
  * @return Register value
  */
  uint32_t inline pll_gain_bypass_val_get() {
    return regmap->pll_cal_bypass.fields.pll_gain_bypass_val;
  }

  /** @brief HAL Setter of signal PLL_GAIN_BYPASS_VAL from register PLL_CAL_BYPASS
  *
  * VCO gain value applied when PLL_CAL_BYPASS_EN=1
  *
  * @param pll_gain_bypass_val Value to be set
  */
  void inline pll_gain_bypass_val_set(uint32_t pll_gain_bypass_val) {
    regmap->pll_cal_bypass.fields.pll_gain_bypass_val = pll_gain_bypass_val;
  }

  /** @brief HAL Getter of signal PLL_ANA_CONFIG from register PLL_ANA_CONTROL
  *
  * Reserved
  *
  * @return Register value
  */
  uint32_t inline pll_ana_config_get() {
    return regmap->pll_ana_control.fields.pll_ana_config;
  }

  /** @brief HAL Setter of signal PLL_ANA_CONFIG from register PLL_ANA_CONTROL
  *
  * Reserved
  *
  * @param pll_ana_config Value to be set
  */
  void inline pll_ana_config_set(uint32_t pll_ana_config) {
    regmap->pll_ana_control.fields.pll_ana_config = pll_ana_config;
  }

  /** @brief HAL Getter of signal PLL_CTRL_IN_EN from register PLL_ANA_CONTROL
  *
  * When ‘1’, enable the debug IO into PLL
  *
  * @return Register value
  */
  uint32_t inline pll_ctrl_in_en_get() {
    return regmap->pll_ana_control.fields.pll_ctrl_in_en;
  }

  /** @brief HAL Setter of signal PLL_CTRL_IN_EN from register PLL_ANA_CONTROL
  *
  * When ‘1’, enable the debug IO into PLL
  *
  * @param pll_ctrl_in_en Value to be set
  */
  void inline pll_ctrl_in_en_set(uint32_t pll_ctrl_in_en) {
    regmap->pll_ana_control.fields.pll_ctrl_in_en = pll_ctrl_in_en;
  }

  /** @brief HAL Getter of signal PLL_FORCE_EN from register PLL_ANA_CONTROL
  *
  * When ‘1’, the PLL enabled is force to ‘1’
  *
  * @return Register value
  */
  uint32_t inline pll_force_en_get() {
    return regmap->pll_ana_control.fields.pll_force_en;
  }

  /** @brief HAL Setter of signal PLL_FORCE_EN from register PLL_ANA_CONTROL
  *
  * When ‘1’, the PLL enabled is force to ‘1’
  *
  * @param pll_force_en Value to be set
  */
  void inline pll_force_en_set(uint32_t pll_force_en) {
    regmap->pll_ana_control.fields.pll_force_en = pll_force_en;
  }

  /** @brief HAL Getter of signal PLL_FORCE_CLOSEDLOOP from register PLL_ANA_CONTROL
  *
  * When ‘1’, the PLL loop is force to be closed
  *
  * @return Register value
  */
  uint32_t inline pll_force_closedloop_get() {
    return regmap->pll_ana_control.fields.pll_force_closedloop;
  }

  /** @brief HAL Setter of signal PLL_FORCE_CLOSEDLOOP from register PLL_ANA_CONTROL
  *
  * When ‘1’, the PLL loop is force to be closed
  *
  * @param pll_force_closedloop Value to be set
  */
  void inline pll_force_closedloop_set(uint32_t pll_force_closedloop) {
    regmap->pll_ana_control.fields.pll_force_closedloop = pll_force_closedloop;
  }

  /** @brief HAL Getter of signal PLL_CAL_STATE from register PLL_STATUS
  *
  * PLL calibration state. States are {IDLE, STABILIZE_DELAY, MEASURE, END_DELAY}
  *
  * @return Register value
  */
  uint32_t inline pll_cal_state_get() {
    return regmap->pll_status.fields.pll_cal_state;
  }

  /** @brief HAL Getter of signal PLL_CAL_RUNNING from register PLL_STATUS
  *
  * Calibration running indicator
  *
  * @return Register value
  */
  uint32_t inline pll_cal_running_get() {
    return regmap->pll_status.fields.pll_cal_running;
  }

  /** @brief HAL Getter of signal PLL_LOCKED from register PLL_STATUS
  *
  * PLL locked flag
  *
  * @return Register value
  */
  uint32_t inline pll_locked_get() {
    return regmap->pll_status.fields.pll_locked;
  }

  /** @brief HAL Getter of signal PLL_VVCO_FLAG_L from register PLL_STATUS
  *
  * VCO voltage low flag
  *
  * @return Register value
  */
  uint32_t inline pll_vvco_flag_l_get() {
    return regmap->pll_status.fields.pll_vvco_flag_l;
  }

  /** @brief HAL Getter of signal PLL_VVCO_FLAG_H from register PLL_STATUS
  *
  * VCO voltage high flag
  *
  * @return Register value
  */
  uint32_t inline pll_vvco_flag_h_get() {
    return regmap->pll_status.fields.pll_vvco_flag_h;
  }

  /** @brief HAL Getter of signal PLL_COARSE_TUNE from register PLL_STATUS
  *
  * PLL coarse tuning value
  *
  * @return Register value
  */
  uint32_t inline pll_coarse_tune_get() {
    return regmap->pll_status.fields.pll_coarse_tune;
  }

  /** @brief HAL Getter of signal PLL_CAL_MEAS from register PLL_STATUS
  *
  * PLL VCO clock measurement
  *
  * @return Register value
  */
  uint32_t inline pll_cal_meas_get() {
    return regmap->pll_status.fields.pll_cal_meas;
  }

  /** @brief HAL Getter of signal CHIP_REV_MINOR from register CHIP_VERSION
  *
  * Hardware version minor
  *
  * @return Register value
  */
  uint32_t inline chip_rev_minor_get() {
    return regmap->chip_version.fields.chip_rev_minor;
  }

  /** @brief HAL Getter of signal CHIP_REV_MAJOR from register CHIP_VERSION
  *
  * Hardware version major
  *
  * @return Register value
  */
  uint32_t inline chip_rev_major_get() {
    return regmap->chip_version.fields.chip_rev_major;
  }

};
