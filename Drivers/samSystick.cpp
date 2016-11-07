/*
 * samSystick.cpp
 *
 * Created: 8/05/2016 8:59:28 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samGPIO.hpp"
#include "samClock.hpp"



void samSysTick_c::Begin(uint32_t frequency, int32_t ledDebug) {
	//Resets systick and assigns given frequency.
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk; // Disable temporarily, if running
	uint32_t ticks = samClock.MasterFreqGet() / frequency;
	if (ticks > 0x01000000) {
		ticks = 0x01000000; // Max 24-bit counter, and -1 later.
	}
	SysTick->LOAD = ticks - 1; // Load value. Note flag set when counting 1->0, so -1 ticks.
	SysTick->VAL = 0; // Reset counter
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk; // Enable
	
	samSysTick_c::ledDebug = ledDebug;
	if (ledDebug >= 0) {
		gpioA.PinSetLow(ledDebug);
		gpioA.PinMode(ledDebug, gpioOutput);
	}
}

void samSysTick_c::FreqSet(uint32_t frequency) {
	//Changes systick frequency without resetting counter.
	uint32_t ticks = samClock.MasterFreqGet() / frequency;
	if (ticks > 0x00ffffff) {
		ticks = 0x00ffffff; // Max 24-bit counter!
	}
	SysTick->LOAD = ticks - 1; // Load value. Note flag set when counting 1->0, so -1 ticks.
}
uint32_t samSysTick_c::FreqGet(void) {
	//Returns frequency of systick triggers.
	return (samClock.MasterFreqGet() / SysTick->LOAD);
}

void samSysTick_c::Wait(void) {
	//Waits for SysTick counter flag to be set.
	
	if (samSysTick_c::ledDebug) {
		gpioA.PinSetLow(ledDebug);
		//Wait for flag:
		while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
			continue;
		}
		gpioA.PinSetHigh(ledDebug);
	}
	else {
		//Wait for flag:
		while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
			continue;
		}
	}
}

// Global definition:
samSysTick_c samSysTick;