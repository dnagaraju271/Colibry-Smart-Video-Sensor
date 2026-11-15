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

#include "dma_mmap.h"


/// @brief HAL script-generated layer of dma register bank.
/// @note This code needs a C++ compiler to be used.
class DmaHal {
 public:
  volatile s_dma_reg_type* regmap;

  DmaHal() {
  }

  void inline set_periph_addr(uint32_t address) {
    regmap = (s_dma_reg_type*) address;
  }

  /** @brief HAL Getter of signal DMA_VERSION_MINOR from register DMA_VERSION
  *
  * DMA version minor
  *
  * @return Register value
  */
  uint32_t inline dma_version_minor_get() {
    return regmap->dma_version.fields.dma_version_minor;
  }

  /** @brief HAL Getter of signal DMA_VERSION_MAJOR from register DMA_VERSION
  *
  * DMA version major
  *
  * @return Register value
  */
  uint32_t inline dma_version_major_get() {
    return regmap->dma_version.fields.dma_version_major;
  }

  /** @brief HAL Getter of signal DMA_EVT_DONE from register DMA_INT_STATUS
  *
  * Event latched on transfer done, per channel
  *
  * @return Register value
  */
  uint32_t inline dma_evt_done_get() {
    return regmap->dma_int_status.fields.dma_evt_done;
  }

  /** @brief HAL Getter of signal DMA_EVT_READ_ERROR from register DMA_INT_STATUS
  *
  * Event latched on read transfer error, per channel
  *
  * @return Register value
  */
  uint32_t inline dma_evt_read_error_get() {
    return regmap->dma_int_status.fields.dma_evt_read_error;
  }

  /** @brief HAL Getter of signal DMA_EVT_WRITE_ERROR from register DMA_INT_STATUS
  *
  * Event latched on write transfer error, per channel
  *
  * @return Register value
  */
  uint32_t inline dma_evt_write_error_get() {
    return regmap->dma_int_status.fields.dma_evt_write_error;
  }

  /** @brief HAL Getter of signal DMA_EVT_THRESHOLD from register DMA_INT_STATUS
  *
  * Event latched when transfer threshold reached, per channel
  *
  * @return Register value
  */
  uint32_t inline dma_evt_threshold_get() {
    return regmap->dma_int_status.fields.dma_evt_threshold;
  }

  /** @brief HAL Getter of signal DMA_INT_EN from register DMA_INT_EN
  *
  * When an event has its corresponding enable bit set, it will contribute to IRQ generation of the block
  *
  * @return Register value
  */
  uint32_t inline dma_int_en_get() {
    return regmap->dma_int_en.fields.dma_int_en;
  }

  /** @brief HAL Setter of signal DMA_INT_EN from register DMA_INT_EN
  *
  * When an event has its corresponding enable bit set, it will contribute to IRQ generation of the block
  *
  * @param dma_int_en Value to be set
  */
  void inline dma_int_en_set(uint32_t dma_int_en) {
    regmap->dma_int_en.fields.dma_int_en = dma_int_en;
  }

  /** @brief HAL Trigger 'write pulse' of signal DMA_INT_SET from register DMA_INT_SET
  *
  * Emulate events by software by writing ‘1’ to corresponding positions
  *
  * @param value Value to apply to signal (32 bits large)
  */
  void inline dma_int_set(uint32_t value) {
    regmap->dma_int_set.word = DMA_INT_SET_MASK & (value << DMA_INT_SET_OFFSET);
  }

  /** @brief HAL Trigger 'write pulse' of signal DMA_INT_CLEAR from register DMA_INT_CLEAR
  *
  * Erase event flags of DMA_INT_STATUS by writing ‘1’ to corresponding positions
  *
  * @param value Value to apply to signal (32 bits large)
  */
  void inline dma_int_clear(uint32_t value) {
    regmap->dma_int_clear.word = DMA_INT_CLEAR_MASK & (value << DMA_INT_CLEAR_OFFSET);
  }

  /** @brief HAL Trigger 'write pulse' of signal DMA_START from register DMA_CHAN_CTRL
  *
  * When written ‘1’, starts DMA transfer for this channel
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  */
  void inline dma_start(uint32_t index) {
    regmap->dma_chan_ctrl[index].word |= DMA_START_MASK;
  }

  /** @brief HAL Trigger 'write pulse' of signal DMA_STOP from register DMA_CHAN_CTRL
  *
  * When written ‘1’, stops current DMA transfer for this channel 
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  */
  void inline dma_stop(uint32_t index) {
    regmap->dma_chan_ctrl[index].word |= DMA_STOP_MASK;
  }

  /** @brief HAL Getter of signal DMA_RUNNING from register DMA_CHAN_CTRL
  *
  * Flag indicating when this channel is active
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_running_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_running;
  }

  /** @brief HAL Getter of signal DMA_RESTART_EN from register DMA_CHAN_CTRL
  *
  * When ‘1’, the DMA transfers restarts automatically on copy completion. Otherwise, the DMA transfers stops when over
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_restart_en_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_restart_en;
  }

  /** @brief HAL Setter of signal DMA_RESTART_EN from register DMA_CHAN_CTRL
  *
  * When ‘1’, the DMA transfers restarts automatically on copy completion. Otherwise, the DMA transfers stops when over
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_restart_en Value to be set
  */
  void inline dma_restart_en_set(uint32_t index, uint32_t dma_restart_en) {
    regmap->dma_chan_ctrl[index].fields.dma_restart_en = dma_restart_en;
  }

  /** @brief HAL Getter of signal DMA_PRIORITY from register DMA_CHAN_CTRL
  *
  * DMA priority for this channel. The higher value, the higher priority over other channels
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_priority_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_priority;
  }

  /** @brief HAL Setter of signal DMA_PRIORITY from register DMA_CHAN_CTRL
  *
  * DMA priority for this channel. The higher value, the higher priority over other channels
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_priority Value to be set
  */
  void inline dma_priority_set(uint32_t index, uint32_t dma_priority) {
    regmap->dma_chan_ctrl[index].fields.dma_priority = dma_priority;
  }

  /** @brief HAL Getter of signal DMA_STRIDE_EN from register DMA_CHAN_CTRL
  *
  * When ‘1’, strided transfers are enabled
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_stride_en_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_stride_en;
  }

  /** @brief HAL Setter of signal DMA_STRIDE_EN from register DMA_CHAN_CTRL
  *
  * When ‘1’, strided transfers are enabled
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_stride_en Value to be set
  */
  void inline dma_stride_en_set(uint32_t index, uint32_t dma_stride_en) {
    regmap->dma_chan_ctrl[index].fields.dma_stride_en = dma_stride_en;
  }

  /** @brief HAL Getter of signal DMA_PERIPH_SRC_ID from register DMA_CHAN_CTRL
  *
  * RX peripheral ID. If 0, no peripheral linked as source
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_periph_src_id_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_periph_src_id;
  }

  /** @brief HAL Setter of signal DMA_PERIPH_SRC_ID from register DMA_CHAN_CTRL
  *
  * RX peripheral ID. If 0, no peripheral linked as source
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_periph_src_id Value to be set
  */
  void inline dma_periph_src_id_set(uint32_t index, uint32_t dma_periph_src_id) {
    regmap->dma_chan_ctrl[index].fields.dma_periph_src_id = dma_periph_src_id;
  }

  /** @brief HAL Getter of signal DMA_SRC_STATIC from register DMA_CHAN_CTRL
  *
  * When ‘1’, source address is static and not incremented
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_src_static_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_src_static;
  }

  /** @brief HAL Setter of signal DMA_SRC_STATIC from register DMA_CHAN_CTRL
  *
  * When ‘1’, source address is static and not incremented
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_src_static Value to be set
  */
  void inline dma_src_static_set(uint32_t index, uint32_t dma_src_static) {
    regmap->dma_chan_ctrl[index].fields.dma_src_static = dma_src_static;
  }

  /** @brief HAL Getter of signal DMA_SRC_WIDTH from register DMA_CHAN_CTRL
  *
  * Source peripheral data transfer width in byte
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_src_width_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_src_width;
  }

  /** @brief HAL Setter of signal DMA_SRC_WIDTH from register DMA_CHAN_CTRL
  *
  * Source peripheral data transfer width in byte
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_src_width Value to be set
  */
  void inline dma_src_width_set(uint32_t index, uint32_t dma_src_width) {
    regmap->dma_chan_ctrl[index].fields.dma_src_width = dma_src_width;
  }

  /** @brief HAL Getter of signal DMA_PERIPH_DST_ID from register DMA_CHAN_CTRL
  *
  * TX peripheral ID. If 0, no peripheral linked as destination
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_periph_dst_id_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_periph_dst_id;
  }

  /** @brief HAL Setter of signal DMA_PERIPH_DST_ID from register DMA_CHAN_CTRL
  *
  * TX peripheral ID. If 0, no peripheral linked as destination
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_periph_dst_id Value to be set
  */
  void inline dma_periph_dst_id_set(uint32_t index, uint32_t dma_periph_dst_id) {
    regmap->dma_chan_ctrl[index].fields.dma_periph_dst_id = dma_periph_dst_id;
  }

  /** @brief HAL Getter of signal DMA_DST_STATIC from register DMA_CHAN_CTRL
  *
  * When ‘1’, destination address is static and not incremented
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_dst_static_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_dst_static;
  }

  /** @brief HAL Setter of signal DMA_DST_STATIC from register DMA_CHAN_CTRL
  *
  * When ‘1’, destination address is static and not incremented
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_dst_static Value to be set
  */
  void inline dma_dst_static_set(uint32_t index, uint32_t dma_dst_static) {
    regmap->dma_chan_ctrl[index].fields.dma_dst_static = dma_dst_static;
  }

  /** @brief HAL Getter of signal DMA_DST_WIDTH from register DMA_CHAN_CTRL
  *
  * Destination peripheral data transfer width in byte
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_dst_width_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_dst_width;
  }

  /** @brief HAL Setter of signal DMA_DST_WIDTH from register DMA_CHAN_CTRL
  *
  * Destination peripheral data transfer width in byte
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_dst_width Value to be set
  */
  void inline dma_dst_width_set(uint32_t index, uint32_t dma_dst_width) {
    regmap->dma_chan_ctrl[index].fields.dma_dst_width = dma_dst_width;
  }

  /** @brief HAL Getter of signal DMA_EXT_TRIG_EN from register DMA_CHAN_CTRL
  *
  * When ‘1’, an external trigger can start the DMA channel
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_ext_trig_en_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_ext_trig_en;
  }

  /** @brief HAL Setter of signal DMA_EXT_TRIG_EN from register DMA_CHAN_CTRL
  *
  * When ‘1’, an external trigger can start the DMA channel
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_ext_trig_en Value to be set
  */
  void inline dma_ext_trig_en_set(uint32_t index, uint32_t dma_ext_trig_en) {
    regmap->dma_chan_ctrl[index].fields.dma_ext_trig_en = dma_ext_trig_en;
  }

  /** @brief HAL Getter of signal DMA_EXT_TRIG_ID from register DMA_CHAN_CTRL
  *
  * External trigger selection
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_ext_trig_id_get(uint32_t index) {
    return regmap->dma_chan_ctrl[index].fields.dma_ext_trig_id;
  }

  /** @brief HAL Setter of signal DMA_EXT_TRIG_ID from register DMA_CHAN_CTRL
  *
  * External trigger selection
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_ext_trig_id Value to be set
  */
  void inline dma_ext_trig_id_set(uint32_t index, uint32_t dma_ext_trig_id) {
    regmap->dma_chan_ctrl[index].fields.dma_ext_trig_id = dma_ext_trig_id;
  }

  /** @brief HAL Getter of signal DMA_LEN from register DMA_LEN
  *
  * Number of bytes to transfer = DMA_LEN + 1
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_len_get(uint32_t index) {
    return regmap->dma_len[index].fields.dma_len;
  }

  /** @brief HAL Setter of signal DMA_LEN from register DMA_LEN
  *
  * Number of bytes to transfer = DMA_LEN + 1
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_len Value to be set
  */
  void inline dma_len_set(uint32_t index, uint32_t dma_len) {
    regmap->dma_len[index].fields.dma_len = dma_len;
  }

  /** @brief HAL Getter of signal DMA_SRC from register DMA_SRC
  *
  * Source start address of the transfer
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_src_get(uint32_t index) {
    return regmap->dma_src[index].fields.dma_src;
  }

  /** @brief HAL Setter of signal DMA_SRC from register DMA_SRC
  *
  * Source start address of the transfer
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_src Value to be set
  */
  void inline dma_src_set(uint32_t index, uint32_t dma_src) {
    regmap->dma_src[index].fields.dma_src = dma_src;
  }

  /** @brief HAL Getter of signal DMA_DST from register DMA_DST
  *
  * Destination start address of the transfer
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_dst_get(uint32_t index) {
    return regmap->dma_dst[index].fields.dma_dst;
  }

  /** @brief HAL Setter of signal DMA_DST from register DMA_DST
  *
  * Destination start address of the transfer
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_dst Value to be set
  */
  void inline dma_dst_set(uint32_t index, uint32_t dma_dst) {
    regmap->dma_dst[index].fields.dma_dst = dma_dst;
  }

  /** @brief HAL Getter of signal DMA_SDL from register DMA_SDL
  *
  * Stride data byte length = DMA_SDL + 1
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_sdl_get(uint32_t index) {
    return regmap->dma_sdl[index].fields.dma_sdl;
  }

  /** @brief HAL Setter of signal DMA_SDL from register DMA_SDL
  *
  * Stride data byte length = DMA_SDL + 1
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_sdl Value to be set
  */
  void inline dma_sdl_set(uint32_t index, uint32_t dma_sdl) {
    regmap->dma_sdl[index].fields.dma_sdl = dma_sdl;
  }

  /** @brief HAL Getter of signal DMA_SKL from register DMA_SKL
  *
  * Stride skip byte length = DMA_SKL + 1
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_skl_get(uint32_t index) {
    return regmap->dma_skl[index].fields.dma_skl;
  }

  /** @brief HAL Setter of signal DMA_SKL from register DMA_SKL
  *
  * Stride skip byte length = DMA_SKL + 1
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_skl Value to be set
  */
  void inline dma_skl_set(uint32_t index, uint32_t dma_skl) {
    regmap->dma_skl[index].fields.dma_skl = dma_skl;
  }

  /** @brief HAL Getter of signal DMA_THRESHOLD from register DMA_THRESHOLD
  *
  * During a transfer, a Threshold event is generated when the remaining number of bytes reaches this value
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @return Register value
  */
  uint32_t inline dma_threshold_get(uint32_t index) {
    return regmap->dma_threshold[index].fields.dma_threshold;
  }

  /** @brief HAL Setter of signal DMA_THRESHOLD from register DMA_THRESHOLD
  *
  * During a transfer, a Threshold event is generated when the remaining number of bytes reaches this value
  *
  * @param index Index id (from 0 to 7)
  * @attention Undefined behavior if index > 7 (out-of-bounds access)
  * @param dma_threshold Value to be set
  */
  void inline dma_threshold_set(uint32_t index, uint32_t dma_threshold) {
    regmap->dma_threshold[index].fields.dma_threshold = dma_threshold;
  }

};
