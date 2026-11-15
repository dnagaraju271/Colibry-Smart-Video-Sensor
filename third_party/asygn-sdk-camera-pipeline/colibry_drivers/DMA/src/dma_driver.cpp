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



#include "dma_driver.hpp"

DmaDriver::DmaDriver() {
  set_periph_addr(DMA_BASE_ADDR);
}

uint8_t DmaDriver::done_evt_get(uint8_t dma_channel) {
  return (regmap->dma_int_status.fields.dma_evt_done & (1 << dma_channel));
}

uint8_t DmaDriver::read_err_evt_get(uint8_t dma_channel) {
  return (regmap->dma_int_status.fields.dma_evt_read_error & (1 << dma_channel));
}

uint8_t DmaDriver::write_err_evt_get(uint8_t dma_channel) {
  return (regmap->dma_int_status.fields.dma_evt_write_error & (1 << dma_channel));
}

void DmaDriver::evt_clear(uint8_t dma_channel) {
  regmap->dma_int_clear.word = (1 << (dma_channel + DMA_EVT_DONE_OFFSET)) |
                               (1 << (dma_channel + DMA_EVT_READ_ERROR_OFFSET)) |
                               (1 << (dma_channel + DMA_EVT_WRITE_ERROR_OFFSET));
}


void DmaDriver::done_irq_enable(uint8_t dma_channel) {
  regmap->dma_int_en.word |= (1 << (dma_channel + DMA_EVT_DONE_OFFSET));
}

void DmaDriver::rderr_irq_enable(uint8_t dma_channel) {
  regmap->dma_int_en.word |= (1 << (dma_channel + DMA_EVT_READ_ERROR_OFFSET));
}

void DmaDriver::wrerr_irq_enable(uint8_t dma_channel) {
  regmap->dma_int_en.word |= (1 << (dma_channel + DMA_EVT_WRITE_ERROR_OFFSET));
}

void DmaDriver::src_periph_en(uint8_t dma_channel, uint8_t periph_id) {
#if defined(COLIBRYNPU_V2)
  dma_periph_src_id_set(dma_channel, periph_id);
#else
  dma_periph_src_id_set(dma_channel, (periph_id << 1) | 1);
#endif
}

void DmaDriver::src_periph_dis(uint8_t dma_channel) {
  dma_periph_src_id_set(dma_channel, 0);
}

void DmaDriver::dst_periph_en(uint8_t dma_channel, uint8_t periph_id) {
#if defined(COLIBRYNPU_V2)
  dma_periph_dst_id_set(dma_channel, periph_id);
#else
  dma_periph_dst_id_set(dma_channel, (periph_id << 1) | 1);
#endif
}

void DmaDriver::dst_periph_dis(uint8_t dma_channel) {
  dma_periph_dst_id_set(dma_channel, 0);
}

void DmaDriver::driver_irq_handler() {
    uint8_t evt;
    int8_t status;
    uint8_t chan_id;

    evt = dma_evt_read_error_get();
    if (evt)
        status = READ_ERR;
    else {
        evt = dma_evt_read_error_get();
        if (evt)
            status = WRITE_ERR;
        else {
            evt = dma_evt_done_get();
            status = STATUS_OK;
        }
    }

    for (int i = 0; i < CHAN_NB; i++) {
        if (((evt) >> i) & 1) {
            chan_id = i;
            break;
        }
    }

    chan_running[chan_id] = false;
    dma_int_clear(evt | evt << 8 | evt << 16);
}

int8_t DmaDriver::get_free_chan() {
    for (int i = 0; i < CHAN_NB; i++)
        if (!chan_running[i])
            return i;
    return CHANNEL_FULL_ERR;
}

int8_t DmaDriver::memcpy(uint32_t* src, uint32_t* dst, uint32_t size, bool stride_cpy, uint32_t sdl, uint32_t skl) {
    uint8_t chan_id;

    chan_id = get_free_chan();
    if (chan_id == CHANNEL_FULL_ERR)
        return CHANNEL_FULL_ERR;
    chan_running[chan_id] = true;

    dma_restart_en_set(chan_id, 0);
    dma_priority_set(chan_id, LOW_PRIORITY);
    dma_stride_en_set(chan_id, stride_cpy);
    dma_periph_src_id_set(chan_id, 0);
    dma_periph_dst_id_set(chan_id, 0);

    dma_len_set(chan_id, size);
    dma_src_set(chan_id, (uint32_t)src);
    dma_dst_set(chan_id, (uint32_t)dst);
    dma_sdl_set(chan_id, sdl);
    dma_skl_set(chan_id, skl);

    dma_start(chan_id);

    return chan_id;
}

int8_t DmaDriver::periph2mem(dma_periph_t periph_src, uint32_t* dst, uint32_t size, bool restart) {
    uint8_t chan_id;

    chan_id = get_free_chan();
    if (chan_id == CHANNEL_FULL_ERR)
        return CHANNEL_FULL_ERR;
    chan_running[chan_id] = true;

    dma_restart_en_set(chan_id, restart);
    dma_priority_set(chan_id, HIGH_PRIORITY);
    dma_stride_en_set(chan_id, 0);
    src_periph_en(chan_id, periph_src.id);
    dma_src_width_set(chan_id, periph_src.rx_fifo_width);
    dst_periph_dis(chan_id);

    dma_src_set(chan_id, (uint32_t)periph_src.rx_fifo_addr);
    if (dma_src_get(chan_id) != (uint32_t)periph_src.rx_fifo_addr)
        return PARAM_ERR;

    dma_dst_set(chan_id, (uint32_t)dst);
    if (dma_dst_get(chan_id) != (uint32_t)dst)
        return PARAM_ERR;

    dma_len_set(chan_id, size);
    if (dma_len_get(chan_id) != size)
        return PARAM_ERR;

    dma_start(chan_id);

    return chan_id;
}

int8_t DmaDriver::mem2periph(uint32_t* src, dma_periph_t periph_dst, uint32_t size, bool restart) {
    uint8_t chan_id;

    chan_id = get_free_chan();
    if (chan_id == CHANNEL_FULL_ERR)
        return CHANNEL_FULL_ERR;
    chan_running[chan_id] = true;

    dma_restart_en_set(chan_id, restart);
    dma_priority_set(chan_id, HIGH_PRIORITY);
    dma_stride_en_set(chan_id, 0);
    dst_periph_en(chan_id, periph_dst.id);
    dma_dst_width_set(chan_id, periph_dst.tx_fifo_width);

    dma_src_set(chan_id, (uint32_t)src);
    if (dma_src_get(chan_id) != (uint32_t)src)
        return PARAM_ERR;

    dma_dst_set(chan_id, (uint32_t)periph_dst.tx_fifo_addr);
    if (dma_dst_get(chan_id) != (uint32_t)periph_dst.tx_fifo_addr)
        return PARAM_ERR;

    dma_len_set(chan_id, size);
    if (dma_len_get(chan_id) != size)
        return PARAM_ERR;

    dma_start(chan_id);

    return chan_id;
}

void DmaDriver::wait_all_jobs() {
    uint8_t busy;

    do {
        busy = 0;
        for (int i = 0; i < CHAN_NB; i++)
            busy |= chan_running[i];
    } while (busy);
}

int8_t DmaDriver::dma_wait_for_end_of_job(uint8_t channel) {
  int8_t status;
  
  if (dma_running_get(channel) == 0)
    return STATUS_OK;
  
  while ((done_evt_get(channel) | read_err_evt_get(channel) | write_err_evt_get(channel)) == 0) ;
  if (done_evt_get(channel)) status = STATUS_OK;
  if (read_err_evt_get(channel)) status = READ_ERR;
  if (write_err_evt_get(channel)) status = WRITE_ERR;

  evt_clear(channel);
  return status;
}

