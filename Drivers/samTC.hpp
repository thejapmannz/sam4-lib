/* 
 * samTC.hpp
 * Driver for the Timer-Counter peripheral. Allows a variety of 
 *     timer functions and techniques.
 * The interrupt handlers (IRQ's) have not been used in this library, 
 *     so the user is free to implement their own interrupt system. 
 *     An interface for this is still to be written!
 * 
 * TODO: Add support for waveform mode (not just capture mode)
 * TODO: Add interface for enabling/disabling interrupts. 
 * 
 * Created: 7/11/2016
 * Author: Benjamin Jones
 */

#ifndef SAMTC_HPP
#define SAMTC_HPP

#include "sam.h"

//Options for functions arguments:
//enum{};


class samTC_c {
	
	public:
		samTC_c(int ID);
		
		// Set up peripheral:
		void Begin(uint32_t clockSource);
		
		//Calculate tick rate from clock freq, source, etc...
		uint32_t getTickrate(void);
		
		
		// Software start, stop, and trigger (reset and start) to start counting:
		void Trigger(void);
		void Start(void);
		void Stop(void);
		
		
	
	private:
		int ID;
		Tc* base;
	
};

#include "samTC.cpp"

//Global declarations:
extern samTC_c samTC0;

#endif