/*
 * samGPIO.cpp
 * Generic GPIO high/low/input/pullup driver, similar to Arduino style.
 *
 *
 * Created: 4/05/2016 8:42:04 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samClock.hpp"


gpioPort_c::gpioPort_c(char port) {
	switch (port) {
		case 'A':
			this->base = PIOA;
			this->port_id = ID_PIOA;
			break;
		case 'B':
			this->base = PIOB;
			this->port_id = ID_PIOB;
			break;
#ifdef PIOC
		case 'C':
			this->base = PIOC;
			this->port_id = ID_PIOC;
			break;
#endif
	}
}

void gpioPort_c::Begin(void) {
	//Enables peripheral clock to GPIO. Necessary if using as inputs.
	samClock.PeriphClockEnable(this->port_id);
}

void gpioPort_c::PinSet(uint32_t pin, bool state) {
	//Sets or clears state of given pin.
	if (state) {
		this->base->PIO_SODR = 1 << pin;
	}
	else {
		this->base->PIO_CODR = 1 << pin;
	}
}
void gpioPort_c::PinSetHigh(uint32_t pin) {
	this->base->PIO_SODR = 1 << pin;
}
void gpioPort_c::PinSetLow(uint32_t pin) {
	this->base->PIO_CODR = 1 << pin;
}


bool gpioPort_c::PinRead(uint32_t pin) {
	//Reads pin value.
	return (this->base->PIO_PDSR & (1 << pin)) != 0;
}
bool gpioPort_c::PinReadSetVal(uint32_t pin) {
	//Reads pin output register value.
	return (this->base->PIO_ODSR & (1 << pin)) != 0;
}

void gpioPort_c::PinMode(uint32_t pin, uint32_t mode, uint32_t properties) {
	//Sets pin control registers for desired behaviour.
	// Relevant registers:
	// PIO_OER,  PIO_ODR,		output enable
	// PIO_MDER, PIO_MDDR,		open-drain enable
	// PIO_PUER, PIO_PUDR,		pullup enable
	// PIO_PPDER,PIO_PPDDR,		pulldown enable
	// PIO_PER,  PIO_PDR,		PIO enable (peripheral disable)
	// PIO_ABCDSR[2],			Peripheral selection
	// PIO_SCHMITT,				Schmitt-trigger input
	// PIO_IFSCDR, PIO_IFSCER,	Slowclock-input enable/disable
	
	uint32_t bitmask = 1 << pin;
	
	// Select mode - what's in control of the pin
	switch (mode) {
		case gpio_modeOutput:
			this->base->PIO_OER = 1 << pin;
			this->base->PIO_PER = bitmask;
			break;
		case gpio_modeInput:
			this->base->PIO_ODR = bitmask;
			this->base->PIO_PER = bitmask;
			break;
		case gpio_modePeriphA:
			this->base->PIO_ABCDSR[0] &= ~bitmask;
			this->base->PIO_ABCDSR[1] &= ~bitmask;
			this->base->PIO_PDR = bitmask;
			break;
		case gpio_modePeriphB:
			this->base->PIO_ABCDSR[0] |= bitmask;
			this->base->PIO_ABCDSR[1] &= ~bitmask;
			this->base->PIO_PDR = bitmask;
			break;
		case gpio_modePeriphC:
			this->base->PIO_ABCDSR[0] &= ~bitmask;
			this->base->PIO_ABCDSR[1] |= bitmask;
			this->base->PIO_PDR = bitmask;
			break;
		case gpio_modePeriphD:
			this->base->PIO_ABCDSR[0] |= bitmask;
			this->base->PIO_ABCDSR[1] |= bitmask;
			this->base->PIO_PDR = bitmask;
			break;
	}
	
	// Select properties - the pin's electrical behaviour
	if (properties & gpio_propPullup) {
		this->base->PIO_PPDDR = bitmask;
		this->base->PIO_PUER = bitmask;
	}
	else if (properties & gpio_propPulldown) {
		this->base->PIO_PUDR = bitmask;
		this->base->PIO_PPDER = bitmask;
	}
	else { // No pullup/pulldown
		this->base->PIO_PUDR = bitmask;
		this->base->PIO_PPDDR = bitmask;
	}
	
	if (properties & gpio_propSchmittTrig) {
		this->base->PIO_SCHMITT |= bitmask;
	}
	else {
		this->base->PIO_SCHMITT &= ~bitmask;
	}
	
	if (properties & gpio_propDebounceFilter) {
		this->base->PIO_IFSCER = bitmask;
	}
	else {
		this->base->PIO_IFSCDR = bitmask;
	}
}

// Global definiton:
gpioPort_c samGPIOA('A');
gpioPort_c samGPIOB('B');