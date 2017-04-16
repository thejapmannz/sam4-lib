/*
 * samClock.cpp
 * Routines for managing the SAM4S clock generators.
 *
 * Created: 4/05/2016 9:34:12 AM
 * Author: Ben Jones
 */ 

#include "sam.h"

//////////////////////////////////////////////////////////////////////////
//Constants. Some from Michael's code, and need checking.
#define MCU_MAINCK_COUNT 100 // CHECK THIS Wait counter before starting.
#define MCU_PLL_COUNT 63 //CHECK THIS Wait counter before starting.

//////////////////////////////////////////////////////////////////////////

void samClock_c::CrystalStart(uint32_t crystalFreq) {
	//Start crystal oscillator:
    PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCXTBY) |
        CKGR_MOR_KEY (0x37) | CKGR_MOR_MOSCXTEN |
        CKGR_MOR_MOSCXTST (MCU_MAINCK_COUNT);
    
    /* Wait for the xtal oscillator to stabilize.  */
    while (! (PMC->PMC_SR & PMC_SR_MOSCXTS)) {
        continue;
	}
	
	this->crystalFreq = crystalFreq;

    /* Could check if xtal oscillator fails to start; say if xtal
       not connected.  */
}

void samClock_c::PeriphClockEnable(uint32_t periph_id) {
	//Enable selected peripheral clock.
	if (periph_id < 32) {
		PMC->PMC_PCER0 = 1 << periph_id;
	}
	else {
		PMC->PMC_PCER1 = 1 << (periph_id - 32);
	}
}
void samClock_c::PeriphClockDisable(uint32_t periph_id) {
	//Disable selected peripheral clock.
	if (periph_id < 32) {
		PMC->PMC_PCDR0 = 1 << periph_id;
	}
	else {
		PMC->PMC_PCDR1 = 1 << (periph_id - 32);
	}
}

void samClock_c::StartPLLA(uint32_t pll_predivide, uint32_t pll_multiplier) {
	//Starts PLLA with given multipliers. Note divider is 8-bit, multiplier is 12-bit.
	//WARNING: Set clock divider before setting source to PLL, or
	// you will momentarily force 200MHz into the CPU!
	
	if (pll_multiplier == 0) {
		pll_multiplier = 1; // Account for -1 in register set.
	}
	
	// Disable PLLA if it is running and reset fields. Clears issues with JTAG.
	PMC->CKGR_PLLAR = CKGR_PLLAR_ONE | CKGR_PLLAR_MULA (0);
	
	/* Configure and start PLLA.  The PLL start delay is MCU_PLL_COUNT
       SLCK cycles.  Note, PLLA (but not PLBB) needs the mysterious
       bit CKGR_PLLAR_ONE set.  */
    PMC->CKGR_PLLAR = CKGR_PLLAR_MULA (pll_multiplier - 1) 
        | CKGR_PLLAR_DIVA (pll_predivide) 
        | CKGR_PLLAR_PLLACOUNT (MCU_PLL_COUNT) | CKGR_PLLAR_ONE;

    /* Wait for PLLA to start up.  */
    while (! (PMC->PMC_SR & PMC_SR_LOCKA)) {
        continue;
	}
}
void samClock_c::StartPLLB(uint32_t pll_predivide, uint32_t pll_multiplier) {
	//Starts PLLB with given multipliers. Note divider is 8-bit, multiplier is 12-bit.
	//WARNING: Set clock divider before setting source to PLL, or
	// you will momentarily force 200MHz into the CPU!
	
	if (pll_multiplier == 0) {
		pll_multiplier = 1; // Account for -1 in register set.
	}
	
	// Disable PLLB if it is running and reset fields. Clears issues with JTAG.
	PMC->CKGR_PLLBR = CKGR_PLLBR_MULB (0);
	
	/* Configure and start PLLB.  The PLL start delay is MCU_PLL_COUNT
       SLCK cycles. */
    PMC->CKGR_PLLBR = CKGR_PLLBR_MULB (pll_multiplier - 1) 
        | CKGR_PLLBR_DIVB (pll_predivide) 
        | CKGR_PLLBR_PLLBCOUNT (MCU_PLL_COUNT);

    /* Wait for PLLB to start up.  */
    while (! (PMC->PMC_SR & PMC_SR_LOCKB)) {
        continue;
	}
}

void samClock_c::MasterSourceSet(uint32_t clock_source) {
	//Selects Main, Slow, or PLL as clock source. Assumes source already running!
	switch (clock_source) {
		case clock_MasterSourceMain:
			PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_MAIN_CLK; //Select main clock
			break;
		case clock_MasterSourceSlow:
			PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_SLOW_CLK; //Select slow clock
			break;
		case clock_MasterSourcePLLA:
			PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_PLLA_CLK; //Select PLLA clock
			break;
		case clock_MasterSourcePLLB:
			PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS_PLLB_CLK; //Select PLLB clock
			break;
	}
	
	this->MasterFreq = 0;
	//Set up flash wait for new frequency.
	system_init_flash(this->MasterFreqGet());
}

void samClock_c::MainSourceSet(uint32_t clock_source) {
	//Sets main clock to either RC or Xtal.
	switch (clock_source) {
		case clock_MainSourceXtal:
			PMC->CKGR_MOR |= CKGR_MOR_KEY (0x37) | CKGR_MOR_MOSCSEL; //Select crystal over internal RC for main clock
			break;
		case clock_MainSourceRC:
			PMC->CKGR_MOR = (PMC->CKGR_MOR | CKGR_MOR_KEY (0x37)) & ~CKGR_MOR_MOSCSEL; //Clear crystal select for main clock
			break;
	}
	
	this->MasterFreq = 0;
	//Set up flash wait for new frequency.
	system_init_flash(this->MasterFreqGet());
}

void samClock_c::PrescalerSet(uint32_t divide_amount) {
	//Sets prescaler divider to 1, 2, 4, ..., 64, 3. Clear then set.
	//Note that API named values are in this order, from 0x0<<4 to 0x7<<4.
	PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_PRES_Msk) | PMC_MCKR_PRES(divide_amount);
	
	this->MasterFreq = 0;
	//Set up flash wait for new frequency.
	system_init_flash(this->MasterFreqGet());
}

void samClock_c::RCFreqSet(uint32_t rc_freq) {
	//Sets RC oscillator frequency to 4, 8, or 12 MHz.
	switch (rc_freq) {
		case clock_RCFreq4M: 
			PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY (0x37) | CKGR_MOR_MOSCRCF_4_MHz;
			break;
			
		case clock_RCFreq8M:
			PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY (0x37) | CKGR_MOR_MOSCRCF_8_MHz;
			break;
		
		case clock_RCFreq12M:
			PMC->CKGR_MOR = (PMC->CKGR_MOR & ~CKGR_MOR_MOSCRCF_Msk) | CKGR_MOR_KEY (0x37) | CKGR_MOR_MOSCRCF_12_MHz;
			break;
	}
	
	this->MasterFreq = 0;
	//Set up flash wait for new frequency.
	system_init_flash(this->MasterFreqGet());
}

uint32_t samClock_c::AutoSetup(uint32_t desired_freq, uint32_t crystal_freq) {
	//Guesses an ideal setup for a given frequency, and returns calculated frequency.
	uint32_t actual_freq = 0;
	
	if (desired_freq == 0 || desired_freq > 120000000) {
		//SAM4S max frequency is 120MHz, change nothing.
		actual_freq = 0;
	}
	
	else if (desired_freq == 4000000) {
		this->RCFreqSet(clock_RCFreq4M);
		this->MainSourceSet(clock_MainSourceRC);
		this->MasterSourceSet(clock_MasterSourceMain);
		actual_freq = desired_freq;
	}
	else if (desired_freq == 8000000) {
		this->RCFreqSet(clock_RCFreq8M);
		this->MainSourceSet(clock_MainSourceRC);
		this->MasterSourceSet(clock_MasterSourceMain);
		actual_freq = desired_freq;
	}
	else if (desired_freq == 12000000) {
		this->RCFreqSet(clock_RCFreq12M);
		this->MainSourceSet(clock_MainSourceRC);
		this->MasterSourceSet(clock_MasterSourceMain);
		actual_freq = desired_freq;
	}
	else if (desired_freq == 32768) {
		this->MasterSourceSet(clock_MasterSourceSlow);
		actual_freq = desired_freq;
	}
	
	else if (crystal_freq != 0) { //Use provided crystal.
		if (desired_freq == crystal_freq) {
			this->CrystalStart(crystal_freq);
			this->MainSourceSet(clock_MainSourceXtal);
			this->MasterSourceSet(clock_MasterSourceMain);
			this->PrescalerSet(clock_PrescalerDiv1);
			actual_freq = desired_freq;
		}
		else {
			//Use highest possible PLL multiplier, below 240MHz.
			uint32_t pll_mul = 240000000 / crystal_freq;
			if (pll_mul > 80) {
				pll_mul = 80;
			}
			//Use greatest prescaler division, above desired freq: Note div3 not used.
			uint32_t prescaler_div_nr = 0;
			while (((pll_mul * crystal_freq) >> prescaler_div_nr > desired_freq) && (prescaler_div_nr < 6)) {
				prescaler_div_nr++;
			}
			prescaler_div_nr--; //Undo erroneous last add.
			//Decrement multiplier to get closer:
			while ((pll_mul * crystal_freq >> prescaler_div_nr) > desired_freq && pll_mul > 1) {
				pll_mul--;
			}
			actual_freq = pll_mul * crystal_freq >> prescaler_div_nr;
			
			//Check which of last two values was closer:
			if ((((pll_mul + 1) * crystal_freq >> prescaler_div_nr) - desired_freq) < (desired_freq - actual_freq)) {
				pll_mul++;
				actual_freq = pll_mul * crystal_freq >> prescaler_div_nr;
			}
			
			this->CrystalStart(crystal_freq);
			this->MainSourceSet(clock_MainSourceXtal);
			this->StartPLLA(1, pll_mul);
			this->PrescalerSet(prescaler_div_nr); // 0-6 corresponds to the appropriate value in the enum table!
			this->MasterSourceSet(clock_MasterSourcePLLA);
			
		}
	}
	else {//Use RC oscillator and PLL. Last-resort option.
		//Use highest possible PLL multiplier, below 240MHz.
		uint32_t pll_mul = 240/12; //Multiplier for 240MHz from max. 12MHz RC oscillator.
		//Use greatest prescaler division, above desired freq: Note div3 not used.
		uint32_t prescaler_div_nr = 0;
		while (((pll_mul * 12000000) >> prescaler_div_nr > desired_freq) && (prescaler_div_nr < 6)) {
			prescaler_div_nr++;
		}
		prescaler_div_nr--; //Undo erroneous last add.
		//Decrement multiplier to get closer:
		while ((pll_mul * 12000000 >> prescaler_div_nr) > desired_freq && pll_mul > 1) {
			pll_mul--;
		}
		actual_freq = pll_mul * 12000000 >> prescaler_div_nr;
		
		//Check which of last two values was closer:
		if ((((pll_mul + 1) * 12000000 >> prescaler_div_nr) - desired_freq) < (desired_freq - actual_freq)) {
			pll_mul++;
			actual_freq = pll_mul * 12000000 >> prescaler_div_nr;
		}
		
		this->RCFreqSet(clock_RCFreq12M);
		this->MainSourceSet(clock_MainSourceRC);
		this->StartPLLA(1, pll_mul);
		this->PrescalerSet(prescaler_div_nr); // 0-6 corresponds to the appropriate value in the enum table!
		this->MasterSourceSet(clock_MasterSourcePLLA);
		
	}
	
	this->MasterFreq = actual_freq;
	//Set up flash wait for new frequency.
	system_init_flash(this->MasterFreqGet());
	return actual_freq;
}

uint32_t samClock_c::MasterFreqGet(void) {
	//Calculates clock frequency. Taken from SAM4 specification.
	
	if (this->MasterFreq == 0) {
	
		uint32_t SystemCoreClock = 0;
		/* Determine clock frequency according to clock register values */
		switch ( PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk ) {
			case PMC_MCKR_CSS_SLOW_CLK: /* Slow clock */
				if ( SUPC->SUPC_SR & SUPC_SR_OSCSEL ) {
					SystemCoreClock = CHIP_FREQ_XTAL_32K;
				}
				else {
					SystemCoreClock = CHIP_FREQ_SLCK_RC;
				}
				break;

			case PMC_MCKR_CSS_MAIN_CLK: /* Main clock */
				if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL ) {
					SystemCoreClock = samClock_c::crystalFreq;
				}
				else {
					SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

					switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk ) {
						case CKGR_MOR_MOSCRCF_4_MHz:
							SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;
							break;

						case CKGR_MOR_MOSCRCF_8_MHz:
							SystemCoreClock = CHIP_FREQ_MAINCK_RC_8MHZ;
							break;

						case CKGR_MOR_MOSCRCF_12_MHz:
							SystemCoreClock = CHIP_FREQ_MAINCK_RC_12MHZ;
							break;

						default:
							break;
					}
				}
				break;

			case PMC_MCKR_CSS_PLLA_CLK:	/* PLLA clock */
			case PMC_MCKR_CSS_PLLB_CLK:	/* PLLB clock */
				if ( PMC->CKGR_MOR & CKGR_MOR_MOSCSEL ) {
					SystemCoreClock = samClock_c::crystalFreq;
				}
				else {
					SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;

					switch ( PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk ) {
						case CKGR_MOR_MOSCRCF_4_MHz:
							SystemCoreClock = CHIP_FREQ_MAINCK_RC_4MHZ;
							break;

						case CKGR_MOR_MOSCRCF_8_MHz:
							SystemCoreClock = CHIP_FREQ_MAINCK_RC_8MHZ;
							break;

						case CKGR_MOR_MOSCRCF_12_MHz:
							SystemCoreClock = CHIP_FREQ_MAINCK_RC_12MHZ;
							break;

						default:
							break;
					}
				}

				if ( (uint32_t)(PMC->PMC_MCKR & (uint32_t) PMC_MCKR_CSS_Msk) == PMC_MCKR_CSS_PLLA_CLK ) {
					SystemCoreClock *= ((((PMC->CKGR_PLLAR) & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos) + 1U);
					SystemCoreClock /= ((((PMC->CKGR_PLLAR) & CKGR_PLLAR_DIVA_Msk) >> CKGR_PLLAR_DIVA_Pos));
				}
				else {
					SystemCoreClock *= ((((PMC->CKGR_PLLBR) & CKGR_PLLBR_MULB_Msk) >> CKGR_PLLBR_MULB_Pos) + 1U);
					SystemCoreClock /= ((((PMC->CKGR_PLLBR) & CKGR_PLLBR_DIVB_Msk) >> CKGR_PLLBR_DIVB_Pos));
				}
				break;

			default:
				break;
		}

		if ( (PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) == PMC_MCKR_PRES_CLK_3 ) {
			SystemCoreClock /= 3U;
		}
		else {
			SystemCoreClock >>= ((PMC->PMC_MCKR & PMC_MCKR_PRES_Msk) >> PMC_MCKR_PRES_Pos);
		}
		
		this->MasterFreq = SystemCoreClock;
	}
	
	return this->MasterFreq;
}

uint32_t samClock_c::MainFreqMeasure(bool wait) {
	//Calculates master clock frequency from measurement register.
	//Option to wait for measurement to be ready, if not already.
	
	while (wait && !(PMC->CKGR_MCFR & CKGR_MCFR_MAINFRDY)) {
		continue;
	}
	
	if (PMC->CKGR_MCFR & CKGR_MCFR_MAINFRDY)
		return (PMC->CKGR_MCFR & CKGR_MCFR_MAINF_Msk) * 2048; // 2048 = 32768 / 16;
	else
		return 0;
}

void samClock_c::delay_us(uint32_t time_us) {
	uint32_t loops = this->MasterFreqGet() / 1000000;
	
	while (time_us) {
		this->delay_cycles(loops); // Delay one microsecond
		time_us--;
	}
}

void samClock_c::delay_ms(uint32_t time_ms) {
	uint32_t loops = this->MasterFreqGet() / 1000;
	
	while (time_ms) {
		this->delay_cycles(loops); // Delay one millisecond
		time_ms--;
	}
}

void samClock_c::delay_cycles(uint32_t cycles) {
	//Busy wait. Note 3 instructions per loop.
	uint32_t loops = cycles / 3;
	
	asm volatile (
		"mov r3, %[loops] \n"	// Load the loop counter
		"loop: \n"				// Code section label
		"    subs r3, #1 \n"	// Subtract 1 from counter
		"    bne loop \n"		// If counter zero
		:						// Empty output list
		: [loops] "r" (loops)	// Input list
		: "r3", "cc"			// Clobber list
	);
}

//Global instance:
samClock_c samClock;