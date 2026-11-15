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
#include "dma_hal.hpp"

typedef struct {
    uint8_t id;
    uint32_t rx_fifo_addr;
    uint8_t rx_fifo_width;
    uint32_t tx_fifo_addr;
    uint8_t tx_fifo_width;
} dma_periph_t;

class DmaDriver : public DmaHal {
  public:
    static const uint8_t CHAN_NB = 8;

    static const int8_t STATUS_OK = 0;
    static const int8_t PARAM_ERR = -1;
    static const int8_t READ_ERR = -2;
    static const int8_t WRITE_ERR = -3;
    static const int8_t CHANNEL_FULL_ERR = -4;

    static const uint8_t ULTRA_LOW_PRIORITY = 0;
    static const uint8_t LOW_PRIORITY = 1;
    static const uint8_t HIGH_PRIORITY = 2;
    static const uint8_t ULTRA_HIGH_PRIORITY = 3;

    static const uint8_t BYTE_WIDTH_1 = 0;
    static const uint8_t BYTE_WIDTH_2 = 1;
    static const uint8_t BYTE_WIDTH_4 = 2;

    volatile bool chan_running[CHAN_NB] = {false};


    DmaDriver();

    /** 
    * @brief IRQ handler that should be called within the global SPIMx IRQ handler.
    */
    void driver_irq_handler();
  
    /**
     * @brief Get the done status.
     *​
     */
    uint8_t done_evt_get(uint8_t dma_channel);

    /**
     * @brief Get the read error status.
     *​
     */
    uint8_t read_err_evt_get(uint8_t dma_channel);
 
    /**
     * @brief Get the write error status.
     *​
     */
    uint8_t write_err_evt_get(uint8_t dma_channel);

    /**
     * @brief Clears all events for a given channel.
     *​
     */
    void evt_clear(uint8_t dma_channel);
  
    /**
     * @brief Enable the done IRQ.
     *​
     */
    void done_irq_enable(uint8_t dma_channel);

    /**
     * @brief Enable the read error IRQ.
     *​
     */
    void rderr_irq_enable(uint8_t dma_channel);

    /**
     * @brief Enable the write error IRQ.
     *​
     */
    void wrerr_irq_enable(uint8_t dma_channel);
  


    /**
     * @brief Enable the source peripheral mode for the channel number 'dma_channel'.
     *​
     * @param[in]  dma_channel DMA channel number.
     * @param[in]  periph_id ID of the peripheral to link to this channel.
     *​
     */
    void src_periph_en(uint8_t dma_channel, uint8_t periph_id);


    /**
     * @brief Disable the source peripheral mode for the channel number 'dma_channel'.
     *​
     */
    void src_periph_dis(uint8_t dma_channel);


     /**
     * @brief Enable the destination peripheral mode for the channel number 'dma_channel'.
     *​
     * @param[in]  dma_channel DMA channel number.
     * @param[in]  periph_id ID of the peripheral to link to this channel.
     *​
     */
    void dst_periph_en(uint8_t dma_channel, uint8_t periph_id);


    /**
     * @brief Disable the destination peripheral mode for the channel number 'dma_channel'.
     *​
     */
    void dst_periph_dis(uint8_t dma_channel);

    /**
     * @brief Return the first free channel.
     *​
     * @return  Channel ID
     * @retval  [0, CHAN_NB - 1]  If a channel is available.
     * @retval  DmaDriver::CHANNEL_FULL_ERR  If no channel is available.
     *​
     */
    int8_t get_free_chan();

    /**
     * @brief Copy data form src to dst.
     *​
     * @param[in]  src        Source start address.
     * @param[out] dst        Destination start address.
     * @param[in]  size       Number of bytes that should be transferred.
     * @param[in]  stride_cpy Number of bytes that should be read.
     * @param[in]  sdl        Ignored if stride_cpy is false. Number of bytes that should be read.
     * @param[in]  skl        Ignored if stride_cpy is false. Number of bytes that should be skipped.
     *
     * @return  Channel ID of the copy
     * @retval  [0, CHAN_NB - 1]  If a copy has started.
     * @retval  DmaDriver::CHANNEL_FULL_ERR else.
     *​
     */
    int8_t memcpy(uint32_t* src, uint32_t* dst, uint32_t size, bool stride_cpy = false, uint32_t sdl = 0, uint32_t skl = 0);

    /**
     * @brief Copy data from peripheral src to memory dst.
     * Do not use IRQ.
     *​
     * @param[in]  periph_src  Source peripheral.
     * @param[out] dst         Destination start address.
     * @param[in]  size        Number of bytes that should be transferred.
     * @param[in]  restart     If true, the same transfer is relaunched on compleption.
     *​
     * @return  Channel ID of the copy
     * @retval  [0, CHAN_NB - 1]  If a copy has started.
     * @retval  DmaDriver::CHANNEL_FULL_ERR else.
     */
    int8_t periph2mem(dma_periph_t periph_src, uint32_t* dst, uint32_t size, bool restart = false);

    /**
     * @brief Copy data from memory src to peripheral dst.
     * Do not use IRQ.
     *​
     * @param[in]  src         Source start address.
     * @param[out] periph_dst  Destination start address.
     * @param[in]  size        Number of bytes that should be transferred.
     * @param[in]  restart     If true, the same transfer is relaunched on compleption.
     *​
     * @return  Channel ID of the copy
     * @retval  [0, CHAN_NB - 1]  If a copy has started.
     * @retval  DmaDriver::CHANNEL_FULL_ERR else.
     */
    int8_t mem2periph(uint32_t* src, dma_periph_t periph_dst, uint32_t size, bool restart = false);

    void wait_all_jobs();

    int8_t dma_wait_for_end_of_job(uint8_t channel);
    
};
