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
			gpioPort_c::base = PIOA;
			gpioPort_c::port_id = ID_PIOA;
			break;
		case 'B':
			gpioPort_c::base = PIOB;
			gpioPort_c::port_id = ID_PIOB;
			break;
#ifdef PIOC
		case 'C':
			gpioPort_c::base = PIOC;
			gpioPort_c::port_id = ID_PIOC;
			break;
#endif
	}
}

void gpioPort_c::Begin(void) {
	//Enables peripheral clock to GPIO.
	samClock.PeriphClockEnable(gpioPort_c::port_id);
}

void gpioPort_c::PinSet(uint32_t pin, bool state) {
	//Sets or clears state of given pin.
	if (state) gpioPort_c::base->PIO_SODR = 1 << pin;
	else gpioPort_c::base->PIO_CODR = 1 << pin;
}
void gpioPort_c::PinSetHigh(uint32_t pin) {
	gpioPort_c::base->PIO_SODR = 1 << pin;
}
void gpioPort_c::PinSetLow(uint32_t pin) {
	gpioPort_c::base->PIO_CODR = 1 << pin;
}


bool gpioPort_c::PinRead(uint32_t pin) {
	//Reads pin value.
	return (gpioPort_c::base->PIO_PDSR & (1 << pin)) != 0;
}
bool gpioPort_c::PinReadSetVal(uint32_t pin) {
	//Reads pin output register value.
	return (gpioPort_c::base->PIO_ODSR & (1 << pin)) != 0;
}

void gpioPort_c::PinMode(uint32_t pin, uint32_t mode) {
	//Sets pin control registers for desired mode.
	// Relevant registers:
	// PIO_OER,  PIO_ODR,  output enable
	// PIO_MDER, PIO_MDDR, open-drain enable
	// PIO_PUER, PIO_PUDR, pullup enable
	// PIO_PPDER,PIO_PPDDR,pulldown enable
	// PIO_PER,  PIO_PDR   PIO enable (peripheral disable)
	// PIO_ABCDSR[2],      Peripheral selection
	
	uint32_t bitmask = 1 << pin;
	
	switch (mode) {
		case gpioOutput:
			gpioPort_c::base->PIO_MDDR = 1 << pin;
			gpioPort_c::base->PIO_OER = 1 << pin;
			gpioPort_c::base->PIO_PER = bitmask;
			break;
		case gpioOutputOpenDrain:
			gpioPort_c::base->PIO_MDER = bitmask;
			gpioPort_c::base->PIO_OER = bitmask;
			gpioPort_c::base->PIO_PER = bitmask;
			break;
		case gpioInput:
			gpioPort_c::base->PIO_ODR = bitmask;
			gpioPort_c::base->PIO_PUDR = bitmask;
			gpioPort_c::base->PIO_PPDDR = bitmask;
			gpioPort_c::base->PIO_PER = bitmask;
			break;
		case gpioInputPullup:
			gpioPort_c::base->PIO_ODR = bitmask;
			gpioPort_c::base->PIO_PPDDR = bitmask;
			gpioPort_c::base->PIO_PUER = bitmask;
			gpioPort_c::base->PIO_PER = bitmask;
			break;
		case gpioInputPulldown:
			gpioPort_c::base->PIO_ODR = bitmask;
			gpioPort_c::base->PIO_PUDR = bitmask;
			gpioPort_c::base->PIO_PPDER = bitmask;
			gpioPort_c::base->PIO_PER = bitmask;
			break;
		case gpioPeriphA:
			gpioPort_c::base->PIO_ABCDSR[0] &= ~bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] &= ~bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUDR = bitmask;
			break;
		case gpioPeriphAPullup:
			gpioPort_c::base->PIO_ABCDSR[0] &= ~bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] &= ~bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUER = bitmask;
			break;
		case gpioPeriphB:
			gpioPort_c::base->PIO_ABCDSR[0] |= bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] &= ~bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUDR = bitmask;
			break;
		case gpioPeriphBPullup:
			gpioPort_c::base->PIO_ABCDSR[0] |= bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] &= ~bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUER = bitmask;
			break;
		case gpioPeriphC:
			gpioPort_c::base->PIO_ABCDSR[0] &= ~bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] |= bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUDR = bitmask;
			break;
		case gpioPeriphCPullup:
			gpioPort_c::base->PIO_ABCDSR[0] &= ~bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] |= bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUER = bitmask;
			break;
		case gpioPeriphD:
			gpioPort_c::base->PIO_ABCDSR[0] |= bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] |= bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUDR = bitmask;
			break;
		case gpioPeriphDPullup:
			gpioPort_c::base->PIO_ABCDSR[0] |= bitmask;
			gpioPort_c::base->PIO_ABCDSR[1] |= bitmask;
			gpioPort_c::base->PIO_PDR = bitmask;
			gpioPort_c::base->PIO_PUER = bitmask;
			break;
	}
	
}

// Global definiton:
gpioPort_c gpioA('A');
gpioPort_c gpioB('B');