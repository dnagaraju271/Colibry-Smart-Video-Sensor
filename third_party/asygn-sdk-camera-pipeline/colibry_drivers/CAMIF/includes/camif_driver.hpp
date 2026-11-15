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
#include "camif_hal.hpp"

#define CAMIF_DATA_IN_MODE_RAW 0
#define CAMIF_DATA_IN_MODE_RGBSPLIT 1
#define CAMIF_DATA_IN_MODE_RGB555 2
#define CAMIF_DATA_IN_MODE_RGB565 3
#define CAMIF_DATA_IN_MODE_RGB444x 4
#define CAMIF_DATA_IN_MODE_RGBx444 5

#define CAMIF_NIBBLE_MSB_FIRST 0
#define CAMIF_NIBBLE_LSB_FIRST 1

#define CAMIF_PCLK_SAMPLE_RISING_EDGE 0
#define CAMIF_PCLK_SAMPLE_FALLING_EDGE 1

#define CAMIF_PATGEN_DATA_MODE_PSEUDO_RANDOM 0
#define CAMIF_PATGEN_DATA_MODE_RAMP 1


typedef void (*Callback_camif)(void);


/**
 * @brief The CamifDriver class allows controlling the CAMIF peripheral
 *        of the ColibryNPU.
 */
class CamifDriver : public CamifHal {
 private:
  Callback_camif callback_camif;
  
 public:
  colibry_periph_t this_periph;

  CamifDriver();


  /** 
   * @brief IRQ handler that should be called within the global CAMIF IRQ handler.
   */
  void driver_irq_handler();
};
