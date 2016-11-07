/*
 * samPWM.cpp
 * API for PWM peripheral.
 * Some code copied from Michael Hayes' code.
 *
 * Created: 4/05/2016 8:11:39 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samClock.hpp"

//////////////////////////////////////////////////////////////////////////

void pwmCore_c::Begin(void) {
	//Start peripheral clock, set up default internal dividers etc.
	samClock.PeriphClockEnable(ID_PWM);
	//Need something else??
}

void pwmCore_c::ClockASetup(bool enable, uint32_t preDiv, uint8_t postDiv) {
	//Enable CLKA, and set to divide by 2^n times, up to 2^10 or div/1024
	
	PWM->PWM_CLK &= ~PWM_CLK_PREA_Msk & ~PWM_CLK_DIVA_Msk; // Clear clock predivider bits
	
	if (enable) {
		//Can't predivide by more than 2^10 (note unsigned so already greater than 0)
		if (preDiv > 10) {
			preDiv = 10;
		}
		PWM->PWM_CLK |= PWM_CLK_PREA(preDiv) | PWM_CLK_DIVA(postDiv);
	}
}
uint32_t pwmCore_c::ClockAFreqGet(void) {
	//Retrieves frequency from hardware
	uint32_t preDiv = (PWM->PWM_CLK | PWM_CLK_PREA_Msk) >> PWM_CLK_PREA_Pos;
	uint32_t postDiv = (PWM->PWM_CLK | PWM_CLK_DIVA_Msk) >> PWM_CLK_DIVA_Pos;
	return samClock.MasterFreqGet() / ((1 << preDiv) * postDiv);
}

void pwmCore_c::ClockBSetup(bool enable, uint32_t preDiv, uint8_t postDiv) {
	//Enable CLKB, and set to divide by 2^n times, up to 2^10 or div/1024
	
	PWM->PWM_CLK &= ~PWM_CLK_PREB_Msk & ~PWM_CLK_DIVB_Msk; // Clear clock predivider bits
	
	if (enable) {
		//Can't predivide by more than 2^10 (note unsigned so already greater than 0)
		if (preDiv > 10) {
			preDiv = 10;
		}
		PWM->PWM_CLK |= PWM_CLK_PREB(preDiv) | PWM_CLK_DIVB(postDiv);
	}
}
uint32_t pwmCore_c::ClockBFreqGet(void) {
	//Retrieves frequency from hardware
	uint32_t preDiv = (PWM->PWM_CLK | PWM_CLK_PREB_Msk) >> PWM_CLK_PREB_Pos;
	uint32_t postDiv = (PWM->PWM_CLK | PWM_CLK_DIVB_Msk) >> PWM_CLK_DIVB_Pos;
	return samClock.MasterFreqGet() / ((1 << preDiv) * postDiv);
}

// Global definition:
pwmCore_c pwmCore;

//////////////////////////////////////////////////////////////////////////

void pwmChannel_c::Begin(uint32_t channelClockMode, bool opInvert, bool deadTimeEnable) {
	//Enables channel, with given clock rate. Note period and duty left at 0.
	
	//Set up channel register, with clock and deadtime:
	PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CMR = PWM_CMR_CPRE(channelClockMode) | \
	((uint32_t)opInvert << 9) | ((uint32_t)deadTimeEnable << 16);
	// Enable channel.
	PWM->PWM_ENA = 1 << pwmChannel_c::channel_id;
}

//Class initialiser:
pwmChannel_c::pwmChannel_c(int id) {
	pwmChannel_c::channel_id = id;
}

//Functions for setting period, in various ways:
void pwmChannel_c::PeriodSet(uint16_t period_ticks) {
	//Sets period update register, to be updated after next edge.
	PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRDUPD = period_ticks;
}
void pwmChannel_c::PeriodSet_us(uint32_t period_us) {
	//Sets period in microseconds.
	pwmChannel_c::PeriodSet((uint64_t)period_us * pwmChannel_c::tickrateGet() / 1e6);
}
uint16_t pwmChannel_c::PeriodGet(void) {
	//Retrieves period from hardware.
	return PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRD;
}
uint32_t pwmChannel_c::PeriodGet_us(void) {
	//Retrieves period from hardware, in microseconds.
	return pwmChannel_c::PeriodGet() * 1e6 / pwmChannel_c::tickrateGet();
}
void pwmChannel_c::FrequencySet(uint32_t frequency) {
	//Calculates period needed for given frequency.
	uint32_t period_ticks = pwmChannel_c::tickrateGet() / frequency;
	pwmChannel_c::PeriodSet(period_ticks);
}
uint32_t pwmChannel_c::FrequencyGet(void) {
	//calculates current frequency.
	return pwmChannel_c::tickrateGet() / pwmChannel_c::PeriodGet();
}

void pwmChannel_c::DutySet(uint16_t duty_ticks) {
	//Sets duty cycle update register, to be updated after next edge.
	//Check for duty > period (current or updated period):
// 	if ((duty_ticks > PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRD) &&
// 		(PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRD != 0)) {
// 		duty_ticks = PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRD;
// 		}
// 	if ((duty_ticks > PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRDUPD) &&
// 		(PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRDUPD != 0)) {
// 		duty_ticks = PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CPRDUPD;
// 	}
	PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CDTYUPD = duty_ticks;
}
void pwmChannel_c::DutySet_16(uint16_t duty_ratio) {
	//Sets duty as a 'percent' using the maximum 16-bit range.
	pwmChannel_c::DutySet(duty_ratio * (uint32_t)pwmChannel_c::PeriodGet() / 0xffff);
}
void pwmChannel_c::DutySet_us(uint32_t duty_us) {
	//Sets duty as a high-time in microseconds.
	pwmChannel_c::DutySet(duty_us * (uint64_t)pwmChannel_c::tickrateGet() / 1e6);
}
uint16_t pwmChannel_c::DutyGet(void) {
	//Returns duty, in ticks.
	return PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_CDTY;
}
uint16_t pwmChannel_c::DutyGet_16(void) {
	//Returns duty, as a 16-bit range.
	return pwmChannel_c::DutyGet() * (uint32_t)pwmChannel_c::PeriodGet() / 0xffff;
}
uint16_t pwmChannel_c::DutyGet_us(void) {
	//Retrieves duty from hardware, in microseconds.
	return pwmChannel_c::DutyGet() * (uint64_t)1e6 / pwmChannel_c::tickrateGet();
}

void pwmChannel_c::DeadTimeSet(uint16_t deadtime_ticks) {
	//Sets duty cycle update register, to be updated after next edge.
	deadtime_ticks &= 0x0fff; // Counter only 12-bit!
	PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_DTUPD = deadtime_ticks;
}
void pwmChannel_c::DeadTimeSet_us(uint32_t deadtime_us) {
	//Sets deadtime in microseconds.
	pwmChannel_c::DeadTimeSet(deadtime_us * pwmChannel_c::tickrateGet() / 1e6);
}
uint16_t pwmChannel_c::DeadTimeGet(void) {
	//Gets deadtime in ticks.
	return PWM->PWM_CH_NUM[pwmChannel_c::channel_id].PWM_DT;
}
uint32_t pwmChannel_c::DeadTimeGet_us(void) {
	//Gets deadtime in ticks.
	return DeadTimeGet() * 1e6 / pwmChannel_c::tickrateGet();
}

uint32_t pwmChannel_c::tickrateGet(void) {
	//Gets tickrate for counter.
	uint32_t tickRate = 0;
	uint32_t channelClockMode = (PWM->PWM_CH_NUM[this->channel_id].PWM_CMR & PWM_CMR_CPRE_Msk) >> PWM_CMR_CPRE_Pos;
	switch (channelClockMode) {
		case ch_clockA:
			tickRate = pwmCore.ClockAFreqGet();
			break;
		case ch_clockB:
			tickRate = pwmCore.ClockBFreqGet();
			break;
		default: // Will be div1 thru div1024.
			tickRate = samClock.MasterFreqGet() / (1 << channelClockMode);
			break;
	}
	return tickRate;
}

// Global definition:
pwmChannel_c pwmChannel0(0);
pwmChannel_c pwmChannel1(1);
pwmChannel_c pwmChannel2(2);
pwmChannel_c pwmChannel3(3);