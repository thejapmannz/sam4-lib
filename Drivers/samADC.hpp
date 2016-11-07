/*
 * samADC.hpp
 *
 * Created: 29/05/2016 9:50:00 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMADC_HPP_
#define SAMADC_HPP_

#include "sam.h"

enum {adc_modeFreerun = -2, 
	adc_modeSoftTrigger = -1,
	adc_modeExternalTrigger = 0, 
	adc_modeTC0Trigger, 
	adc_modeTC1Trigger, 
	adc_modeTC2Trigger, 
	adc_modePWME0Trigger, 
	adc_modePWME1Trigger};

enum {//adc_gain0_5, 
	adc_gain1, 
	adc_gain2, 
	adc_gain4};

class samADC_c {
	public:
		//Initialiser: Sets up clock, trigger modes, etc.
		void Begin(int32_t mode, uint32_t gain);
		
		//Enable and disable conversion for a given channel.
		// Note there is only 1 ADC at 1MSa, so 2 channels means 500kSa, etc.
		void channelEnable(uint8_t channel);
		void channelDisable(uint8_t channel);
		
		//Triggers the ADC, then returns captured value. Blocking.
		uint16_t Capture(uint8_t channel);
		
		//Returns the last ADC capture for the provided channel. No idiot checks!
		uint16_t Read(uint8_t channel);
		//Triggers the ADC conversion via software trigger.
		void Trigger(void);
		//Check if new capture had been performed since last read:
		bool NewDataReady(uint8_t channel);
};

#include "samADC.cpp"

//Global declaration:
extern samADC_c samADC;

#endif /* SAMADC_HPP_ */