/*
 * samServo.cpp
 * Abstraction layer for hardware PWM control of servos.
 *
 * Created: 23/05/2016 8:10:29 PM
 *  Author: Ben Jones
 */ 


#include "sam.h"
#include "samClock.h"
#include "samPWM.h"

extern samClock_c samClock;
extern pwmCore_c pwmCore;

void samServo_c::attach(pwmChannel_c* channel) {
	//Starts PWM on given channel, with frequency at 50Hz
	// and period of 1.5ms (halfway for a servo).
	
	//Complicated because we need to calculate the optimal clock predivider.
	
	//Best divider for 50Hz and max. 16-bit period:
	uint32_t coreFreq = samClock.MasterFreqGet();
	uint32_t divider = 0;
	while (divider < 10 && ((coreFreq / 0xffff / (1 << divider)) > 50)) {
		divider++;
	}
	
	samServo_c::pwmCh = channel;
	
	pwmCore.Begin();
	channel->Begin(divider, true, false);
	channel->FrequencySet(50);
	channel->DutySet_us(1500);
}

void samServo_c::write(uint16_t angle) {
	//Sets PWM duty to get desired angle.
	if (angle > 180) 
		angle = 180;
	uint32_t period = (uint32_t)angle * 1000 / 180 + 1000; // Gives 1000-2000us range.
	samServo_c::pwmCh->DutySet_us(period);
}

void samServo_c::writeMicroseconds(uint16_t microseconds) {
	//Passes through value to PWM.
	samServo_c::pwmCh->DutySet_us(microseconds);
}

uint16_t samServo_c::read(void) {
	//Returns set angle in degrees.
	return (samServo_c::pwmCh->DutyGet_us() - 1000) / 180;
}