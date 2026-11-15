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

#include "camif_mmap.h"


/// @brief HAL script-generated layer of camif register bank.
/// @note This code needs a C++ compiler to be used.
class CamifHal {
 public:
  volatile s_camif_reg_type* regmap;

  CamifHal() {
  }

  void inline set_periph_addr(uint32_t address) {
    regmap = (s_camif_reg_type*) address;
  }

  /** @brief HAL Getter of signal DATA_IN_MODE from register CAMIF_PROTOCOL_CFG
  *
  * Camera input data mode.
  * 0: RAW
  * 1: RGBSPLIT
  * 2: RGB555
  * 3: RGB565
  * 4: RGB444x
  * 5: RGBx444
  *
  * @return Register value
  */
  uint32_t inline data_in_mode_get() {
    return regmap->camif_protocol_cfg.fields.data_in_mode;
  }

  /** @brief HAL Setter of signal DATA_IN_MODE from register CAMIF_PROTOCOL_CFG
  *
  * Camera input data mode.
  * 0: RAW
  * 1: RGBSPLIT
  * 2: RGB555
  * 3: RGB565
  * 4: RGB444x
  * 5: RGBx444
  *
  * @param data_in_mode Value to be set
  */
  void inline data_in_mode_set(uint32_t data_in_mode) {
    regmap->camif_protocol_cfg.fields.data_in_mode = data_in_mode;
  }

  /** @brief HAL Getter of signal PIXEL_IN_WIDTH from register CAMIF_PROTOCOL_CFG
  *
  * Number of bits on which pixels channels are coded in RAW and RGBSPLIT modes. Max value is 14.
  *
  * @return Register value
  */
  uint32_t inline pixel_in_width_get() {
    return regmap->camif_protocol_cfg.fields.pixel_in_width;
  }

  /** @brief HAL Setter of signal PIXEL_IN_WIDTH from register CAMIF_PROTOCOL_CFG
  *
  * Number of bits on which pixels channels are coded in RAW and RGBSPLIT modes. Max value is 14.
  *
  * @param pixel_in_width Value to be set
  */
  void inline pixel_in_width_set(uint32_t pixel_in_width) {
    regmap->camif_protocol_cfg.fields.pixel_in_width = pixel_in_width;
  }

  /** @brief HAL Getter of signal RGBSPLIT_R_FIRST from register CAMIF_PROTOCOL_CFG
  *
  * First component received in RGBSPLIT mode definition.
  * 0: Blue-Green-Red order
  * 1: Red-Green-Blue order
  *
  * @return Register value
  */
  uint32_t inline rgbsplit_r_first_get() {
    return regmap->camif_protocol_cfg.fields.rgbsplit_r_first;
  }

  /** @brief HAL Setter of signal RGBSPLIT_R_FIRST from register CAMIF_PROTOCOL_CFG
  *
  * First component received in RGBSPLIT mode definition.
  * 0: Blue-Green-Red order
  * 1: Red-Green-Blue order
  *
  * @param rgbsplit_r_first Value to be set
  */
  void inline rgbsplit_r_first_set(uint32_t rgbsplit_r_first) {
    regmap->camif_protocol_cfg.fields.rgbsplit_r_first = rgbsplit_r_first;
  }

  /** @brief HAL Getter of signal NIBBLE_SPLIT_EN from register CAMIF_PROTOCOL_CFG
  *
  * Enable nibble mode transmission of pixel channels.
  *
  * @return Register value
  */
  uint32_t inline nibble_split_en_get() {
    return regmap->camif_protocol_cfg.fields.nibble_split_en;
  }

  /** @brief HAL Setter of signal NIBBLE_SPLIT_EN from register CAMIF_PROTOCOL_CFG
  *
  * Enable nibble mode transmission of pixel channels.
  *
  * @param nibble_split_en Value to be set
  */
  void inline nibble_split_en_set(uint32_t nibble_split_en) {
    regmap->camif_protocol_cfg.fields.nibble_split_en = nibble_split_en;
  }

  /** @brief HAL Getter of signal NIBBLE_ENDIANNESS from register CAMIF_PROTOCOL_CFG
  *
  * Nibble mode endianness definition.
  * 0: First nibble corresponds to the MSBs of the pixel channel value
  * 1: First nibble corresponds to the LSBs of the pixel channel value
  *
  * @return Register value
  */
  uint32_t inline nibble_endianness_get() {
    return regmap->camif_protocol_cfg.fields.nibble_endianness;
  }

  /** @brief HAL Setter of signal NIBBLE_ENDIANNESS from register CAMIF_PROTOCOL_CFG
  *
  * Nibble mode endianness definition.
  * 0: First nibble corresponds to the MSBs of the pixel channel value
  * 1: First nibble corresponds to the LSBs of the pixel channel value
  *
  * @param nibble_endianness Value to be set
  */
  void inline nibble_endianness_set(uint32_t nibble_endianness) {
    regmap->camif_protocol_cfg.fields.nibble_endianness = nibble_endianness;
  }

  /** @brief HAL Getter of signal PCLK_SAMPLING_CFG from register CAMIF_PROTOCOL_CFG
  *
  * Sampling edge selection for CAM_PCLK clock.
  * 0: Signals sampled on rising edge
  * 1: Signals sampled on falling edge
  *
  * @return Register value
  */
  uint32_t inline pclk_sampling_cfg_get() {
    return regmap->camif_protocol_cfg.fields.pclk_sampling_cfg;
  }

  /** @brief HAL Setter of signal PCLK_SAMPLING_CFG from register CAMIF_PROTOCOL_CFG
  *
  * Sampling edge selection for CAM_PCLK clock.
  * 0: Signals sampled on rising edge
  * 1: Signals sampled on falling edge
  *
  * @param pclk_sampling_cfg Value to be set
  */
  void inline pclk_sampling_cfg_set(uint32_t pclk_sampling_cfg) {
    regmap->camif_protocol_cfg.fields.pclk_sampling_cfg = pclk_sampling_cfg;
  }

  /** @brief HAL Getter of signal CONV_DATA_SHIFT from register CAMIF_PROTOCOL_CFG
  *
  * Left shift value applied to the input pixel channel.
  *
  * @return Register value
  */
  uint32_t inline conv_data_shift_get() {
    return regmap->camif_protocol_cfg.fields.conv_data_shift;
  }

  /** @brief HAL Setter of signal CONV_DATA_SHIFT from register CAMIF_PROTOCOL_CFG
  *
  * Left shift value applied to the input pixel channel.
  *
  * @param conv_data_shift Value to be set
  */
  void inline conv_data_shift_set(uint32_t conv_data_shift) {
    regmap->camif_protocol_cfg.fields.conv_data_shift = conv_data_shift;
  }

  /** @brief HAL Getter of signal RGBX_MSB_ALIGNED from register CAMIF_PROTOCOL_CFG
  *
  * Indicates if each channel data must be MSB aligned or LSB aligned when mapped to a full byte, in RGBxxx mode.
  *
  * @return Register value
  */
  uint32_t inline rgbx_msb_aligned_get() {
    return regmap->camif_protocol_cfg.fields.rgbx_msb_aligned;
  }

  /** @brief HAL Setter of signal RGBX_MSB_ALIGNED from register CAMIF_PROTOCOL_CFG
  *
  * Indicates if each channel data must be MSB aligned or LSB aligned when mapped to a full byte, in RGBxxx mode.
  *
  * @param rgbx_msb_aligned Value to be set
  */
  void inline rgbx_msb_aligned_set(uint32_t rgbx_msb_aligned) {
    regmap->camif_protocol_cfg.fields.rgbx_msb_aligned = rgbx_msb_aligned;
  }

  /** @brief HAL Getter of signal IMG_WIDTH from register CAMIF_IMG_SIZE_CFG
  *
  * Expected input image width (in pixels).
  *
  * @return Register value
  */
  uint32_t inline img_width_get() {
    return regmap->camif_img_size_cfg.fields.img_width;
  }

  /** @brief HAL Setter of signal IMG_WIDTH from register CAMIF_IMG_SIZE_CFG
  *
  * Expected input image width (in pixels).
  *
  * @param img_width Value to be set
  */
  void inline img_width_set(uint32_t img_width) {
    regmap->camif_img_size_cfg.fields.img_width = img_width;
  }

  /** @brief HAL Getter of signal IMG_HEIGHT from register CAMIF_IMG_SIZE_CFG
  *
  * Expected input image height (in rows of pixels).
  *
  * @return Register value
  */
  uint32_t inline img_height_get() {
    return regmap->camif_img_size_cfg.fields.img_height;
  }

  /** @brief HAL Setter of signal IMG_HEIGHT from register CAMIF_IMG_SIZE_CFG
  *
  * Expected input image height (in rows of pixels).
  *
  * @param img_height Value to be set
  */
  void inline img_height_set(uint32_t img_height) {
    regmap->camif_img_size_cfg.fields.img_height = img_height;
  }

  /** @brief HAL Getter of signal IMG_COL_CNT from register CAMIF_IMG_CURR_PTR
  *
  * Current image column counter value.
  *
  * @return Register value
  */
  uint32_t inline img_col_cnt_get() {
    return regmap->camif_img_curr_ptr.fields.img_col_cnt;
  }

  /** @brief HAL Getter of signal IMG_ROW_CNT from register CAMIF_IMG_CURR_PTR
  *
  * Current image row counter value.
  *
  * @return Register value
  */
  uint32_t inline img_row_cnt_get() {
    return regmap->camif_img_curr_ptr.fields.img_row_cnt;
  }

  /** @brief HAL Getter of signal IMG_COUNT from register CAMIF_IMG_COUNT
  *
  * Current number of images received from the camera.
  *
  * @return Register value
  */
  uint32_t inline img_count_get() {
    return regmap->camif_img_count.fields.img_count;
  }

  /** @brief HAL Trigger 'write pulse' of signal IMG_COUNT_RST from register CAMIF_IMG_COUNT_RST
  *
  * Writing a 1 to this bit will clear the image counter provided in IMG_COUNT.
  *
  */
  void inline img_count_rst() {
    regmap->camif_img_count_rst.word = IMG_COUNT_RST_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal PATGEN_FRAME_START from register CAMIF_PATGEN_CONTROL
  *
  * Writing a 1 to this bit will start a frame transmission from the pattern generator.
  *
  */
  void inline patgen_frame_start() {
    regmap->camif_patgen_control.word = PATGEN_FRAME_START_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal PATGEN_LFSR_RST from register CAMIF_PATGEN_CONTROL
  *
  * Writing a 1 to this bit will reset the pattern generator LFSR.
  *
  */
  void inline patgen_lfsr_rst() {
    regmap->camif_patgen_control.word = PATGEN_LFSR_RST_MASK;
  }

  /** @brief HAL Getter of signal PATGEN_DATA_MODE from register CAMIF_PATGEN_CFG
  *
  * Patten generator data mode.
  * 0: Generate pseudo-random data
  * 1: Generate incrementing data
  *
  * @return Register value
  */
  uint32_t inline patgen_data_mode_get() {
    return regmap->camif_patgen_cfg.fields.patgen_data_mode;
  }

  /** @brief HAL Setter of signal PATGEN_DATA_MODE from register CAMIF_PATGEN_CFG
  *
  * Patten generator data mode.
  * 0: Generate pseudo-random data
  * 1: Generate incrementing data
  *
  * @param patgen_data_mode Value to be set
  */
  void inline patgen_data_mode_set(uint32_t patgen_data_mode) {
    regmap->camif_patgen_cfg.fields.patgen_data_mode = patgen_data_mode;
  }

  /** @brief HAL Getter of signal PATGEN_MUX_SEL from register CAMIF_PATGEN_CFG
  *
  * When ‘1’, the pattern generator data path is activated while the functional path is disabled.
  *
  * @return Register value
  */
  uint32_t inline patgen_mux_sel_get() {
    return regmap->camif_patgen_cfg.fields.patgen_mux_sel;
  }

  /** @brief HAL Setter of signal PATGEN_MUX_SEL from register CAMIF_PATGEN_CFG
  *
  * When ‘1’, the pattern generator data path is activated while the functional path is disabled.
  *
  * @param patgen_mux_sel Value to be set
  */
  void inline patgen_mux_sel_set(uint32_t patgen_mux_sel) {
    regmap->camif_patgen_cfg.fields.patgen_mux_sel = patgen_mux_sel;
  }

  /** @brief HAL Getter of signal PATGEN_IMG_NCHAN from register CAMIF_PATGEN_CFG
  *
  * Number of channels used by the pattern generator.
  *
  * @return Register value
  */
  uint32_t inline patgen_img_nchan_get() {
    return regmap->camif_patgen_cfg.fields.patgen_img_nchan;
  }

  /** @brief HAL Setter of signal PATGEN_IMG_NCHAN from register CAMIF_PATGEN_CFG
  *
  * Number of channels used by the pattern generator.
  *
  * @param patgen_img_nchan Value to be set
  */
  void inline patgen_img_nchan_set(uint32_t patgen_img_nchan) {
    regmap->camif_patgen_cfg.fields.patgen_img_nchan = patgen_img_nchan;
  }

  /** @brief HAL Getter of signal PATGEN_SLOW_RATE from register CAMIF_PATGEN_CFG
  *
  * Number of empty cycles inserted after a valid cycle. Max value is 255.
  *
  * @return Register value
  */
  uint32_t inline patgen_slow_rate_get() {
    return regmap->camif_patgen_cfg.fields.patgen_slow_rate;
  }

  /** @brief HAL Setter of signal PATGEN_SLOW_RATE from register CAMIF_PATGEN_CFG
  *
  * Number of empty cycles inserted after a valid cycle. Max value is 255.
  *
  * @param patgen_slow_rate Value to be set
  */
  void inline patgen_slow_rate_set(uint32_t patgen_slow_rate) {
    regmap->camif_patgen_cfg.fields.patgen_slow_rate = patgen_slow_rate;
  }

  /** @brief HAL Getter of signal LFSR_AUTO_RESTART from register CAMIF_PATGEN_CFG
  *
  * When ‘1’, LFSR state is reset at each frame start
  *
  * @return Register value
  */
  uint32_t inline lfsr_auto_restart_get() {
    return regmap->camif_patgen_cfg.fields.lfsr_auto_restart;
  }

  /** @brief HAL Setter of signal LFSR_AUTO_RESTART from register CAMIF_PATGEN_CFG
  *
  * When ‘1’, LFSR state is reset at each frame start
  *
  * @param lfsr_auto_restart Value to be set
  */
  void inline lfsr_auto_restart_set(uint32_t lfsr_auto_restart) {
    regmap->camif_patgen_cfg.fields.lfsr_auto_restart = lfsr_auto_restart;
  }

  /** @brief HAL Getter of signal PATGEN_RAMP_INIT0 from register CAMIF_PATGEN_RAMP_INIT
  *
  * Initial value for first component of generated pixels.
  *
  * @return Register value
  */
  uint32_t inline patgen_ramp_init0_get() {
    return regmap->camif_patgen_ramp_init.fields.patgen_ramp_init0;
  }

  /** @brief HAL Setter of signal PATGEN_RAMP_INIT0 from register CAMIF_PATGEN_RAMP_INIT
  *
  * Initial value for first component of generated pixels.
  *
  * @param patgen_ramp_init0 Value to be set
  */
  void inline patgen_ramp_init0_set(uint32_t patgen_ramp_init0) {
    regmap->camif_patgen_ramp_init.fields.patgen_ramp_init0 = patgen_ramp_init0;
  }

  /** @brief HAL Getter of signal PATGEN_RAMP_INIT1 from register CAMIF_PATGEN_RAMP_INIT
  *
  * Initial value for second component of generated pixels.
  *
  * @return Register value
  */
  uint32_t inline patgen_ramp_init1_get() {
    return regmap->camif_patgen_ramp_init.fields.patgen_ramp_init1;
  }

  /** @brief HAL Setter of signal PATGEN_RAMP_INIT1 from register CAMIF_PATGEN_RAMP_INIT
  *
  * Initial value for second component of generated pixels.
  *
  * @param patgen_ramp_init1 Value to be set
  */
  void inline patgen_ramp_init1_set(uint32_t patgen_ramp_init1) {
    regmap->camif_patgen_ramp_init.fields.patgen_ramp_init1 = patgen_ramp_init1;
  }

  /** @brief HAL Getter of signal PATGEN_RAMP_INIT2 from register CAMIF_PATGEN_RAMP_INIT
  *
  * Initial value for third component of generated pixels.
  *
  * @return Register value
  */
  uint32_t inline patgen_ramp_init2_get() {
    return regmap->camif_patgen_ramp_init.fields.patgen_ramp_init2;
  }

  /** @brief HAL Setter of signal PATGEN_RAMP_INIT2 from register CAMIF_PATGEN_RAMP_INIT
  *
  * Initial value for third component of generated pixels.
  *
  * @param patgen_ramp_init2 Value to be set
  */
  void inline patgen_ramp_init2_set(uint32_t patgen_ramp_init2) {
    regmap->camif_patgen_ramp_init.fields.patgen_ramp_init2 = patgen_ramp_init2;
  }

  /** @brief HAL Getter of signal PATGEN_BUSY from register CAMIF_PATGEN_STATUS
  *
  * Indicates when the pattern generator is sending a frame.
  *
  * @return Register value
  */
  uint32_t inline patgen_busy_get() {
    return regmap->camif_patgen_status.fields.patgen_busy;
  }

  /** @brief HAL Getter of signal CDC_FIFO_BYPASS from register CAMIF_CDC_FIFO_BYPASS
  *
  * CDC FIFO bypass control.
  * 0: FIFO utilized
  * 1: FIFO bypassed
  *
  * @return Register value
  */
  uint32_t inline cdc_fifo_bypass_get() {
    return regmap->camif_cdc_fifo_bypass.fields.cdc_fifo_bypass;
  }

  /** @brief HAL Setter of signal CDC_FIFO_BYPASS from register CAMIF_CDC_FIFO_BYPASS
  *
  * CDC FIFO bypass control.
  * 0: FIFO utilized
  * 1: FIFO bypassed
  *
  * @param cdc_fifo_bypass Value to be set
  */
  void inline cdc_fifo_bypass_set(uint32_t cdc_fifo_bypass) {
    regmap->camif_cdc_fifo_bypass.fields.cdc_fifo_bypass = cdc_fifo_bypass;
  }

  /** @brief HAL Getter of signal CAM_DATA from register CAMIF_CAM_DATA
  *
  * Direct data from Data FIFO.
  *
  * @return Register value
  */
  uint32_t inline cam_data_get() {
    return regmap->camif_cam_data.fields.cam_data;
  }

  /** @brief HAL Getter of signal DATA_FIFO_WORD_NB from register CAMIF_DATA_FIFO_STATUS
  *
  * Current number of words stored in the Data FIFO.
  *
  * @return Register value
  */
  uint32_t inline data_fifo_word_nb_get() {
    return regmap->camif_data_fifo_status.fields.data_fifo_word_nb;
  }

  /** @brief HAL Getter of signal DATA_FIFO_THR from register CAMIF_DATA_FIFO_THR
  *
  * Threshold above which EVT_DATA_FIFO_RDY event is generated.
  *
  * @return Register value
  */
  uint32_t inline data_fifo_thr_get() {
    return regmap->camif_data_fifo_thr.fields.data_fifo_thr;
  }

  /** @brief HAL Setter of signal DATA_FIFO_THR from register CAMIF_DATA_FIFO_THR
  *
  * Threshold above which EVT_DATA_FIFO_RDY event is generated.
  *
  * @param data_fifo_thr Value to be set
  */
  void inline data_fifo_thr_set(uint32_t data_fifo_thr) {
    regmap->camif_data_fifo_thr.fields.data_fifo_thr = data_fifo_thr;
  }

  /** @brief HAL Getter of signal EVT_IMG_OVERFLOW from register CAMIF_INT_STATUS
  *
  * Input image overflow event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline evt_img_overflow_get() {
    return regmap->camif_int_status.fields.evt_img_overflow;
  }

  /** @brief HAL Getter of signal EVT_IMG_UNDERFLOW from register CAMIF_INT_STATUS
  *
  * Input image underflow event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline evt_img_underflow_get() {
    return regmap->camif_int_status.fields.evt_img_underflow;
  }

  /** @brief HAL Getter of signal EVT_IMG_START from register CAMIF_INT_STATUS
  *
  * Input image start event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline evt_img_start_get() {
    return regmap->camif_int_status.fields.evt_img_start;
  }

  /** @brief HAL Getter of signal EVT_IMG_END from register CAMIF_INT_STATUS
  *
  * Input image end event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline evt_img_end_get() {
    return regmap->camif_int_status.fields.evt_img_end;
  }

  /** @brief HAL Getter of signal EVT_DATA_FIFO_RDY from register CAMIF_INT_STATUS
  *
  * Data FIFO ready event. Will rise each time a word is stored while the threshold is reached.
  *
  * @return Register value
  */
  uint32_t inline evt_data_fifo_rdy_get() {
    return regmap->camif_int_status.fields.evt_data_fifo_rdy;
  }

  /** @brief HAL Getter of signal EVT_DATA_FIFO_OVF from register CAMIF_INT_STATUS
  *
  * Data FIFO overflow event. Will rise when an overflow occurs.
  *
  * @return Register value
  */
  uint32_t inline evt_data_fifo_ovf_get() {
    return regmap->camif_int_status.fields.evt_data_fifo_ovf;
  }

  /** @brief HAL Getter of signal CAMIF_INT_EN from register CAMIF_INT_EN
  *
  * When an event has its corresponding enable bit set, it will contribute to IRQ generation of the block.
  *
  * @return Register value
  */
  uint32_t inline camif_int_en_get() {
    return regmap->camif_int_en.fields.camif_int_en;
  }

  /** @brief HAL Setter of signal CAMIF_INT_EN from register CAMIF_INT_EN
  *
  * When an event has its corresponding enable bit set, it will contribute to IRQ generation of the block.
  *
  * @param camif_int_en Value to be set
  */
  void inline camif_int_en_set(uint32_t camif_int_en) {
    regmap->camif_int_en.fields.camif_int_en = camif_int_en;
  }

  /** @brief HAL Trigger 'write pulse' of signal CAMIF_INT_SET from register CAMIF_INT_SET
  *
  * Emulate events by software by writing ‘1’ to corresponding positions.
  *
  * @param value Value to apply to signal (6 bits large)
  */
  void inline camif_int_set(uint32_t value) {
    regmap->camif_int_set.word = CAMIF_INT_SET_MASK & (value << CAMIF_INT_SET_OFFSET);
  }

  /** @brief HAL Trigger 'write pulse' of signal CAMIF_INT_CLEAR from register CAMIF_INT_CLEAR
  *
  * Erase event flags of the CAMIF_INT_STATUS by writing ‘1’ to corresponding positions.
  *
  * @param value Value to apply to signal (6 bits large)
  */
  void inline camif_int_clear(uint32_t value) {
    regmap->camif_int_clear.word = CAMIF_INT_CLEAR_MASK & (value << CAMIF_INT_CLEAR_OFFSET);
  }

  /** @brief HAL Getter of signal CAMIF_REV_MINOR from register CAMIF_VERSION
  *
  * CAMIF hardware revision minor
  *
  * @return Register value
  */
  uint32_t inline camif_rev_minor_get() {
    return regmap->camif_version.fields.camif_rev_minor;
  }

  /** @brief HAL Getter of signal CAMIF_REV_MAJOR from register CAMIF_VERSION
  *
  * CAMIF hardware revision major
  *
  * @return Register value
  */
  uint32_t inline camif_rev_major_get() {
    return regmap->camif_version.fields.camif_rev_major;
  }

};
