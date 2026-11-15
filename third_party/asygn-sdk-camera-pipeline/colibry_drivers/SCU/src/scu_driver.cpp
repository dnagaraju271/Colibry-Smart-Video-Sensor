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



#include "scu_driver.hpp"
#include "peripherals.h"

ScuDriver::ScuDriver() {
  set_periph_addr(SCU_BASE_ADDR);
}
     
uint16_t ScuDriver::configure_hf_clk(hf_clk_mode_t hf_clk_mode,
                                     uint8_t hfxo_with_quartz,
                                     uint32_t ext_freq_hz,
                                     float vdd_voltage,
                                     uint16_t target_freq_mhz
                                     )
{
  uint16_t min_frequency_mhz;
  uint16_t target_freq_mhz_prediv;
  uint16_t post_div_req;
  uint32_t cal_periods;

  // TODO: get_vco_code_from_freq
  // TODO: Check max freq vs voltage
  // TODO: Use the slow bit or gain
  
  // HFXO configuration
  if (hfxo_with_quartz) {
    colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_mode = HFXO_MODE_CRYSTAL;
    if (ext_freq_hz <= 4000000)
      colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_fsel = HFXO_FSEL_1M_4M;
    else if (ext_freq_hz <= 12000000)
      colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_fsel = HFXO_FSEL_4M_12M;
    else if (ext_freq_hz <= 24000000)
      colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_fsel = HFXO_FSEL_12M_24M;
    else
      colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_fsel = HFXO_FSEL_24M_48M;
  }
  else if (hf_clk_mode == HFCLK_PLL_OPENLOOP_UNCALIBRATED) {
    colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_mode = HFXO_MODE_OFF;
  }
  else {
    colibryPORT->regmap->port_hfxo_cfg.fields.port_hfxo_mode = HFXO_MODE_EXT_CLOCK;
  }
  
  // Clocking settings based on HF_CLK mode
  if (hf_clk_mode == HFCLK_EXTERNAL_CLOCK) {
    regmap->scu_hf_clk_cfg.fields.scu_hf_clk_src = HF_CLK_SRC_EXTERNAL;
    if (colibryPORT->regmap->port_dio_cfg[31].fields.port_dio_function == 3)
      colibryPORT->regmap->port_dio_cfg[31].fields.port_dio_function = 0;  // Ensure PLL clock not selected
    regmap->pll_control.fields.pll_en_mode = PLL_MODE_DISABLED;
    ext_hf_clk_freq_hz = ext_freq_hz;
    hf_clk_freq_hz = ext_freq_hz;
  }
  else {
    regmap->scu_hf_clk_cfg.fields.scu_hf_clk_src = HF_CLK_SRC_PLL;
    // Manage the case when target frequency is below the PLL minimum achievable with enough accuracy
    min_frequency_mhz = get_min_freq_from_voltage(vdd_voltage);
    if (target_freq_mhz < min_frequency_mhz) {
      post_div_req = (min_frequency_mhz + target_freq_mhz - 1) / target_freq_mhz;
      if (post_div_req > 16) {
        post_div_req = 16;
        target_freq_mhz_prediv = ((min_frequency_mhz + 8) / 16) * post_div_req;
      }
      else {
        target_freq_mhz_prediv = target_freq_mhz * post_div_req;
      }
      regmap->pll_control.fields.pll_post_div = post_div_req - 1;
    }
    else {
      target_freq_mhz_prediv = target_freq_mhz;
      regmap->pll_control.fields.pll_post_div = 0;
    }
    hf_clk_freq_hz = target_freq_mhz_prediv * 1000000 / (regmap->pll_control.fields.pll_post_div + 1);
         
    // Clocking settings based on the PLL
    if (hf_clk_mode == HFCLK_PLL_OPENLOOP_UNCALIBRATED) {
      regmap->pll_cal_bypass.fields.pll_cal_bypass_val = get_vco_code_from_freq(target_freq_mhz_prediv, vdd_voltage);
      regmap->pll_cal_bypass.fields.pll_cal_bypass_en = 1;
      regmap->pll_control.fields.pll_en_mode = PLL_MODE_OPENLOOP;
    }
    else if (hf_clk_mode == HFCLK_PLL_OPENLOOP_CALIBRATED) {
      regmap->pll_control.fields.pll_freq_mult = (target_freq_mhz_prediv / 4) - 1; // Targeting 4 MHz after internal divider
      if (ext_freq_hz >= 4000000) {
        regmap->pll_cal_config.fields.pll_cal_target = 400 * 4000000 / ext_freq_hz;
        regmap->pll_cal_config.fields.pll_cal_periods = 400;
      }
      else {
        cal_periods = (400 * ext_freq_hz + 3900000) / 4000000;
        regmap->pll_cal_config.fields.pll_cal_periods = cal_periods;
        regmap->pll_cal_config.fields.pll_cal_target = cal_periods * 4000000 / ext_freq_hz;
      }
      regmap->pll_cal_bypass.fields.pll_cal_bypass_en = 0;
      regmap->pll_control.fields.pll_en_mode = PLL_MODE_OPENLOOP;
      regmap->pll_control.fields.pll_restart = 1;
      // Wait for the end of calibration
      for (int i = 0; i < 10000; i++) asm volatile ("nop");
      while (colibrySCU->regmap->pll_status.word & PLL_CAL_RUNNING_MASK) ;
    }
    else {  // hf_clk_mode == HFCLK_PLL_PHASE_LOCKED
      regmap->pll_control.fields.pll_freq_mult = (target_freq_mhz_prediv * 1000000 / ext_freq_hz) - 1;
      regmap->pll_cal_bypass.fields.pll_cal_bypass_en = 0;
      regmap->pll_control.fields.pll_en_mode = PLL_MODE_FREQ_LOCK;
      regmap->pll_control.fields.pll_restart = 1;
    }
  }
  // Compute the real HFCLK frequency. NOTE: need to wait a fix in hardware to always get the correct measurement.
  //if (hf_clk_mode == HFCLK_PLL_OPENLOOP_CALIBRATED) {
  //  hf_clk_freq_hz = (uint32_t) ext_freq_hz * (regmap->pll_control.fields.pll_freq_mult + 1) *
  //                   colibrySCU->regmap->pll_status.fields.pll_cal_meas / cal_periods;
  //}
  
  hf_clk_freq_mhz = (hf_clk_freq_hz + 500000) / 1000000;
  return hf_clk_freq_mhz;
}

uint32_t ScuDriver::get_min_freq_from_voltage(float vdd_voltage) {
  // Note: factor *3 allows a better frequency accuracy
  // TODO: replace by a slower PLL configuration
  if (vdd_voltage < 0.8)
    return 23 * 3;
  else if (vdd_voltage < 0.9)
    return 38 * 3;
  else if (vdd_voltage < 1.0)
    return 42 * 3;
  else if (vdd_voltage < 1.1)
    return 52 * 3;
  else
    return 62 * 3;
}


uint8_t ScuDriver::get_vco_code_from_freq(uint16_t target_freq_mhz, float vdd_voltage) {
  uint8_t table_index;
  float voltage_ratio;
  uint16_t freq_l, freq_h;
  uint8_t code_l, code_h;
  uint32_t interpol_ratio;
  uint16_t interp_pll_freq[N_CAL_CODES];
  int i;
  
  // Find the voltage table to use
  if (vdd_voltage < 0.8f) {
    table_index = 0;
    voltage_ratio = 0;
  }
  else if (vdd_voltage < 0.9f) {
    table_index = 0;
    voltage_ratio = vdd_voltage - 0.8f;
  }
  else if (vdd_voltage < 1.0f) {
    table_index = 1;
    voltage_ratio = vdd_voltage - 0.9f;
  }
  else if (vdd_voltage < 1.2f) {
    table_index = 2;
    voltage_ratio = vdd_voltage - 1.0f;
  }
  else {
    table_index = 3;
    voltage_ratio = 1.0f;
  }

  // Construct an interpolated freq table wrt the voltage
  for (i = 0; i < N_CAL_CODES; i++) {
    if (table_index == 3)
      interp_pll_freq[i] = PLL_BYPASS_FREQ[3][i];
    else
      interp_pll_freq[i] = PLL_BYPASS_FREQ[table_index][i] + voltage_ratio *
        (PLL_BYPASS_FREQ[table_index + 1][i] - PLL_BYPASS_FREQ[table_index][i]);
  }

  // Find the tune code
  if (target_freq_mhz <= interp_pll_freq[0])
    return PLL_BYPASS_TUNE_CODES[0];
  
  for (i = 1; i < N_CAL_CODES; i++) {
    if (target_freq_mhz <= interp_pll_freq[i]) {
      break;
    }
  }
  freq_h = interp_pll_freq[i];
  freq_l = interp_pll_freq[i-1];
  code_h = PLL_BYPASS_TUNE_CODES[i];
  code_l = PLL_BYPASS_TUNE_CODES[i-1];
  interpol_ratio = (uint32_t) (1 << 16) * (target_freq_mhz - freq_l) / (freq_h - freq_l);
      
  return code_l + (interpol_ratio * (code_h - code_l) + (1 << 15)) / (1 << 16);
}


void ScuDriver::set_hf_clk_freq(int frequency_hz)
{
  hf_clk_freq_hz = frequency_hz;
  hf_clk_freq_mhz = frequency_hz / 1000000;
}

uint32_t ScuDriver::get_hf_clk_freq()
{
  return hf_clk_freq_hz;
}

uint32_t ScuDriver::get_hf_clk_freq_mhz()
{
  return hf_clk_freq_mhz;
}

void ScuDriver::set_ext_lf_clk_freq(int frequency_hz)
{
  ext_lf_clk_freq_hz = frequency_hz;
}

uint32_t ScuDriver::get_ext_lf_clk_freq()
{
  return ext_lf_clk_freq_hz;
}

uint32_t ScuDriver::get_axi_clk_freq()
{
  return get_hf_clk_freq() / (1 +  regmap->scu_axi_clk_cfg.fields.scu_axi_clk_div);
}

uint32_t ScuDriver::get_axi_clk_freq_mhz()
{
  return get_hf_clk_freq_mhz() / (1 +  regmap->scu_axi_clk_cfg.fields.scu_axi_clk_div);
}

uint32_t ScuDriver::get_apb_clk_freq()
{
  return get_axi_clk_freq() / (1 +  regmap->scu_apb_clk_cfg.fields.scu_apb_clk_div);
}

uint32_t ScuDriver::get_apb_clk_freq_mhz()
{
  return get_axi_clk_freq_mhz() / (1 +  regmap->scu_apb_clk_cfg.fields.scu_apb_clk_div);
}

void ScuDriver::delay_us(uint32_t delay_us) {
  int target_cycles;
  volatile int cycles_cnt = 0;

  target_cycles = get_axi_clk_freq_mhz() * delay_us - 24;
  
  while (cycles_cnt < target_cycles) {
    cycles_cnt += 12;
  }
}

void ScuDriver::delay_ms(uint32_t delay_ms) {
  uint32_t ms_cnt = 0;
  
  while (ms_cnt < delay_ms) {
    delay_us(1000);
    ms_cnt += 1;
  }
}
