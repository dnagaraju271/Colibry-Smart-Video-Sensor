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

#include "isp_mmap.h"


/// @brief HAL script-generated layer of isp register bank.
/// @note This code needs a C++ compiler to be used.
class IspHal {
 public:
  volatile s_isp_reg_type* regmap;

  IspHal() {
  }

  void inline set_periph_addr(uint32_t address) {
    regmap = (s_isp_reg_type*) address;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_BYPASS from register ISP_CTRL
  *
  * CROPPER bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_bypass_get() {
    return regmap->isp_ctrl.fields.isp_cropper_bypass;
  }

  /** @brief HAL Setter of signal ISP_CROPPER_BYPASS from register ISP_CTRL
  *
  * CROPPER bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @param isp_cropper_bypass Value to be set
  */
  void inline isp_cropper_bypass_set(uint32_t isp_cropper_bypass) {
    regmap->isp_ctrl.fields.isp_cropper_bypass = isp_cropper_bypass;
  }

  /** @brief HAL Getter of signal ISP_BAYER2RGB_BYPASS from register ISP_CTRL
  *
  * BAYER2RGB bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @return Register value
  */
  uint32_t inline isp_bayer2rgb_bypass_get() {
    return regmap->isp_ctrl.fields.isp_bayer2rgb_bypass;
  }

  /** @brief HAL Setter of signal ISP_BAYER2RGB_BYPASS from register ISP_CTRL
  *
  * BAYER2RGB bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @param isp_bayer2rgb_bypass Value to be set
  */
  void inline isp_bayer2rgb_bypass_set(uint32_t isp_bayer2rgb_bypass) {
    regmap->isp_ctrl.fields.isp_bayer2rgb_bypass = isp_bayer2rgb_bypass;
  }

  /** @brief HAL Getter of signal ISP_RGB2GRAY_BYPASS from register ISP_CTRL
  *
  * RGB2GRAY bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @return Register value
  */
  uint32_t inline isp_rgb2gray_bypass_get() {
    return regmap->isp_ctrl.fields.isp_rgb2gray_bypass;
  }

  /** @brief HAL Setter of signal ISP_RGB2GRAY_BYPASS from register ISP_CTRL
  *
  * RGB2GRAY bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @param isp_rgb2gray_bypass Value to be set
  */
  void inline isp_rgb2gray_bypass_set(uint32_t isp_rgb2gray_bypass) {
    regmap->isp_ctrl.fields.isp_rgb2gray_bypass = isp_rgb2gray_bypass;
  }

  /** @brief HAL Getter of signal ISP_SCALER_BYPASS from register ISP_CTRL
  *
  * SCALER bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_bypass_get() {
    return regmap->isp_ctrl.fields.isp_scaler_bypass;
  }

  /** @brief HAL Setter of signal ISP_SCALER_BYPASS from register ISP_CTRL
  *
  * SCALER bypass control.
  * 0 : functional mode
  * 1 : block bypassed and clock gated
  *
  * @param isp_scaler_bypass Value to be set
  */
  void inline isp_scaler_bypass_set(uint32_t isp_scaler_bypass) {
    regmap->isp_ctrl.fields.isp_scaler_bypass = isp_scaler_bypass;
  }

  /** @brief HAL Trigger 'write pulse' of signal ISP_SOFT_RESET from register ISP_CTRL
  *
  * Writing a 1 to this bit will generate a reset of the ISP and sub-blocks, except for the REGBANK configuration.
  *
  */
  void inline isp_soft_reset() {
    regmap->isp_ctrl.word |= ISP_SOFT_RESET_MASK;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_BUSY from register ISP_STATUS
  *
  * This bit indicates the busy status of the CROPPER.
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_busy_get() {
    return regmap->isp_status.fields.isp_cropper_busy;
  }

  /** @brief HAL Getter of signal ISP_BAYER2RGB_BUSY from register ISP_STATUS
  *
  * This bit indicates the busy status of the BAYER2RGB.
  *
  * @return Register value
  */
  uint32_t inline isp_bayer2rgb_busy_get() {
    return regmap->isp_status.fields.isp_bayer2rgb_busy;
  }

  /** @brief HAL Getter of signal ISP_RGB2GRAY_BUSY from register ISP_STATUS
  *
  * This bit indicates the busy status of the RGB2GRAY.
  *
  * @return Register value
  */
  uint32_t inline isp_rgb2gray_busy_get() {
    return regmap->isp_status.fields.isp_rgb2gray_busy;
  }

  /** @brief HAL Getter of signal ISP_SCALER_BUSY from register ISP_STATUS
  *
  * This bit indicates the busy status of the SCALER.
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_busy_get() {
    return regmap->isp_status.fields.isp_scaler_busy;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_PSB_INFO from register ISP_CROPPER_PSB_INFO
  *
  * PSB configuration word corresponding to last frame received by the CROPPER.
  * Bits [10:0]  : Input image width - 1
  * Bits [21:11] : Input image height - 1
  * Bits [23:22] : Input image number of channels - 1 
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_psb_info_get() {
    return regmap->isp_cropper_psb_info.fields.isp_cropper_psb_info;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_ROI_TL_X from register ISP_CROPPER_ROI_TL
  *
  * Configuration of the top-left corner of the output ROI. This 10-bit value defines its X coordinate.
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_roi_tl_x_get() {
    return regmap->isp_cropper_roi_tl.fields.isp_cropper_roi_tl_x;
  }

  /** @brief HAL Setter of signal ISP_CROPPER_ROI_TL_X from register ISP_CROPPER_ROI_TL
  *
  * Configuration of the top-left corner of the output ROI. This 10-bit value defines its X coordinate.
  *
  * @param isp_cropper_roi_tl_x Value to be set
  */
  void inline isp_cropper_roi_tl_x_set(uint32_t isp_cropper_roi_tl_x) {
    regmap->isp_cropper_roi_tl.fields.isp_cropper_roi_tl_x = isp_cropper_roi_tl_x;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_ROI_TL_Y from register ISP_CROPPER_ROI_TL
  *
  * Configuration of the top-left corner of the output ROI. This 10-bit value defines its Y coordinate.
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_roi_tl_y_get() {
    return regmap->isp_cropper_roi_tl.fields.isp_cropper_roi_tl_y;
  }

  /** @brief HAL Setter of signal ISP_CROPPER_ROI_TL_Y from register ISP_CROPPER_ROI_TL
  *
  * Configuration of the top-left corner of the output ROI. This 10-bit value defines its Y coordinate.
  *
  * @param isp_cropper_roi_tl_y Value to be set
  */
  void inline isp_cropper_roi_tl_y_set(uint32_t isp_cropper_roi_tl_y) {
    regmap->isp_cropper_roi_tl.fields.isp_cropper_roi_tl_y = isp_cropper_roi_tl_y;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_ROI_BR_X from register ISP_CROPPER_ROI_BR
  *
  * Configuration of the bottom-right corner of the output ROI. This 10-bit value defines its X coordinate.
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_roi_br_x_get() {
    return regmap->isp_cropper_roi_br.fields.isp_cropper_roi_br_x;
  }

  /** @brief HAL Setter of signal ISP_CROPPER_ROI_BR_X from register ISP_CROPPER_ROI_BR
  *
  * Configuration of the bottom-right corner of the output ROI. This 10-bit value defines its X coordinate.
  *
  * @param isp_cropper_roi_br_x Value to be set
  */
  void inline isp_cropper_roi_br_x_set(uint32_t isp_cropper_roi_br_x) {
    regmap->isp_cropper_roi_br.fields.isp_cropper_roi_br_x = isp_cropper_roi_br_x;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_ROI_BR_Y from register ISP_CROPPER_ROI_BR
  *
  * Configuration of the bottom-right corner of the output ROI. This 10-bit value defines its Y coordinate.
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_roi_br_y_get() {
    return regmap->isp_cropper_roi_br.fields.isp_cropper_roi_br_y;
  }

  /** @brief HAL Setter of signal ISP_CROPPER_ROI_BR_Y from register ISP_CROPPER_ROI_BR
  *
  * Configuration of the bottom-right corner of the output ROI. This 10-bit value defines its Y coordinate.
  *
  * @param isp_cropper_roi_br_y Value to be set
  */
  void inline isp_cropper_roi_br_y_set(uint32_t isp_cropper_roi_br_y) {
    regmap->isp_cropper_roi_br.fields.isp_cropper_roi_br_y = isp_cropper_roi_br_y;
  }

  /** @brief HAL Getter of signal ISP_B2RGB_PSB_INFO from register ISP_B2RGB_PSB_INFO
  *
  * PSB configuration word corresponding to last frame received by the BAYER2RGB.
  * Bits [10:0]  : Input image width - 1
  * Bits [21:11] : Input image height - 1
  * Bits [23:22] : Input image number of channels - 1 
  *
  * @return Register value
  */
  uint32_t inline isp_b2rgb_psb_info_get() {
    return regmap->isp_b2rgb_psb_info.fields.isp_b2rgb_psb_info;
  }

  /** @brief HAL Getter of signal ISP_B2RGB_RGGB_MODE from register ISP_B2RGB_RGGB_MODE
  *
  * Bayer color pattern mode received by the block. Shall be set accordingly to the camera output format.
  * 0 : BGGR pattern
  * 1 : RGGB pattern
  *
  * @return Register value
  */
  uint32_t inline isp_b2rgb_rggb_mode_get() {
    return regmap->isp_b2rgb_rggb_mode.fields.isp_b2rgb_rggb_mode;
  }

  /** @brief HAL Setter of signal ISP_B2RGB_RGGB_MODE from register ISP_B2RGB_RGGB_MODE
  *
  * Bayer color pattern mode received by the block. Shall be set accordingly to the camera output format.
  * 0 : BGGR pattern
  * 1 : RGGB pattern
  *
  * @param isp_b2rgb_rggb_mode Value to be set
  */
  void inline isp_b2rgb_rggb_mode_set(uint32_t isp_b2rgb_rggb_mode) {
    regmap->isp_b2rgb_rggb_mode.fields.isp_b2rgb_rggb_mode = isp_b2rgb_rggb_mode;
  }

  /** @brief HAL Getter of signal ISP_RGB2GRAY_PSB_INFO from register ISP_RGB2GRAY_PSB_INFO
  *
  * PSB configuration word corresponding to last frame received by the RGB2GRAY.
  * Bits [10:0]  : Input image width - 1
  * Bits [21:11] : Input image height - 1
  * Bits [23:22] : Input image number of channels - 1 
  *
  * @return Register value
  */
  uint32_t inline isp_rgb2gray_psb_info_get() {
    return regmap->isp_rgb2gray_psb_info.fields.isp_rgb2gray_psb_info;
  }

  /** @brief HAL Getter of signal ISP_SCALER_PSB_INFO from register ISP_SCALER_PSB_INFO
  *
  * PSB configuration word corresponding to last frame received by the SCALER.
  * Bits [10:0]  : Input image width - 1
  * Bits [21:11] : Input image height - 1
  * Bits [23:22] : Input image number of channels - 1 
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_psb_info_get() {
    return regmap->isp_scaler_psb_info.fields.isp_scaler_psb_info;
  }

  /** @brief HAL Getter of signal ISP_SCALER_SCALING_X from register ISP_SCALER_SCALING_CFG
  *
  * Scaling factor in X direction in fixed-point format 4.8 bits. Image width is divided by this value, so should be ≥ 1.0 (coded as 0x100).
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_scaling_x_get() {
    return regmap->isp_scaler_scaling_cfg.fields.isp_scaler_scaling_x;
  }

  /** @brief HAL Setter of signal ISP_SCALER_SCALING_X from register ISP_SCALER_SCALING_CFG
  *
  * Scaling factor in X direction in fixed-point format 4.8 bits. Image width is divided by this value, so should be ≥ 1.0 (coded as 0x100).
  *
  * @param isp_scaler_scaling_x Value to be set
  */
  void inline isp_scaler_scaling_x_set(uint32_t isp_scaler_scaling_x) {
    regmap->isp_scaler_scaling_cfg.fields.isp_scaler_scaling_x = isp_scaler_scaling_x;
  }

  /** @brief HAL Getter of signal ISP_SCALER_SCALING_Y from register ISP_SCALER_SCALING_CFG
  *
  * Scaling factor in Y direction in fixed-point format 4.8 bits. Image height is divided by this value, so should be ≥ 1.0 (coded as 0x100).
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_scaling_y_get() {
    return regmap->isp_scaler_scaling_cfg.fields.isp_scaler_scaling_y;
  }

  /** @brief HAL Setter of signal ISP_SCALER_SCALING_Y from register ISP_SCALER_SCALING_CFG
  *
  * Scaling factor in Y direction in fixed-point format 4.8 bits. Image height is divided by this value, so should be ≥ 1.0 (coded as 0x100).
  *
  * @param isp_scaler_scaling_y Value to be set
  */
  void inline isp_scaler_scaling_y_set(uint32_t isp_scaler_scaling_y) {
    regmap->isp_scaler_scaling_cfg.fields.isp_scaler_scaling_y = isp_scaler_scaling_y;
  }

  /** @brief HAL Getter of signal ISP_SCALER_SIZE_OUT_X from register ISP_SCALER_SIZE_OUT
  *
  * SCALER exact output image width.
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_size_out_x_get() {
    return regmap->isp_scaler_size_out.fields.isp_scaler_size_out_x;
  }

  /** @brief HAL Setter of signal ISP_SCALER_SIZE_OUT_X from register ISP_SCALER_SIZE_OUT
  *
  * SCALER exact output image width.
  *
  * @param isp_scaler_size_out_x Value to be set
  */
  void inline isp_scaler_size_out_x_set(uint32_t isp_scaler_size_out_x) {
    regmap->isp_scaler_size_out.fields.isp_scaler_size_out_x = isp_scaler_size_out_x;
  }

  /** @brief HAL Getter of signal ISP_SCALER_SIZE_OUT_Y from register ISP_SCALER_SIZE_OUT
  *
  * SCALER exact output image height.
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_size_out_y_get() {
    return regmap->isp_scaler_size_out.fields.isp_scaler_size_out_y;
  }

  /** @brief HAL Setter of signal ISP_SCALER_SIZE_OUT_Y from register ISP_SCALER_SIZE_OUT
  *
  * SCALER exact output image height.
  *
  * @param isp_scaler_size_out_y Value to be set
  */
  void inline isp_scaler_size_out_y_set(uint32_t isp_scaler_size_out_y) {
    regmap->isp_scaler_size_out.fields.isp_scaler_size_out_y = isp_scaler_size_out_y;
  }

  /** @brief HAL Getter of signal ISP_CROPPER_PSB_ERR from register ISP_INT_STATUS
  *
  * PSB error detection in CROPPER event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline isp_cropper_psb_err_get() {
    return regmap->isp_int_status.fields.isp_cropper_psb_err;
  }

  /** @brief HAL Getter of signal ISP_B2RGB_PSB_ERR from register ISP_INT_STATUS
  *
  * PSB error detection in BAYER2RGB event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline isp_b2rgb_psb_err_get() {
    return regmap->isp_int_status.fields.isp_b2rgb_psb_err;
  }

  /** @brief HAL Getter of signal ISP_RGB2GRAY_PSB_ERR from register ISP_INT_STATUS
  *
  * PSB error detection in RGB2GARY event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline isp_rgb2gray_psb_err_get() {
    return regmap->isp_int_status.fields.isp_rgb2gray_psb_err;
  }

  /** @brief HAL Getter of signal ISP_SCALER_PSB_ERR from register ISP_INT_STATUS
  *
  * PSB error detection in SCALER event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_psb_err_get() {
    return regmap->isp_int_status.fields.isp_scaler_psb_err;
  }

  /** @brief HAL Getter of signal ISP_SCALER_PARAM_ERR from register ISP_INT_STATUS
  *
  * Scaling parameters errors in SCALER event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline isp_scaler_param_err_get() {
    return regmap->isp_int_status.fields.isp_scaler_param_err;
  }

  /** @brief HAL Getter of signal ISP_FRAME_START from register ISP_INT_STATUS
  *
  * ISP frame start detection event. Will rise when the event occurs.
  *
  * @return Register value
  */
  uint32_t inline isp_frame_start_get() {
    return regmap->isp_int_status.fields.isp_frame_start;
  }

  /** @brief HAL Getter of signal ISP_INT_EN from register ISP_INT_EN
  *
  * Interrupt enable. Each event latched in the ISP_INT_STATUS register will generate an interrupt if the corresponding bit in this field is set.
  *
  * @return Register value
  */
  uint32_t inline isp_int_en_get() {
    return regmap->isp_int_en.fields.isp_int_en;
  }

  /** @brief HAL Setter of signal ISP_INT_EN from register ISP_INT_EN
  *
  * Interrupt enable. Each event latched in the ISP_INT_STATUS register will generate an interrupt if the corresponding bit in this field is set.
  *
  * @param isp_int_en Value to be set
  */
  void inline isp_int_en_set(uint32_t isp_int_en) {
    regmap->isp_int_en.fields.isp_int_en = isp_int_en;
  }

  /** @brief HAL Trigger 'write pulse' of signal ISP_INT_SET from register ISP_INT_SET
  *
  * Interrupt set. For each bit written to 1, emulates an event by latching a 1 in the corresponding bits of the ISP_INT_STATUS register.
  *
  * @param value Value to apply to signal (6 bits large)
  */
  void inline isp_int_set(uint32_t value) {
    regmap->isp_int_set.word = ISP_INT_SET_MASK & (value << ISP_INT_SET_OFFSET);
  }

  /** @brief HAL Trigger 'write pulse' of signal ISP_INT_CLEAR from register ISP_INT_CLEAR
  *
  * Interrupt clear. For each bit written to 1,  clears the corresponding bits of the ISP_INT_STATUS register.
  *
  * @param value Value to apply to signal (6 bits large)
  */
  void inline isp_int_clear(uint32_t value) {
    regmap->isp_int_clear.word = ISP_INT_CLEAR_MASK & (value << ISP_INT_CLEAR_OFFSET);
  }

  /** @brief HAL Trigger 'write set' of signal ISP_MBIST_RUN from register ISP_MBIST_CTRL
  *
  * Launch memory BIST test
  *
  */
  void inline isp_mbist_run() {
    regmap->isp_mbist_ctrl.word = ISP_MBIST_RUN_MASK;
  }

  /** @brief HAL Getter of signal ISP_MBIST_RUN from register ISP_MBIST_CTRL
  *
  * Launch memory BIST test
  *
  * @return Register value
  */
  uint32_t inline isp_mbist_run_get() {
    return regmap->isp_mbist_ctrl.fields.isp_mbist_run;
  }

  /** @brief HAL Getter of signal ISP_MBIST_DONE from register ISP_MBIST_STATUS
  *
  * Set by HW on memory BIST completion. Cleared by HW when BIST starts
  *
  * @return Register value
  */
  uint32_t inline isp_mbist_done_get() {
    return regmap->isp_mbist_status.fields.isp_mbist_done;
  }

  /** @brief HAL Getter of signal ISP_MBIST_ANY_FAIL from register ISP_MBIST_STATUS
  *
  * Set by HW on any memory BIST failure. Cleared by HW when BIST starts
  *
  * @return Register value
  */
  uint32_t inline isp_mbist_any_fail_get() {
    return regmap->isp_mbist_status.fields.isp_mbist_any_fail;
  }

  /** @brief HAL Getter of signal ISP_MBIST_FAIL from register ISP_MBIST_FAIL
  *
  * BIST status of SRAM cuts 0 to 1
  *
  * @return Register value
  */
  uint32_t inline isp_mbist_fail_get() {
    return regmap->isp_mbist_fail.fields.isp_mbist_fail;
  }

};
