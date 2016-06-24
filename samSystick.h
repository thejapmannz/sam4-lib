/*
 * samSystick.h
 *
 * Created: 8/05/2016 9:00:00 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMSYSTICK_H_
#define SAMSYSTICK_H_

#include "sam.h"

// Global instance is samSysTick.

class samSysTick_c {
	public:
		//Initialiser: Wait frequency, and portA pin to use for load indicator (-1 for none).
		void Begin(uint32_t sysTickFrequency, int32_t ledDebug);
		
		//Frequency set/get, without interrupting counter.
		void FreqSet(uint32_t frequency);
		uint32_t FreqGet(void);
		
		//Busy wait for systick counter:
		void Wait(void);
	
	private:
		int32_t ledDebug;
};

#include "samSystick.cpp"

//Global declaration:
extern samSysTick_c samSysTick;

#endif /* SAMSYSTICK_H_ */