/*
 * samPWM.hpp
 * API for PWM peripheral.
 * Some code copied from Michael Hayes' code.
 *
 * Created: 4/05/2016 8:11:39 PM
 *  Author: Ben Jones
 */ 

#ifndef SAMPWM_HPP_
#define SAMPWM_HPP_

#include "sam.h"

// Copy the following into your file, after header include:
// extern pwmCore_c pwmCore;
// extern pwmChannel_c pwmChannel0;
// extern pwmChannel_c pwmChannel1;
// extern pwmChannel_c pwmChannel2;
// extern pwmChannel_c pwmChannel3;

// See SAM4S datasheet, page 40, for listing of PWM pins, 
// there are too many to list here. 


//An API for the core PWM peripheral, including the main clock:
class pwmCore_c {
	public:
		//Enable peripheral and start clocks:
		void Begin(void);
		
		//Enable PWM internal ClockDivider, and set divide ratio.
		// Predivider as below, post-divider is 0-255.
		enum {pwm_div1, pwm_div2, pwm_div4, pwm_div8, pwm_div16, pwm_div32, 
			 pwm_div64, pwm_div128, pwm_div256, pwm_div512, pwm_div1024};
		void ClockASetup(bool enable, uint32_t preDiv, uint8_t postDiv);
		void ClockBSetup(bool enable, uint32_t preDiv, uint8_t postDiv);
		uint32_t ClockAFreqGet(void);
		uint32_t ClockBFreqGet(void);
};
//////////////////////////////////////////////////////////////////////////

//An API for each of the 4 PWM channels:

class pwmChannel_c {
	public:
		//Class initialiser, allows 4 instances of pwmChannel to be made from h file:
		pwmChannel_c(int id);
		//Initialise channel. Clock is either peripheral clock (div1), peripheral
		//  clock divided (div2 thru div1024) or the core's clock A or B.
		// Remember to set relevant GPIO pin to use peripheral - this is not done
		//  here as there are too many pins we can output on.
		enum {ch_div1, ch_div2, ch_div4, ch_div8, ch_div16, ch_div32, ch_div64,
			ch_div128, ch_div256, ch_div512, ch_div1024, ch_clockA, ch_clockB};
		void Begin(uint32_t channelClockMode, bool opInvert, bool deadTimeEnable);
		
		//Functions related to PWM period. Note 16-bit counter.
		void PeriodSet(uint16_t period_ticks);
		void PeriodSet_us(uint32_t period_us);
		void FrequencySet(uint32_t frequency);
		uint16_t PeriodGet(void);
		uint32_t PeriodGet_us(void);
		uint32_t FrequencyGet(void);
		
		//Functions relating to duty. Note 16-bit counter.
		void DutySet(uint16_t duty_ticks);
		void DutySet_16(uint16_t duty_ratio);
		void DutySet_us(uint32_t duty_us);
		uint16_t DutyGet(void);
		uint16_t DutyGet_16(void);
		uint16_t DutyGet_us(void);
		
		//Functions relating to dead time. Note 12-bit counter.
		void DeadTimeSet(uint16_t deadtime_ticks);
		void DeadTimeSet_us(uint32_t deadtime_us);
		uint16_t DeadTimeGet(void);
		uint32_t DeadTimeGet_us(void);
		
		uint32_t channel_id;
	private:
		//Gets clock tickrate, in Hz:
		uint32_t tickrateGet(void);
};
//////////////////////////////////////////////////////////////////////////

#include "samPWM.cpp"

//Global classes for core and 4 channels:
extern pwmCore_c pwmCore;
extern pwmChannel_c pwmChannel0;
extern pwmChannel_c pwmChannel1;
extern pwmChannel_c pwmChannel2;
extern pwmChannel_c pwmChannel3;

#endif /* SAMPWM_HPP_ */