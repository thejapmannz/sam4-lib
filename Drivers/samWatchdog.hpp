/*
 * samWatchdog.hpp - the ARM Cortex Watchdog timer. 
 * 
 * This is a very simple interface, as a watchdog is supposed to be a simple device.
 *
 * Created: 19/03/2017 7:00:00 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMWATCHDOG_HPP_
#define SAMWATCHDOG_HPP_

#include "sam.h"


class samWatchdog_c {
	public:
		//Initialiser: Wait frequency. Zero disables the watchdog.
		void Begin(uint32_t TimeoutPeriod_ms);
		
		//Reset:
		void Kick(void);
};

#include "samWatchdog.cpp"

//Global declaration:
extern samWatchdog_c samWatchdog;

#endif /* SAMWATCHDOG_HPP_ */