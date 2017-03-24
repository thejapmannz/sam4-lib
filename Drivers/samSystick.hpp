/*
 * samSystick.hpp - the ARM Cortex SysTick timer. 
 * 
 * TODO: Convert from LED Debug pin (dependent on GPIO) 
 *  to function-callback mode.
 *
 * Created: 8/05/2016 9:00:00 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMSYSTICK_HPP_
#define SAMSYSTICK_HPP_

#include "sam.h"


class samSysTick_c {
	public:
		//Initialiser: Wait frequency
		void Begin(uint32_t sysTickFrequency);
		
		//Frequency set/get, without interrupting counter.
		void FreqSet(uint32_t sysTickFrequency);
		uint32_t FreqGet(void);
		
		//Busy wait for systick counter, by disabling CPU clock:
		void Wait(void);
};

#include "samSystick.cpp"

//Global declaration:
extern samSysTick_c samSysTick;

#endif /* SAMSYSTICK_HPP_ */