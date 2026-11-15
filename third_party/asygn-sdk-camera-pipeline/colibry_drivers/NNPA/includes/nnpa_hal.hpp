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

#include "nnpa_mmap.h"


/// @brief HAL script-generated layer of nnpa register bank.
/// @note This code needs a C++ compiler to be used.
class NnpaHal {
 public:
  volatile s_nnpa_reg_type* regmap;

  NnpaHal() {
  }

  void inline set_periph_addr(uint32_t address) {
    regmap = (s_nnpa_reg_type*) address;
  }

  /** @brief HAL Getter of signal NNPA_REV_MINOR from register NNPA_VERSION
  *
  * NNPA hardware revision minor
  *
  * @return Register value
  */
  uint32_t inline nnpa_rev_minor_get() {
    return regmap->nnpa_version.fields.nnpa_rev_minor;
  }

  /** @brief HAL Getter of signal NNPA_REV_MAJOR from register NNPA_VERSION
  *
  * NNPA hardware revision major
  *
  * @return Register value
  */
  uint32_t inline nnpa_rev_major_get() {
    return regmap->nnpa_version.fields.nnpa_rev_major;
  }

  /** @brief HAL Trigger 'write pulse' of signal CONFIG_RELOAD from register NNPA_TRIG_CONTROL
  *
  * Writing ‘1’ reloads the NN configuration (NNPA bitstream) stored at CONFIG_START_MADDR address
  *
  */
  void inline config_reload() {
    regmap->nnpa_trig_control.word = CONFIG_RELOAD_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal CONFIG_CHECK from register NNPA_TRIG_CONTROL
  *
  * Writing ‘1’ launches the NN configuration check procedure
  *
  */
  void inline config_check() {
    regmap->nnpa_trig_control.word = CONFIG_CHECK_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal INPUT_LOADER_RST from register NNPA_TRIG_CONTROL
  *
  * Writing ‘1’ restarts the INPUT LOADER and discards any previously loaded data
  *
  */
  void inline input_loader_rst() {
    regmap->nnpa_trig_control.word = INPUT_LOADER_RST_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal RUN_START from register NNPA_TRIG_CONTROL
  *
  * Writing ‘1’ starts NNPA inference processing
  *
  */
  void inline run_start() {
    regmap->nnpa_trig_control.word = RUN_START_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal INF_COUNTER_CLEAR from register NNPA_TRIG_CONTROL
  *
  * Writing ‘1’ erases the inference counter INFERENCE_COUNTER
  *
  */
  void inline inf_counter_clear() {
    regmap->nnpa_trig_control.word = INF_COUNTER_CLEAR_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal XTEN_PAUSE_RESUME from register NNPA_TRIG_CONTROL
  *
  * Writing ‘1’ resumes the NNPA processing after an XTEN pause
  *
  */
  void inline xten_pause_resume() {
    regmap->nnpa_trig_control.word = XTEN_PAUSE_RESUME_MASK;
  }

  /** @brief HAL Getter of signal BLOCK_RESET from register NNPA_CONTROL
  *
  * When ‘1’, the NNPA core is under reset but the registers configuration and memory content are preserved
  *
  * @return Register value
  */
  uint32_t inline block_reset_get() {
    return regmap->nnpa_control.fields.block_reset;
  }

  /** @brief HAL Setter of signal BLOCK_RESET from register NNPA_CONTROL
  *
  * When ‘1’, the NNPA core is under reset but the registers configuration and memory content are preserved
  *
  * @param block_reset Value to be set
  */
  void inline block_reset_set(uint32_t block_reset) {
    regmap->nnpa_control.fields.block_reset = block_reset;
  }

  /** @brief HAL Getter of signal MANUAL_BUFF_PD from register NNPA_CONTROL
  *
  * Manual control of buffers power mode. When this bit is set, MEM_BUFF_PWR_CTRLx is used to define each buffer power mode
  *
  * @return Register value
  */
  uint32_t inline manual_buff_pd_get() {
    return regmap->nnpa_control.fields.manual_buff_pd;
  }

  /** @brief HAL Setter of signal MANUAL_BUFF_PD from register NNPA_CONTROL
  *
  * Manual control of buffers power mode. When this bit is set, MEM_BUFF_PWR_CTRLx is used to define each buffer power mode
  *
  * @param manual_buff_pd Value to be set
  */
  void inline manual_buff_pd_set(uint32_t manual_buff_pd) {
    regmap->nnpa_control.fields.manual_buff_pd = manual_buff_pd;
  }

  /** @brief HAL Getter of signal AUTO_START_EN from register NNPA_CONTROL
  *
  * When ‘1’, the NN inference processing is automatically started once all input data are received through PSB
  *
  * @return Register value
  */
  uint32_t inline auto_start_en_get() {
    return regmap->nnpa_control.fields.auto_start_en;
  }

  /** @brief HAL Setter of signal AUTO_START_EN from register NNPA_CONTROL
  *
  * When ‘1’, the NN inference processing is automatically started once all input data are received through PSB
  *
  * @param auto_start_en Value to be set
  */
  void inline auto_start_en_set(uint32_t auto_start_en) {
    regmap->nnpa_control.fields.auto_start_en = auto_start_en;
  }

  /** @brief HAL Getter of signal CPU_MEM_ACCESS from register NNPA_CONTROL
  *
  * When ‘1’, the NNPA memory can be accessed by the system. When ‘0’, the NNPA memory is reserved to the core
  *
  * @return Register value
  */
  uint32_t inline cpu_mem_access_get() {
    return regmap->nnpa_control.fields.cpu_mem_access;
  }

  /** @brief HAL Setter of signal CPU_MEM_ACCESS from register NNPA_CONTROL
  *
  * When ‘1’, the NNPA memory can be accessed by the system. When ‘0’, the NNPA memory is reserved to the core
  *
  * @param cpu_mem_access Value to be set
  */
  void inline cpu_mem_access_set(uint32_t cpu_mem_access) {
    regmap->nnpa_control.fields.cpu_mem_access = cpu_mem_access;
  }

  /** @brief HAL Getter of signal NNPA_CLK_FORCE_EN from register NNPA_CONTROL
  *
  * When ‘1’, the NNPA clock is forced to be active
  *
  * @return Register value
  */
  uint32_t inline nnpa_clk_force_en_get() {
    return regmap->nnpa_control.fields.nnpa_clk_force_en;
  }

  /** @brief HAL Setter of signal NNPA_CLK_FORCE_EN from register NNPA_CONTROL
  *
  * When ‘1’, the NNPA clock is forced to be active
  *
  * @param nnpa_clk_force_en Value to be set
  */
  void inline nnpa_clk_force_en_set(uint32_t nnpa_clk_force_en) {
    regmap->nnpa_control.fields.nnpa_clk_force_en = nnpa_clk_force_en;
  }

  /** @brief HAL Getter of signal MEM_CLK_FORCE_EN from register NNPA_CONTROL
  *
  * When ‘1’, the clocks to all memory buffers are forced to be active
  *
  * @return Register value
  */
  uint32_t inline mem_clk_force_en_get() {
    return regmap->nnpa_control.fields.mem_clk_force_en;
  }

  /** @brief HAL Setter of signal MEM_CLK_FORCE_EN from register NNPA_CONTROL
  *
  * When ‘1’, the clocks to all memory buffers are forced to be active
  *
  * @param mem_clk_force_en Value to be set
  */
  void inline mem_clk_force_en_set(uint32_t mem_clk_force_en) {
    regmap->nnpa_control.fields.mem_clk_force_en = mem_clk_force_en;
  }

  /** @brief HAL Getter of signal NNPA_BUSY from register NNPA_STATUS
  *
  * Flag high when the NNPA core is running
  *
  * @return Register value
  */
  uint32_t inline nnpa_busy_get() {
    return regmap->nnpa_status.fields.nnpa_busy;
  }

  /** @brief HAL Getter of signal CONFIG_CHECK_OK from register NNPA_STATUS
  *
  * Flag high when the NN configuration check procedure is passed
  *
  * @return Register value
  */
  uint32_t inline config_check_ok_get() {
    return regmap->nnpa_status.fields.config_check_ok;
  }

  /** @brief HAL Getter of signal INPUT_LOADING_RDY from register NNPA_STATUS
  *
  * Flag high when the NNPA is ready to receive input data
  *
  * @return Register value
  */
  uint32_t inline input_loading_rdy_get() {
    return regmap->nnpa_status.fields.input_loading_rdy;
  }

  /** @brief HAL Getter of signal INPUT_LOADER_FULL from register NNPA_STATUS
  *
  * Flag high when the INPUT LOADER has received the expected number of data
  *
  * @return Register value
  */
  uint32_t inline input_loader_full_get() {
    return regmap->nnpa_status.fields.input_loader_full;
  }

  /** @brief HAL Getter of signal XTEN_PAUSE_STATE from register NNPA_STATUS
  *
  * Flag high when the NNPA is in an XTEN pause state
  *
  * @return Register value
  */
  uint32_t inline xten_pause_state_get() {
    return regmap->nnpa_status.fields.xten_pause_state;
  }

  /** @brief HAL Getter of signal RUN_LAYER_IDX from register NNPA_STATUS
  *
  * Current processed layer index while running an inference
  *
  * @return Register value
  */
  uint32_t inline run_layer_idx_get() {
    return regmap->nnpa_status.fields.run_layer_idx;
  }

  /** @brief HAL Getter of signal NB_DATA_BUFF from register NB_DATA_BUFF
  *
  * Number of buffers allocated to NN execution. If NB_DATA_BUFF=0, this number is automatically set from the bitstream information
  *
  * @return Register value
  */
  uint32_t inline nb_data_buff_get() {
    return regmap->nb_data_buff.fields.nb_data_buff;
  }

  /** @brief HAL Setter of signal NB_DATA_BUFF from register NB_DATA_BUFF
  *
  * Number of buffers allocated to NN execution. If NB_DATA_BUFF=0, this number is automatically set from the bitstream information
  *
  * @param nb_data_buff Value to be set
  */
  void inline nb_data_buff_set(uint32_t nb_data_buff) {
    regmap->nb_data_buff.fields.nb_data_buff = nb_data_buff;
  }

  /** @brief HAL Getter of signal CONFIG_START_MADDR from register CONFIG_START_MADDR
  *
  * NN configuration start address in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline config_start_maddr_get() {
    return regmap->config_start_maddr.fields.config_start_maddr;
  }

  /** @brief HAL Setter of signal CONFIG_START_MADDR from register CONFIG_START_MADDR
  *
  * NN configuration start address in NNPA memory words
  *
  * @param config_start_maddr Value to be set
  */
  void inline config_start_maddr_set(uint32_t config_start_maddr) {
    regmap->config_start_maddr.fields.config_start_maddr = config_start_maddr;
  }

  /** @brief HAL Getter of signal INPUT_START_MADDR from register INPUT_START_MADDR
  *
  * NN input data start address in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline input_start_maddr_get() {
    return regmap->input_start_maddr.fields.input_start_maddr;
  }

  /** @brief HAL Setter of signal INPUT_START_MADDR from register INPUT_START_MADDR
  *
  * NN input data start address in NNPA memory words
  *
  * @param input_start_maddr Value to be set
  */
  void inline input_start_maddr_set(uint32_t input_start_maddr) {
    regmap->input_start_maddr.fields.input_start_maddr = input_start_maddr;
  }

  /** @brief HAL Getter of signal DATA_START_MADDR from register DATA_START_MADDR
  *
  * NN activations start address in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline data_start_maddr_get() {
    return regmap->data_start_maddr.fields.data_start_maddr;
  }

  /** @brief HAL Setter of signal DATA_START_MADDR from register DATA_START_MADDR
  *
  * NN activations start address in NNPA memory words
  *
  * @param data_start_maddr Value to be set
  */
  void inline data_start_maddr_set(uint32_t data_start_maddr) {
    regmap->data_start_maddr.fields.data_start_maddr = data_start_maddr;
  }

  /** @brief HAL Getter of signal MEM_BUFF_PD0 from register MEM_BUFF_PD0
  *
  * Memory buffers power-down control. Each bit corresponds to memory buffer 0 to 31:
  * 0: Normal mode
  * 1: Power off, buffer cannot be read or written
  *
  * @return Register value
  */
  uint32_t inline mem_buff_pd0_get() {
    return regmap->mem_buff_pd0.fields.mem_buff_pd0;
  }

  /** @brief HAL Setter of signal MEM_BUFF_PD0 from register MEM_BUFF_PD0
  *
  * Memory buffers power-down control. Each bit corresponds to memory buffer 0 to 31:
  * 0: Normal mode
  * 1: Power off, buffer cannot be read or written
  *
  * @param mem_buff_pd0 Value to be set
  */
  void inline mem_buff_pd0_set(uint32_t mem_buff_pd0) {
    regmap->mem_buff_pd0.fields.mem_buff_pd0 = mem_buff_pd0;
  }

  /** @brief HAL Getter of signal MEM_BUFF_PD1 from register MEM_BUFF_PD1
  *
  * Memory buffers power-down control. Each bit corresponds to memory buffer 32 to 39:
  * 0: Normal mode
  * 1: Power off, buffer cannot be read or written
  *
  * @return Register value
  */
  uint32_t inline mem_buff_pd1_get() {
    return regmap->mem_buff_pd1.fields.mem_buff_pd1;
  }

  /** @brief HAL Setter of signal MEM_BUFF_PD1 from register MEM_BUFF_PD1
  *
  * Memory buffers power-down control. Each bit corresponds to memory buffer 32 to 39:
  * 0: Normal mode
  * 1: Power off, buffer cannot be read or written
  *
  * @param mem_buff_pd1 Value to be set
  */
  void inline mem_buff_pd1_set(uint32_t mem_buff_pd1) {
    regmap->mem_buff_pd1.fields.mem_buff_pd1 = mem_buff_pd1;
  }

  /** @brief HAL Getter of signal MEM_BUFF_SLEEP_AUTO from register MEM_BUFF_SLEEP
  *
  * When ‘1’, the memory buffers sleep mode is automatically controlled
  *
  * @return Register value
  */
  uint32_t inline mem_buff_sleep_auto_get() {
    return regmap->mem_buff_sleep.fields.mem_buff_sleep_auto;
  }

  /** @brief HAL Setter of signal MEM_BUFF_SLEEP_AUTO from register MEM_BUFF_SLEEP
  *
  * When ‘1’, the memory buffers sleep mode is automatically controlled
  *
  * @param mem_buff_sleep_auto Value to be set
  */
  void inline mem_buff_sleep_auto_set(uint32_t mem_buff_sleep_auto) {
    regmap->mem_buff_sleep.fields.mem_buff_sleep_auto = mem_buff_sleep_auto;
  }

  /** @brief HAL Getter of signal MEM_BUFF_SLEEP_EN from register MEM_BUFF_SLEEP
  *
  * Memory buffers sleep mode enabled (‘1’) or disabled (‘0’) when MEM_BUFF_SLEEP_AUTO=0
  *
  * @return Register value
  */
  uint32_t inline mem_buff_sleep_en_get() {
    return regmap->mem_buff_sleep.fields.mem_buff_sleep_en;
  }

  /** @brief HAL Setter of signal MEM_BUFF_SLEEP_EN from register MEM_BUFF_SLEEP
  *
  * Memory buffers sleep mode enabled (‘1’) or disabled (‘0’) when MEM_BUFF_SLEEP_AUTO=0
  *
  * @param mem_buff_sleep_en Value to be set
  */
  void inline mem_buff_sleep_en_set(uint32_t mem_buff_sleep_en) {
    regmap->mem_buff_sleep.fields.mem_buff_sleep_en = mem_buff_sleep_en;
  }

  /** @brief HAL Getter of signal CUTS_CTRL_TEST1 from register MEM_BUFF_CUTS_CTRL
  *
  * Test mode to bypass self-timed circuit.The external clock controls the read and write control signals
  *
  * @return Register value
  */
  uint32_t inline cuts_ctrl_test1_get() {
    return regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_test1;
  }

  /** @brief HAL Setter of signal CUTS_CTRL_TEST1 from register MEM_BUFF_CUTS_CTRL
  *
  * Test mode to bypass self-timed circuit.The external clock controls the read and write control signals
  *
  * @param cuts_ctrl_test1 Value to be set
  */
  void inline cuts_ctrl_test1_set(uint32_t cuts_ctrl_test1) {
    regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_test1 = cuts_ctrl_test1;
  }

  /** @brief HAL Getter of signal CUTS_CTRL_RME from register MEM_BUFF_CUTS_CTRL
  *
  * Read-Write margin enable
  *
  * @return Register value
  */
  uint32_t inline cuts_ctrl_rme_get() {
    return regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_rme;
  }

  /** @brief HAL Setter of signal CUTS_CTRL_RME from register MEM_BUFF_CUTS_CTRL
  *
  * Read-Write margin enable
  *
  * @param cuts_ctrl_rme Value to be set
  */
  void inline cuts_ctrl_rme_set(uint32_t cuts_ctrl_rme) {
    regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_rme = cuts_ctrl_rme;
  }

  /** @brief HAL Getter of signal CUTS_CTRL_RM from register MEM_BUFF_CUTS_CTRL
  *
  * Read-Write margin
  *
  * @return Register value
  */
  uint32_t inline cuts_ctrl_rm_get() {
    return regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_rm;
  }

  /** @brief HAL Setter of signal CUTS_CTRL_RM from register MEM_BUFF_CUTS_CTRL
  *
  * Read-Write margin
  *
  * @param cuts_ctrl_rm Value to be set
  */
  void inline cuts_ctrl_rm_set(uint32_t cuts_ctrl_rm) {
    regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_rm = cuts_ctrl_rm;
  }

  /** @brief HAL Getter of signal CUTS_CTRL_RA from register MEM_BUFF_CUTS_CTRL
  *
  * Read Assist setting
  *
  * @return Register value
  */
  uint32_t inline cuts_ctrl_ra_get() {
    return regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_ra;
  }

  /** @brief HAL Setter of signal CUTS_CTRL_RA from register MEM_BUFF_CUTS_CTRL
  *
  * Read Assist setting
  *
  * @param cuts_ctrl_ra Value to be set
  */
  void inline cuts_ctrl_ra_set(uint32_t cuts_ctrl_ra) {
    regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_ra = cuts_ctrl_ra;
  }

  /** @brief HAL Getter of signal CUTS_CTRL_WA from register MEM_BUFF_CUTS_CTRL
  *
  * Write Assist setting
  *
  * @return Register value
  */
  uint32_t inline cuts_ctrl_wa_get() {
    return regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_wa;
  }

  /** @brief HAL Setter of signal CUTS_CTRL_WA from register MEM_BUFF_CUTS_CTRL
  *
  * Write Assist setting
  *
  * @param cuts_ctrl_wa Value to be set
  */
  void inline cuts_ctrl_wa_set(uint32_t cuts_ctrl_wa) {
    regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_wa = cuts_ctrl_wa;
  }

  /** @brief HAL Getter of signal CUTS_CTRL_WPULSE from register MEM_BUFF_CUTS_CTRL
  *
  * Write Assist pulse width setting
  *
  * @return Register value
  */
  uint32_t inline cuts_ctrl_wpulse_get() {
    return regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_wpulse;
  }

  /** @brief HAL Setter of signal CUTS_CTRL_WPULSE from register MEM_BUFF_CUTS_CTRL
  *
  * Write Assist pulse width setting
  *
  * @param cuts_ctrl_wpulse Value to be set
  */
  void inline cuts_ctrl_wpulse_set(uint32_t cuts_ctrl_wpulse) {
    regmap->mem_buff_cuts_ctrl.fields.cuts_ctrl_wpulse = cuts_ctrl_wpulse;
  }

  /** @brief HAL Getter of signal INPUT_LOADER_DATA from register INPUT_LOADER_DATA
  *
  * When written, loads the next four input bytes (LSB first) through the input loader
  *
  * @return Register value
  */
  uint32_t inline input_loader_data_get() {
    return regmap->input_loader_data.fields.input_loader_data;
  }

  /** @brief HAL Setter of signal INPUT_LOADER_DATA from register INPUT_LOADER_DATA
  *
  * When written, loads the next four input bytes (LSB first) through the input loader
  *
  * @param input_loader_data Value to be set
  */
  void inline input_loader_data_set(uint32_t input_loader_data) {
    regmap->input_loader_data.fields.input_loader_data = input_loader_data;
  }

  /** @brief HAL Getter of signal LOADER_NBYTE_VALID from register INPUT_LOADER_CFG
  *
  * Number of valid bytes minus 1 that are provided each time INPUT_LOADER_DATA is written to. Only relevant when LOADER_NBIT_MODE=0
  *
  * @return Register value
  */
  uint32_t inline loader_nbyte_valid_get() {
    return regmap->input_loader_cfg.fields.loader_nbyte_valid;
  }

  /** @brief HAL Setter of signal LOADER_NBYTE_VALID from register INPUT_LOADER_CFG
  *
  * Number of valid bytes minus 1 that are provided each time INPUT_LOADER_DATA is written to. Only relevant when LOADER_NBIT_MODE=0
  *
  * @param loader_nbyte_valid Value to be set
  */
  void inline loader_nbyte_valid_set(uint32_t loader_nbyte_valid) {
    regmap->input_loader_cfg.fields.loader_nbyte_valid = loader_nbyte_valid;
  }

  /** @brief HAL Getter of signal LOADER_NBIT_MODE from register INPUT_LOADER_CFG
  *
  * Number of bits of loaded data.
  * 0: 8-bit mode (multi-byte enabled)
  * 1: 10-bit mode
  * 2: 12-bit mode
  * 3: 14-bit mode
  * 4: 16-bit mode
  *
  * @return Register value
  */
  uint32_t inline loader_nbit_mode_get() {
    return regmap->input_loader_cfg.fields.loader_nbit_mode;
  }

  /** @brief HAL Setter of signal LOADER_NBIT_MODE from register INPUT_LOADER_CFG
  *
  * Number of bits of loaded data.
  * 0: 8-bit mode (multi-byte enabled)
  * 1: 10-bit mode
  * 2: 12-bit mode
  * 3: 14-bit mode
  * 4: 16-bit mode
  *
  * @param loader_nbit_mode Value to be set
  */
  void inline loader_nbit_mode_set(uint32_t loader_nbit_mode) {
    regmap->input_loader_cfg.fields.loader_nbit_mode = loader_nbit_mode;
  }

  /** @brief HAL Getter of signal LOADER_SIGNED_DATA from register INPUT_LOADER_CFG
  *
  * When ‘1’, the INPUT LOADER handles inputs as 2’s complement signed data. Otherwise it considers unsigned data
  *
  * @return Register value
  */
  uint32_t inline loader_signed_data_get() {
    return regmap->input_loader_cfg.fields.loader_signed_data;
  }

  /** @brief HAL Setter of signal LOADER_SIGNED_DATA from register INPUT_LOADER_CFG
  *
  * When ‘1’, the INPUT LOADER handles inputs as 2’s complement signed data. Otherwise it considers unsigned data
  *
  * @param loader_signed_data Value to be set
  */
  void inline loader_signed_data_set(uint32_t loader_signed_data) {
    regmap->input_loader_cfg.fields.loader_signed_data = loader_signed_data;
  }

  /** @brief HAL Getter of signal INPUT_LOADER_LEVEL from register INPUT_LOADER_LEVEL
  *
  * Current number of bytes loaded through the input loader for a given frame
  *
  * @return Register value
  */
  uint32_t inline input_loader_level_get() {
    return regmap->input_loader_level.fields.input_loader_level;
  }

  /** @brief HAL Getter of signal INPUT_SCALING_FXP from register INPUT_DATA_RESCALE
  *
  * Scaling factor applied to all input data in fixed-point 4.12 format (4 bits for integer part, 12 bits for decimal part)
  *
  * @return Register value
  */
  uint32_t inline input_scaling_fxp_get() {
    return regmap->input_data_rescale.fields.input_scaling_fxp;
  }

  /** @brief HAL Setter of signal INPUT_SCALING_FXP from register INPUT_DATA_RESCALE
  *
  * Scaling factor applied to all input data in fixed-point 4.12 format (4 bits for integer part, 12 bits for decimal part)
  *
  * @param input_scaling_fxp Value to be set
  */
  void inline input_scaling_fxp_set(uint32_t input_scaling_fxp) {
    regmap->input_data_rescale.fields.input_scaling_fxp = input_scaling_fxp;
  }

  /** @brief HAL Getter of signal INPUT_OFFSET_INT from register INPUT_DATA_RESCALE
  *
  * 2’s complement signed offset added to all input data
  *
  * @return Register value
  */
  uint32_t inline input_offset_int_get() {
    return regmap->input_data_rescale.fields.input_offset_int;
  }

  /** @brief HAL Setter of signal INPUT_OFFSET_INT from register INPUT_DATA_RESCALE
  *
  * 2’s complement signed offset added to all input data
  *
  * @param input_offset_int Value to be set
  */
  void inline input_offset_int_set(uint32_t input_offset_int) {
    regmap->input_data_rescale.fields.input_offset_int = input_offset_int;
  }

  /** @brief HAL Getter of signal RD_MODEL_ID from register RD_MODEL_ID
  *
  * Identifier of the loaded model
  *
  * @return Register value
  */
  uint32_t inline rd_model_id_get() {
    return regmap->rd_model_id.fields.rd_model_id;
  }

  /** @brief HAL Getter of signal RD_INPUT_SCALING from register RD_INPUT_SCALING
  *
  * Required input data scaling in fixed-point 4.12 format
  *
  * @return Register value
  */
  uint32_t inline rd_input_scaling_get() {
    return regmap->rd_input_scaling.fields.rd_input_scaling;
  }

  /** @brief HAL Getter of signal RD_INPUT_OFFSET from register RD_INPUT_OFFSET
  *
  * Required input data offset in 2’s complement format
  *
  * @return Register value
  */
  uint32_t inline rd_input_offset_get() {
    return regmap->rd_input_offset.fields.rd_input_offset;
  }

  /** @brief HAL Getter of signal RD_OUTPUT_SCALING from register RD_OUTPUT_SCALING
  *
  * Output data scaling for real number conversion
  *
  * @return Register value
  */
  uint32_t inline rd_output_scaling_get() {
    return regmap->rd_output_scaling.fields.rd_output_scaling;
  }

  /** @brief HAL Getter of signal RD_OUTPUT_OFFSET from register RD_OUTPUT_OFFSET
  *
  * Output data offset for real number conversion
  *
  * @return Register value
  */
  uint32_t inline rd_output_offset_get() {
    return regmap->rd_output_offset.fields.rd_output_offset;
  }

  /** @brief HAL Getter of signal RD_MODEL_SIZE from register RD_MODEL_SIZE
  *
  * Model configuration size in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline rd_model_size_get() {
    return regmap->rd_model_size.fields.rd_model_size;
  }

  /** @brief HAL Getter of signal RD_INPUT_SIZE from register RD_INPUT_SIZE
  *
  * Input data size in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline rd_input_size_get() {
    return regmap->rd_input_size.fields.rd_input_size;
  }

  /** @brief HAL Getter of signal RD_DATA_SIZE from register RD_DATA_SIZE
  *
  * Data inference needs in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline rd_data_size_get() {
    return regmap->rd_data_size.fields.rd_data_size;
  }

  /** @brief HAL Getter of signal IFMAP_FLATTENED from register RD_IFMAP_INFO
  *
  * Indicates whether the NN input feature map is flattened or not
  *
  * @return Register value
  */
  uint32_t inline ifmap_flattened_get() {
    return regmap->rd_ifmap_info.fields.ifmap_flattened;
  }

  /** @brief HAL Getter of signal IFMAP_PACK_LEVEL from register RD_IFMAP_INFO
  *
  * Packing level of NN input feature map
  *
  * @return Register value
  */
  uint32_t inline ifmap_pack_level_get() {
    return regmap->rd_ifmap_info.fields.ifmap_pack_level;
  }

  /** @brief HAL Getter of signal IFMAP_WIDTH from register RD_IFMAP_SIZE
  *
  * NN input feature map width minus 1
  *
  * @return Register value
  */
  uint32_t inline ifmap_width_get() {
    return regmap->rd_ifmap_size.fields.ifmap_width;
  }

  /** @brief HAL Getter of signal IFMAP_HEIGHT from register RD_IFMAP_SIZE
  *
  * NN input feature map height minus 1
  *
  * @return Register value
  */
  uint32_t inline ifmap_height_get() {
    return regmap->rd_ifmap_size.fields.ifmap_height;
  }

  /** @brief HAL Getter of signal IFMAP_DEPTH from register RD_IFMAP_SIZE
  *
  * NN input feature map depth minus 1
  *
  * @return Register value
  */
  uint32_t inline ifmap_depth_get() {
    return regmap->rd_ifmap_size.fields.ifmap_depth;
  }

  /** @brief HAL Getter of signal OFMAP_FLATTENED from register RD_OFMAP_INFO
  *
  * Indicates whether the NN output feature map is flattened or not
  *
  * @return Register value
  */
  uint32_t inline ofmap_flattened_get() {
    return regmap->rd_ofmap_info.fields.ofmap_flattened;
  }

  /** @brief HAL Getter of signal OFMAP_PACK_LEVEL from register RD_OFMAP_INFO
  *
  * Packing level of NN output feature map
  *
  * @return Register value
  */
  uint32_t inline ofmap_pack_level_get() {
    return regmap->rd_ofmap_info.fields.ofmap_pack_level;
  }

  /** @brief HAL Getter of signal OFMAP_WIDTH from register RD_OFMAP_SIZE
  *
  * NN output  feature map width minus 1
  *
  * @return Register value
  */
  uint32_t inline ofmap_width_get() {
    return regmap->rd_ofmap_size.fields.ofmap_width;
  }

  /** @brief HAL Getter of signal OFMAP_HEIGHT from register RD_OFMAP_SIZE
  *
  * NN output feature map height minus 1
  *
  * @return Register value
  */
  uint32_t inline ofmap_height_get() {
    return regmap->rd_ofmap_size.fields.ofmap_height;
  }

  /** @brief HAL Getter of signal OFMAP_DEPTH from register RD_OFMAP_SIZE
  *
  * NN output feature map depth minus 1
  *
  * @return Register value
  */
  uint32_t inline ofmap_depth_get() {
    return regmap->rd_ofmap_size.fields.ofmap_depth;
  }

  /** @brief HAL Getter of signal DATA_END_MADDR from register DATA_END_MADDR
  *
  * NN data end computed address in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline data_end_maddr_get() {
    return regmap->data_end_maddr.fields.data_end_maddr;
  }

  /** @brief HAL Getter of signal OFMAP_START_MADDR from register OFMAP_START_MADDR
  *
  * NN output data start memory address in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline ofmap_start_maddr_get() {
    return regmap->ofmap_start_maddr.fields.ofmap_start_maddr;
  }

  /** @brief HAL Getter of signal INFERENCE_COUNTER from register INFERENCE_COUNTER
  *
  * Inference counter incremented at each end of inference
  *
  * @return Register value
  */
  uint32_t inline inference_counter_get() {
    return regmap->inference_counter.fields.inference_counter;
  }

  /** @brief HAL Getter of signal MEM_CONFLICT_SOURCE from register MEM_CONFLICT_INFO
  *
  * Indicates the sources of the last memory conflict event
  *
  * @return Register value
  */
  uint32_t inline mem_conflict_source_get() {
    return regmap->mem_conflict_info.fields.mem_conflict_source;
  }

  /** @brief HAL Getter of signal MEM_CONFLICT_BUFFER from register MEM_CONFLICT_INFO
  *
  * Indicates in which buffer the last conflict occurred 
  *
  * @return Register value
  */
  uint32_t inline mem_conflict_buffer_get() {
    return regmap->mem_conflict_info.fields.mem_conflict_buffer;
  }

  /** @brief HAL Getter of signal XTEN_IFMAP_MADDR from register XTEN_IFMAP_MADDR
  *
  * Start address of input feature map for XTEN layers in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline xten_ifmap_maddr_get() {
    return regmap->xten_ifmap_maddr.fields.xten_ifmap_maddr;
  }

  /** @brief HAL Getter of signal XTEN_OFMAP_MADDR from register XTEN_OFMAP_MADDR
  *
  * Start address of output feature map for XTEN layers in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline xten_ofmap_maddr_get() {
    return regmap->xten_ofmap_maddr.fields.xten_ofmap_maddr;
  }

  /** @brief HAL Getter of signal XTEN_WEIGHT_MADDR from register XTEN_WEIGHT_MADDR
  *
  * Start address of weights list for XTEN layers in NNPA memory words
  *
  * @return Register value
  */
  uint32_t inline xten_weight_maddr_get() {
    return regmap->xten_weight_maddr.fields.xten_weight_maddr;
  }

  /** @brief HAL Getter of signal CONFIG_CHECK_CRC from register CONFIG_CHECK_CRC
  *
  * Result of the last NN configuration check CRC computation
  *
  * @return Register value
  */
  uint32_t inline config_check_crc_get() {
    return regmap->config_check_crc.fields.config_check_crc;
  }

  /** @brief HAL Trigger 'write set' of signal NNPA_MBIST_RUN from register NNPA_MBIST_CTRL
  *
  * Writing ‘1’ to bits k launches the memory BIST test for buffers groups k
  *
  * @param value Value to apply to signal (10 bits large)
  */
  void inline nnpa_mbist_run(uint32_t value) {
    regmap->nnpa_mbist_ctrl.word |= NNPA_MBIST_RUN_MASK & (value << NNPA_MBIST_RUN_OFFSET);
  }

  /** @brief HAL Getter of signal NNPA_MBIST_RUN from register NNPA_MBIST_CTRL
  *
  * Writing ‘1’ to bits k launches the memory BIST test for buffers groups k
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_run_get() {
    return regmap->nnpa_mbist_ctrl.fields.nnpa_mbist_run;
  }

  /** @brief HAL Getter of signal NNPA_MBIST_MODE from register NNPA_MBIST_CTRL
  *
  * Memory BIST mode.
  * 0: Use all ‘1’ and all ‘0’ pattern
  * 1: Use alternating ‘1’ and ‘0’ pattern
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_mode_get() {
    return regmap->nnpa_mbist_ctrl.fields.nnpa_mbist_mode;
  }

  /** @brief HAL Setter of signal NNPA_MBIST_MODE from register NNPA_MBIST_CTRL
  *
  * Memory BIST mode.
  * 0: Use all ‘1’ and all ‘0’ pattern
  * 1: Use alternating ‘1’ and ‘0’ pattern
  *
  * @param nnpa_mbist_mode Value to be set
  */
  void inline nnpa_mbist_mode_set(uint32_t nnpa_mbist_mode) {
    regmap->nnpa_mbist_ctrl.fields.nnpa_mbist_mode = nnpa_mbist_mode;
  }

  /** @brief HAL Getter of signal NNPA_MBIST_DONE from register NNPA_MBIST_STATUS
  *
  * Rises on memory BIST completion. Automatically cleared when the BIST test starts
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_done_get() {
    return regmap->nnpa_mbist_status.fields.nnpa_mbist_done;
  }

  /** @brief HAL Getter of signal NNPA_MBIST_ANY_FAIL from register NNPA_MBIST_STATUS
  *
  * Rises on any memory BIST failure. Automatically cleared when the BIST test starts
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_any_fail_get() {
    return regmap->nnpa_mbist_status.fields.nnpa_mbist_any_fail;
  }

  /** @brief HAL Getter of signal NNPA_MBIST_FAIL0 from register NNPA_MBIST_FAIL0
  *
  * BIST status of SRAM cuts 0 to 31
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_fail0_get() {
    return regmap->nnpa_mbist_fail0.fields.nnpa_mbist_fail0;
  }

  /** @brief HAL Getter of signal NNPA_MBIST_FAIL1 from register NNPA_MBIST_FAIL1
  *
  * BIST status of SRAM cuts 32 to 63
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_fail1_get() {
    return regmap->nnpa_mbist_fail1.fields.nnpa_mbist_fail1;
  }

  /** @brief HAL Getter of signal NNPA_MBIST_FAIL2 from register NNPA_MBIST_FAIL2
  *
  * BIST status of SRAM cuts 64 to 79
  *
  * @return Register value
  */
  uint32_t inline nnpa_mbist_fail2_get() {
    return regmap->nnpa_mbist_fail2.fields.nnpa_mbist_fail2;
  }

  /** @brief HAL Getter of signal EVT_DATA_LOADED from register NNPA_INT_STATUS
  *
  * Event latched once the input loader has received all expected input data
  *
  * @return Register value
  */
  uint32_t inline evt_data_loaded_get() {
    return regmap->nnpa_int_status.fields.evt_data_loaded;
  }

  /** @brief HAL Getter of signal EVT_CONFIG_CHECK_END from register NNPA_INT_STATUS
  *
  * Event latched at the end of NN configuration check procedure
  *
  * @return Register value
  */
  uint32_t inline evt_config_check_end_get() {
    return regmap->nnpa_int_status.fields.evt_config_check_end;
  }

  /** @brief HAL Getter of signal EVT_EXEC_START from register NNPA_INT_STATUS
  *
  * Event latched at the start of each NNPA execution
  *
  * @return Register value
  */
  uint32_t inline evt_exec_start_get() {
    return regmap->nnpa_int_status.fields.evt_exec_start;
  }

  /** @brief HAL Getter of signal EVT_EXEC_END from register NNPA_INT_STATUS
  *
  * Event latched at the end of each NNPA execution
  *
  * @return Register value
  */
  uint32_t inline evt_exec_end_get() {
    return regmap->nnpa_int_status.fields.evt_exec_end;
  }

  /** @brief HAL Getter of signal EVT_XTEN_PAUSE from register NNPA_INT_STATUS
  *
  * Event latched at the start of an XTEN layer, pausing the processing
  *
  * @return Register value
  */
  uint32_t inline evt_xten_pause_get() {
    return regmap->nnpa_int_status.fields.evt_xten_pause;
  }

  /** @brief HAL Getter of signal EVT_MEM_CONFLICT from register NNPA_INT_STATUS
  *
  * Event latched when a memory conflict is detected
  *
  * @return Register value
  */
  uint32_t inline evt_mem_conflict_get() {
    return regmap->nnpa_int_status.fields.evt_mem_conflict;
  }

  /** @brief HAL Getter of signal EVT_INPUT_LOADER_OVF from register NNPA_INT_STATUS
  *
  * Event latched when the INPUT LOADER received more data than expected
  *
  * @return Register value
  */
  uint32_t inline evt_input_loader_ovf_get() {
    return regmap->nnpa_int_status.fields.evt_input_loader_ovf;
  }

  /** @brief HAL Getter of signal EVT_ISP_SIZE_ERR from register NNPA_INT_STATUS
  *
  * Event latched when the image size received from the ISP does not match the expected input feature map size
  *
  * @return Register value
  */
  uint32_t inline evt_isp_size_err_get() {
    return regmap->nnpa_int_status.fields.evt_isp_size_err;
  }

  /** @brief HAL Getter of signal NNPA_INT_EN from register NNPA_INT_EN
  *
  * When an event has its corresponding enable bit set, it will contribute to IRQ generation of the block
  *
  * @return Register value
  */
  uint32_t inline nnpa_int_en_get() {
    return regmap->nnpa_int_en.fields.nnpa_int_en;
  }

  /** @brief HAL Setter of signal NNPA_INT_EN from register NNPA_INT_EN
  *
  * When an event has its corresponding enable bit set, it will contribute to IRQ generation of the block
  *
  * @param nnpa_int_en Value to be set
  */
  void inline nnpa_int_en_set(uint32_t nnpa_int_en) {
    regmap->nnpa_int_en.fields.nnpa_int_en = nnpa_int_en;
  }

  /** @brief HAL Trigger 'write pulse' of signal NNPA_INT_SET from register NNPA_INT_SET
  *
  * Emulate events by software by writing ‘1’ to corresponding positions
  *
  * @param value Value to apply to signal (8 bits large)
  */
  void inline nnpa_int_set(uint32_t value) {
    regmap->nnpa_int_set.word = NNPA_INT_SET_MASK & (value << NNPA_INT_SET_OFFSET);
  }

  /** @brief HAL Trigger 'write pulse' of signal NNPA_INT_CLEAR from register NNPA_INT_CLEAR
  *
  * Erase event flags of the NNPA_INT_STATUS by writing ‘1’ to corresponding positions
  *
  * @param value Value to apply to signal (8 bits large)
  */
  void inline nnpa_int_clear(uint32_t value) {
    regmap->nnpa_int_clear.word = NNPA_INT_CLEAR_MASK & (value << NNPA_INT_CLEAR_OFFSET);
  }

};
