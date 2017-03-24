/*
 * samSystick.cpp
 *
 * Created: 8/05/2016 8:59:28 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samClock.hpp"
#include "../Utilities/arduino-funcs.hpp"


void samSysTick_c::Begin(uint32_t sysTickFrequency) {
	//Resets systick and assigns given frequency.
	SysTick->CTRL = 0; // Disable temporarily, if running
	
	uint32_t ticks = samClock.MasterFreqGet() / (8 * sysTickFrequency); // SAM4 provides reference clock at MCK/8.
	ticks = ardu_constrain(ticks, 1UL, 1UL << 24);
	
	SysTick->LOAD = ticks - 1; // Load value. Note flag set when counting 1->0, so -1 ticks.
	SysTick->VAL = 0; // Reset counter
	SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk; // Enable, including interrupts
	NVIC_EnableIRQ(SysTick_IRQn);
}


void samSysTick_c::FreqSet(uint32_t sysTickFrequency) {
	//Changes systick frequency without resetting counter.
	uint32_t ticks = samClock.MasterFreqGet() / (8 * sysTickFrequency); // SAM4 provides reference clock at MCK/8.
	ticks = ardu_constrain(ticks, 1UL, 1UL << 24);
	SysTick->LOAD = ticks - 1; // Load value. Note flag set when counting 1->0, so -1 ticks.
}

uint32_t samSysTick_c::FreqGet(void) {
	//Returns frequency of systick triggers.
	return (samClock.MasterFreqGet() * 8 / SysTick->LOAD);
}


void samSysTick_c::Wait(void) {
	//Waits for SysTick counter flag to be set.
	while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {
		__WFI(); // halt CPU clock until interrupt occurs
		continue;
	}
}


// Global definition:
samSysTick_c samSysTick;


// Empty interrupt handler - only needed so that the interrupt address 
// is not Null. Feel free to comment-out and use for other purposes!
void SysTick_Handler(void) {
	return;
}