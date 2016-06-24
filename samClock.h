/*
 * samClock.h
 * Routines for managing the SAM4S clock generators.
 *
 * Created: 4/05/2016 10:09:51 AM
 *  Author: Ben Jones
 */ 


#ifndef INCSAMCLOCK_H
#define INCSAMCLOCK_H

#include "sam.h"

// Copy into your main file after file include:
// extern samClock_c samClock;

enum {clock_RCFreq4M, 
	  clock_RCFreq8M, 
	  clock_RCFreq12M};
	  
enum {clock_MainSourceRC, 
	  clock_MainSourceXtal};
	  
enum {clock_MasterSourceSlow, 
	  clock_MasterSourceMain, 
	  clock_MasterSourcePLLA, 
	  clock_MasterSourcePLLB};
	  
enum {clock_PrescalerDiv1, 
	  clock_PrescalerDiv2, 
	  clock_PrescalerDiv4, 
	  clock_PrescalerDiv8,
	  clock_PrescalerDiv16, 
	  clock_PrescalerDiv32, 
	  clock_PrescalerDiv64, 
	  clock_PrescalerDiv3};

class samClock_c {
	public:
		//Automatically guesses the best combo to get the desired frequency.
		uint32_t AutoSetup(uint32_t desired_freq, uint32_t crystal_freq);
		
		//Sets frequency of internal RC oscillator.
		void RCFreqSet(uint32_t rc_freq);
		
		//Enables external crystal driver.
		void CrystalStart(uint32_t crystalFreq);
		
		//Enable clock to selected peripheral number.
		void PeriphClockEnable(uint32_t periph_id);
		void PeriphClockDisable(uint32_t periph_id);
		
		//Start PLL, with options for predivider and multiplier.
		// Note divider max. 0xff/8-bit, multiplier max. 0x7ff/11-bit/2047.
		//WARNING: Set clock divider before setting source to PLL, or 
		// you will momentarily force 200MHz into the CPU!
		void StartPLLA(uint32_t pll_predivide, uint32_t pll_multiplier);
		void StartPLLB(uint32_t pll_predivide, uint32_t pll_multiplier);
		
		//Selects between the various clock sources for the main and master clocks.
		// Main clock is 3-20MHz, from internal RC or external crystal. 
		// Master clock is what drives CPU, from Main, PLL, or Slow clocks.
		void MasterSourceSet(uint32_t clock_source);
		void MainSourceSet(uint32_t clock_source);
		
		//Sets prescaler on master clock.
		void PrescalerSet(uint32_t divide_amount);
		
		//Calculates actual clock frequency:
		uint32_t MasterFreqGet(void);
		//Uses slow clock to measure main clock freq:
		uint32_t MainFreqMeasure(bool wait);
		
		//Delay commands, based on frequency get.
		void delay_us(uint32_t time_us);
		void delay_ms(uint32_t time_ms);
	
	private:
		uint32_t crystalFreq;
};

#include "samClock.cpp"

//Global declaration:
extern samClock_c samClock;

#endif /* INCSAMCLOCK_H */