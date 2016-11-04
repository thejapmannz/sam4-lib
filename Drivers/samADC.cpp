/*
 * samADC.cpp
 *
 * Created: 29/05/2016 9:49:23 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samClock.h"
extern samClock_c samClock;

//ADC clock to aim for:
#define adc_clock 20000000

void samADC_c::Begin(int32_t mode, uint32_t gain) {
	//Sets up the ADC main controls.
	
	uint32_t modeRegister = 0;
	
	switch (mode) {
		case adc_modeFreerun:
			modeRegister |= ADC_MR_FREERUN_ON;
			break;
		case adc_modeSoftTrigger:
			break;
		default:// Will be a trigger mode.
			modeRegister |= ADC_MR_TRGEN | ADC_MR_TRGSEL(mode);
			break;
	}
	
	uint32_t prescale = 0;
	uint32_t clock = samClock.MasterFreqGet();
	if (clock > 20000000) { // ADC clock must be between 1 and 20 MHz.
		prescale = clock / (2*adc_clock) - 1;
	}
	modeRegister |= ADC_MR_PRESCAL(prescale); 
	
	modeRegister |= ADC_MR_TRANSFER(2); // Requirement from datasheet???
	modeRegister |= ADC_MR_STARTUP(8); // Analog reference startup time, 512 clock periods???
	
	uint32_t trackTime = clock / (2*(prescale + 1)); // Current ADC clock rate, Hz
	trackTime = trackTime / 6250000; // Minimum track time 160ns, or 6.25MHz...
	modeRegister |= ADC_MR_TRACKTIM(trackTime); // There's a +1 done in hardware, so minimum is met.
	
	ADC->ADC_MR = modeRegister;
	
	//ADC supports per-channel gain, offset, and differential mode.
	//In future, can set ANACH bit in MR, to allow different gains etc per channel.
	// For now, just one gain, and leave offsets at 0 and non-differential (COR).
	ADC->ADC_CGR = ADC_CGR_GAIN0(gain);
	
	//Enable temperature sensor - little more current drawn, but who cares.
	ADC->ADC_ACR |= ADC_ACR_TSON;
	
	samClock.PeriphClockEnable(ID_ADC);
}

void samADC_c::channelEnable(uint8_t channel) {
	//Sets relevant bit in channel enable register.
	if (channel > 15)
	channel = 15; // Only 16 channels...
	ADC->ADC_CHER = (1 << channel);
}
void samADC_c::channelDisable(uint8_t channel) {
	//Sets relevant bit in channel disable register.
	if (channel > 15)
	channel = 15; // Only 16 channels...
	ADC->ADC_CHDR = (1 << channel);
}

uint16_t samADC_c::Capture(uint8_t channel) {
	//Software-triggers ADC, waits for capture, then returns data.
	if (channel > 15) 
		channel = 15;
	
	//Enable, to avoid pointlessness and help idiots:
	this->channelEnable(channel);
	
	//Trigger:
	this->Trigger();
	
	//Wait for conversion:
	uint32_t timeouts = 45000;
	ADC->ADC_CDR[channel]; // Dummy read
	while (timeouts && !(ADC->ADC_ISR & (1 << channel))) {
		timeouts--;
	}
	return ADC->ADC_CDR[channel];
}

uint16_t samADC_c::Read(uint8_t channel) {
	//Returns last capture for provided channel.
	// No idiot checks, no disabled channel checks, etc.
	if (channel > 15)
		channel = 15; // Only 16 channels...
	return ADC->ADC_CDR[channel];
}

void samADC_c::Trigger(void) {
	//Sets software trigger flag.
	ADC->ADC_CR |= ADC_CR_START;
}

bool samADC_c::NewDataReady(uint8_t channel) {
	//Checks interrupt register for new data flag.
	if (channel > 15) 
		channel = 15;
	return (ADC->ADC_ISR & (1 << channel)) != 0;
}

//Global definition:
samADC_c samADC;