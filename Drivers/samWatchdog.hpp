/*
 * samWatchdog.hpp - the ARM Cortex Watchdog timer. 
 * 
 * This is a very simple interface, as a watchdog is supposed to be a simple device.
 * More functionality is available in hardware - to be explored later.
 *
 * Created: 19/03/2017 7:00:00 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMWATCHDOG_HPP_
#define SAMWATCHDOG_HPP_

#include "sam.h"


class samWatchdog_c {
	public:
		//Initialiser. Zero disables the watchdog.
		// Warning: This can only be called once; the registers are 
		// hardware write-once protected.
		void Begin(uint32_t TimeoutPeriod_ms);
		
		//Reset:
		void Kick(void);
};

#include "samWatchdog.cpp"

//Global declaration:
extern samWatchdog_c samWatchdog;

#endif /* SAMWATCHDOG_HPP_ */