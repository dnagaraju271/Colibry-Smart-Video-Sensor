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

#include "colibry_const.h"
#include "colibry_soc_mmap.h"
#include "scu_hal.hpp"

#define WATCHDOG_RESET_MODE_DISABLED  0
#define WATCHDOG_RESET_MODE_CORE_ONLY 1
#define WATCHDOG_RESET_MODE_FULL      3

#define HF_CLK_SRC_PLL      0
#define HF_CLK_SRC_EXTERNAL 1

#define CLKOUT_SRC_OFF         0
#define CLKOUT_SRC_EXT_HF_CLK  1
#define CLKOUT_SRC_PLL_DIV_CLK 2
#define CLKOUT_SRC_EXT_LF_CLK  3


typedef enum {
              HFCLK_EXTERNAL_CLOCK,            // External clock provided at HFXO0 I/O and used as HF_CLK
              HFCLK_PLL_OPENLOOP_UNCALIBRATED, // Internal PLL clock in open-loop uncalibrated mode
              HFCLK_PLL_OPENLOOP_CALIBRATED,   // Internal PLL clock in open-loop calibrated mode
              HFCLK_PLL_PHASE_LOCKED,          // Internal PLL clock in phase-locked mode
} hf_clk_mode_t;

typedef enum {
              PLL_MODE_DISABLED,
              PLL_MODE_OPENLOOP,
              PLL_MODE_FREQ_LOCK,
} scu_pll_mode_t;


/* Calibrated tables */
const uint8_t N_CAL_CODES = 20;

const uint8_t PLL_BYPASS_TUNE_CODES[N_CAL_CODES] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 31, 63, 127, 255};

const uint16_t PLL_BYPASS_FREQ[][N_CAL_CODES] = {
   {23, 36, 46, 52, 56, 59, 62, 63, 65, 66, 67, 68, 69, 71, 72, 73, 94, 130, 178, 222},                // 0V8
   {38, 58, 60, 69, 78, 84, 90, 93, 96, 98, 100, 101, 102, 103, 104, 105, 122, 161, 218, 277},         // 0V9
   {42, 67, 89, 105, 119, 128, 135, 140, 143, 145, 147, 148,  149, 149, 150, 151, 176, 231, 307, 375}, // 1V0
   {62, 99, 131, 156, 180, 198, 214, 226, 236, 242, 247, 249, 251, 251, 251, 251, 266, 337, 437, 525} // 1V2
};


class ScuDriver : public ScuHal {
 private:
  uint32_t get_min_freq_from_voltage(float vdd_voltage);
  uint8_t get_vco_code_from_freq(uint16_t target_freq_mhz, float vdd_voltage);
  
 public:
  uint32_t ext_hf_clk_freq_hz = 0;
  uint32_t hf_clk_freq_hz = 60000000;
  uint32_t hf_clk_freq_mhz = 60;
  
  uint32_t ext_lf_clk_freq_hz = 32768;

  ScuDriver();
  
  /** @brief Configure the clocking system to generate an HF_CLK clock at a target frequency. Based
   *         on the parameters provided, the function tries to obtain an HF_CLK as close as possible
   *         to the target.
   *
   * @param hf_clk_mode The mode used to generate the HF_CLK (see hf_clk_mode_t)
   * @param hfxo_with_quartz Should be set to 0 if no quartz is present at HFXOx I/Os; instead, a direct 
   *                         clock is provided if needed. If 1, an external quartz will be driven.
   * @param ext_freq_hz The external clock or quartz frequency (EXT_HF_CLK), in Hertz. Not relevant for 
   *                    HFCLK_PLL_OPENLOOP_UNCALIBRATED mode.
   * @param vdd_voltage Indication of the voltage applied to VDD. Not relevant in HFCLK_EXTERNAL_CLOCK mode.
   *                    If not known, should be set to 1.2.
   * @param target_freq_mhz Wanted HL_CLK frequency, in MHz. Not relevant in HFCLK_EXTERNAL_CLOCK mode.
   * @retval The estimated HL_CLK frequency obtained, in MHz. Set to 0 in case of parameters error. 
   */          
  uint16_t configure_hf_clk(hf_clk_mode_t hf_clk_mode,
                            uint8_t hfxo_with_quartz,
                            uint32_t ext_freq_hz,
                            float vdd_voltage,
                            uint16_t target_freq_mhz
                            );

  
  /** @brief Sets the hf_clk_freq_hz variable to indicated the current HL_CLK frequency. This function
   *         should be used for the system to know its frequency, in case configure_hf_clk() has not been
   *         called to configure the HF_CLK or if the clocking parameters have changed. 

   * @param frequency_hz The HF_CLK actual frequency in Hz.
   */
  void set_hf_clk_freq(int frequency_hz);

  
  /** @brief Gets the current HF_CLK frequency.

   * @retval The HF_CLK estimated frequency in Hz.
   */
  uint32_t get_hf_clk_freq();


  /** @brief Sets the ext_lf_clk_freq_hz variable to indicated the current EXT_LF_CLK frequency. 

   * @param frequency_hz The HF_CLK actual frequency in Hz.
   */
  void set_ext_lf_clk_freq(int frequency_hz);


  /** @brief Gets the current EXT_LF_CLK frequency.

   * @retval The EXT_LF_CLK estimated frequency in Hz.
   */
  uint32_t get_ext_lf_clk_freq();

  
  /** @brief Gets the current HF_CLK frequency in MHz.

   * @retval The HF_CLK estimated frequency in MHz.
   */
  uint32_t get_hf_clk_freq_mhz();

  
  /** @brief Gets the current AXI bus clock frequency.

   * @retval The AXI bus clock estimated frequency in Hz.
   */
  uint32_t get_axi_clk_freq();

  
  /** @brief Gets the current AXI bus clock frequency in MHz.

   * @retval The AXI bus clock estimated frequency in MHz.
   */
  uint32_t get_axi_clk_freq_mhz();

  
  /** @brief Gets the current APB bus clock frequency.

   * @retval The APB bus clock estimated frequency in Hz.
   */
  uint32_t get_apb_clk_freq();

  
  /** @brief Gets the current APB bus clock frequency in MHz.

   * @retval The APB bus clock estimated frequency in MHz.
   */
  uint32_t get_apb_clk_freq_mhz();
  

  /** @brief Generate a busy-wait delay in micro-seconds.
   *
   * @param delay_us The delay in micro-seconds.
   */
  void delay_us(uint32_t delay_us);

  
  /** @brief Generate a busy-wait delay in milli-seconds.
   *
   * @param delay_us The delay in milli-seconds.
   */
  void delay_ms(uint32_t delay_ms);
  
};
