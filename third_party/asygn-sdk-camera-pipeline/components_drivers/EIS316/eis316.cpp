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

#include "eis316.h"


CameraEIS316::CameraEIS316() {}

CameraEIS316::CameraEIS316(I2CmSw *i2cmsw) : m_i2cmsw(i2cmsw) {
  this->bitdepth = 1;  // 8-bit mode when set to 1
  this->video = 0;
  m_i2cmsw->scl_pulse();  // Needed for first access to be functional
}

uint8_t CameraEIS316::readRegister(uint8_t address){
  m_i2cmsw->txbuffer[0] = address;
  m_i2cmsw->transfer(1, 1, i2c_slave_7baddr);
  return m_i2cmsw->rxbuffer[0];
}


void CameraEIS316::writeRegister(uint8_t address, uint8_t value){
  m_i2cmsw->txbuffer[0] = address;
  m_i2cmsw->txbuffer[1] = value;
  m_i2cmsw->transfer(2, 0, i2c_slave_7baddr);
}


uint8_t CameraEIS316::checkVersion() {
  if (readRegister(EIS316_REG_VERSION_A) == 0x10)
    return VERSION_OK;
  else
    return VERSION_ERROR;
}


void CameraEIS316::startupSequence(uint8_t mclk_rc_mode, uint32_t mclk_cal_freq, uint8_t max_fps) {
	uint8_t  add;

	// Reset
	writeRegister(EIS316_REG_CTRL_A, 3);
  
	// release reset
	setRSTB(true);
	
	// get version
	add = readRegister(EIS316_REG_VERSION_A);
	if(add != 0x10){
		//@TODO : error handling	
		printf("error : wrong version number\r\n");
	}
	
	// test test register 
	testRegister();
	
	// calibration rc if we use internal clock
	if (mclk_rc_mode) {
		rcManCalib(mclk_cal_freq);
	}
	
	// calibration cc
	cclkAutoCalib(255);
	
	// enable fast auto exp and trigger it once quickly
  if (max_fps <= 4)
    enableAutoExposure(70, 300, 10000, true);
  else if (max_fps <= 6)
    enableAutoExposure(70, 300, 8000, true);
  else if (max_fps <= 8)
    enableAutoExposure(70, 300, 5000, true);
  else if (max_fps <= 12)
    enableAutoExposure(70, 300, 3000, true);
  else
    enableAutoExposure(70, 300, 2000, true);
  
  int exp_time = 5000;
	writeRegister(EIS316_REG_EXP0_A, exp_time % 256);
	writeRegister(EIS316_REG_EXP1_A, exp_time >> 8);
  //disableAutoExposure();

	//writeRegister(EIS316_REG_ANALOG0_A, 4);
  
	// set mode
	setMode(EIS316_REG_SO_HDR_MODE);

  // Enable PCLK only when needed
  writeRegister(EIS316_REG_DCMI_A, 0x03);
  writeRegister(EIS316_REG_PWR_A, 0x01);

  // Enable frame start IRQ
  writeRegister(EIS316_REG_IRQEN_A, EIS316_REG_EXPAD_IRQ_MASK);
}

void CameraEIS316::setImageFormat(uint8_t image_format) {
  if (image_format == EIS316_IMAGE_FORMAT_QQVGA) {
    writeRegister(EIS316_REG_SAMPLING_A, 0x02 | (1 << 2));
  }
  else if (image_format == EIS316_IMAGE_FORMAT_QVGA) {
    writeRegister(EIS316_REG_SAMPLING_A, 0x01 | (1 << 2));
  }
  else {
    writeRegister(EIS316_REG_SAMPLING_A, 0x00);
  }
}

void CameraEIS316::setImageFlip(uint8_t flip_v, uint8_t flip_h) {
	writeRegister(EIS316_REG_FLIP_A, (flip_v << 1) | flip_h);
}

void CameraEIS316::enableAutoExposure(uint32_t target, uint32_t min, uint32_t max, bool fast){
	uint8_t lsb,msb;
	
	lsb = (target%256);
	msb = (target>>8);
	writeRegister(EIS316_REG_AUTOEXP1_A, lsb);
	writeRegister(EIS316_REG_AUTOEXP2_A, msb);
		
	lsb = (max%256);
	msb = (max>>8);
	writeRegister(EIS316_REG_AUTOEXP3_A, lsb);
	writeRegister(EIS316_REG_AUTOEXP4_A, msb);
	
	lsb = (min%256);
	msb = (min>>8);
	writeRegister(EIS316_REG_AUTOEXP5_A, lsb);
	writeRegister(EIS316_REG_AUTOEXP6_A, msb);
	
	if(fast){
		writeRegister(EIS316_REG_AUTOEXP0_A, 0x02);
		video=1;
		setMode(EIS316_REG_SO_MODE);
		trigSingleFrame();
		// would be much better to monitor IRQ instead of waiting
		sleep(100);
		stopVideoMode();
		video=0;
		setMode(EIS316_REG_SO_MODE);
	}
	
  writeRegister(EIS316_REG_CFG_A,  (video<<5) | (bitdepth<<4) | EIS316_REG_SO_MODE);
	//enable auto exp
	writeRegister(EIS316_REG_AUTOEXP0_A, 0x01);

}

void CameraEIS316::disableAutoExposure(){
	writeRegister(EIS316_REG_AUTOEXP0_A, 0x00);
}

uint32_t CameraEIS316::cclkAutoCalib(uint16_t target){

  uint8_t lsb,msb,trimCode,mode,a,b;
  uint32_t trimResults;
		
  lsb = (target % 256);
  msb = (target >> 8);
		
  mode = readRegister(EIS316_REG_CFG_A);

  //sent CCLK0 trim cmd + new mode
  writeRegister(EIS316_REG_CCLK1_A, lsb);
  writeRegister(EIS316_REG_CCLK2_A, msb);
  writeRegister(EIS316_REG_CFG_A, (0 << 5) | (bitdepth << 4) | EIS316_REG_CAL_CLK_AUTO_MODE);
  trigSingleFrame();
  sleep(20);

  //read back results : trim results + trim code
  trimCode = readRegister(EIS316_REG_CCLK0_A);
  a = readRegister(EIS316_REG_CCLK3_A);
  b = readRegister(EIS316_REG_CCLK4_A);
  trimResults = a + (b << 8);

  //go back to previous mode
  setMode(mode);

  // should return a value close to the target
  return trimResults;
}

uint32_t CameraEIS316::rcManCalib(uint32_t mclk_cal_freq){
		
  uint8_t trimCode, mode; 
  uint16_t trimResults;
  uint32_t trimTarget;
  uint16_t diffTarget = 0, diffTargetBest = 65535;

  trimTarget = 12000000 / mclk_cal_freq;

  mode=readRegister(EIS316_REG_CFG_A);
  writeRegister(EIS316_REG_CFG_A, EIS316_REG_CAL_RC_MODE);

  // Calibration loop
  for (int i = 0; i <= 31; i++) {
    writeRegister(EIS316_REG_RC0_A, i);
    sleep(20);
    // Start capture and measurement
    trigSingleFrame();
		
    // Capture delay
    sleep(10);
    trimResults = (uint16_t) readRegister(EIS316_REG_RC1_A) + ((uint16_t) readRegister(EIS316_REG_RC2_A) << 8);

    diffTarget = (trimResults < trimTarget) ? trimTarget - trimResults :
                                              trimResults - trimTarget;
    if (diffTarget > diffTargetBest) {
      if (i > 0)
        writeRegister(EIS316_REG_RC0_A, i - 1);  // The previous code was better
      break;
    }
    else {
      diffTargetBest = diffTarget;
    }
  }
  //printf("Best RC code: %d\n", readRegister(EIS316_REG_RC0_A));
  
  // Revert mode
  setMode(mode);
  return trimResults;
}

uint32_t CameraEIS316::rcAutoCalib(){
		
  uint8_t trimCode, a,b, mode; 
  uint32_t trimResults;

  mode=readRegister(EIS316_REG_CFG_A);

  writeRegister(EIS316_REG_CFG_A, EIS316_REG_CAL_RC_AUTO_MODE); 

  //@TODO set MCLK to 32.768kHZ if not already done

  //trig capture
  trigSingleFrame();
		
  //sleep for 20ms
  sleep(20);

  //read back results - can be used if needed
  trimCode = readRegister(EIS316_REG_RC0_A);
  a = readRegister(EIS316_REG_RC1_A);
  b = readRegister(EIS316_REG_RC2_A);
  trimResults = a + (b << 8);

  //go back to previous mode
  setMode(mode);

  return trimResults;
}

void CameraEIS316::setMode(uint8_t mode){
	writeRegister(EIS316_REG_CFG_A, (video<<5) | (bitdepth<<4) | mode);
}

void CameraEIS316::setExposure(uint32_t target){
	uint8_t lsb,msb;
	lsb = (target%256);
	msb = (target>>8);
	writeRegister(EIS316_REG_EXP0_A, lsb);
	writeRegister(EIS316_REG_EXP1_A, msb);
}

uint32_t CameraEIS316::getExposure(){
	uint8_t lsb,msb;

	lsb=readRegister(EIS316_REG_EXP0_A);
	msb=readRegister(EIS316_REG_EXP1_A);
	return lsb+(msb<<8);
}

void CameraEIS316::trigSingleFrame(){
	writeRegister(EIS316_REG_OPCTRL_A, 0x01);
}

void CameraEIS316::startVideoMode(uint16_t framerate){
  uint8_t mode;

  writeRegister(EIS316_REG_FRATE0_A, 16 / framerate);
  writeRegister(EIS316_REG_FRATE1_A, 0);
  
  mode = readRegister(EIS316_REG_CFG_A) | 0x20;
	writeRegister(EIS316_REG_CFG_A, mode);
	writeRegister(EIS316_REG_OPCTRL_A, 1);
}

void CameraEIS316::stopVideoMode() {
  uint8_t mode;

	writeRegister(EIS316_REG_OPCTRL_A, 0);
  mode = readRegister(EIS316_REG_CFG_A) & ~(0x20);
	writeRegister(EIS316_REG_CFG_A, mode);
}

void CameraEIS316::clearFrameStartIrq() {
  writeRegister(EIS316_REG_IRQCLR_A, EIS316_REG_EXPAD_IRQ_MASK);
}

void CameraEIS316::waitIrqEvent(uint8_t eventMask) {
  uint8_t eventFlag = 0;

  writeRegister(EIS316_REG_IRQCLR_A, eventMask); 
  while (eventFlag == 0) {
    sleep(1);
    eventFlag = readRegister(EIS316_REG_IRQFLG_A) & eventMask;
  }
}

uint32_t CameraEIS316::testRegister(){
	
	uint8_t val1, val2, val3, val4;
	
	writeRegister(EIS316_REG_TEST_A, 0x00);
	val1 = readRegister(EIS316_REG_TEST_A);
	writeRegister(EIS316_REG_TEST_A, 0x55);
	val2 = readRegister(EIS316_REG_TEST_A);
	writeRegister(EIS316_REG_TEST_A, 0xAA);
	val3 = readRegister(EIS316_REG_TEST_A);	
	writeRegister(EIS316_REG_TEST_A, 0xFF);
	val4 = readRegister(EIS316_REG_TEST_A);
	
	if(val1 != 0x00 || val2 != 0x55 || val3!=0xAA || val4!=0xFF){
		//@TODO : error handling
		printf("error : testing test register\r\n");	
		return -1;
	}
	else{
		return 0;
	}
	
}

void CameraEIS316::sleep(int ms){
  colibrySCU->delay_ms(ms);
}

uint8_t CameraEIS316::getMCLK_SEL(){
	return 1;  // TODO: here we use a static configuration
}

void CameraEIS316::setMCLK_SEL(bool value){
	//@TODO set the GPIO value
}

void CameraEIS316::setRSTB(bool value){
	//@TODO set the GPIO value
}
