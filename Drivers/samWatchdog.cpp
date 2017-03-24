/*
 * samWatchdog.cpp - the ARM Cortex Watchdog timer. 
 * 
 * This is a very simple interface, as a watchdog is supposed to be a simple device.
 *
 * Created: 19/03/2017 7:00:00 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "../Utilities/arduino-funcs.hpp"


void samWatchdog_c::Begin(uint32_t TimeoutPeriod_ms) {
	//Reset watchdog and set timeout frequency.
	uint32_t timer_ticks = TimeoutPeriod_ms * 256 / 1000; // Watchdog runs from SLCK/128=256Hz
	timer_ticks = ardu_constrain(timer_ticks, 0UL, 0xfffUL); // 12-bit counter
	
	if (timer_ticks) {
		// Timeout and reset threshold as calculated, allow resetting of system, 
		//     and pause when in debug mode.
		WDT->WDT_MR = WDT_MR_WDD(timer_ticks) | WDT_MR_WDV(timer_ticks) | 
			WDT_MR_WDRSTEN | WDT_MR_WDDBGHLT;
	}
	else {
		// Maximum period, and disable bit.
		WDT->WDT_MR = WDT_MR_WDD(0xfff) | WDT_MR_WDV(0xfff) | WDT_MR_WDDIS;
	}
	
}

void samWatchdog_c::Kick(void) {
	// Reset watchdog counter.
	WDT->WDT_CR = WDT_CR_KEY_PASSWD | WDT_CR_WDRSTT;
}

// Global definition:
samWatchdog_c samWatchdog;