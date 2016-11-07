/*
 * samServo.h
 * Abstraction layer for hardware PWM control of servos.
 * Based on the Arduino style, and intended to be a drop-in replacement.
 *
 * Created: 23/05/2016 8:12:50 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMSERVO_H_
#define SAMSERVO_H_

#include "sam.h"
#include "../Drivers/samPWM.h"

class samServo_c {
	public:
		//Initialiser: Need to know which PWM channel you want. 
		// Again, too many PWM pin options, so GPIO pin and  
		// peripheral mode still need to be set manually (see 
		// GPIO library and SAM4s manual page 40).
		//Overdrive option gives extended pulse width range, for sloppy servos.
		void attach(pwmChannel_c* channel, bool overdrive);
		
		//Sets or retrieves angle of servo, in degrees.
 		void write(uint16_t angle);
 		uint16_t read(void);
 		
 		void writeMicroseconds(uint16_t microseconds);
		
	private:
		pwmChannel_c* pwmCh;
		bool overdrive;
};

#include "samServo.cpp"

#endif /* SAMSERVO_H_ */