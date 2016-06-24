/*
 * samGPIO.h
 * Generic GPIO high/low/input/pullup driver, similar to Arduino style.
 *
 * Created: 4/05/2016 8:50:55 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMGPIO_H_
#define SAMGPIO_H_

#include "sam.h"

//Global instances are gpioA and gpioB (no port C on sam4s8b).

//The following options are available for the pinMode function:
enum {gpioOutput, gpioOutputOpenDrain, gpioInput, gpioInputPullup, gpioInputPulldown, 
		gpioPeriphA, gpioPeriphAPullup, gpioPeriphB, gpioPeriphBPullup, 
		gpioPeriphC, gpioPeriphCPullup, gpioPeriphD, gpioPeriphDPullup};

class gpioPort_c {
	public:
		//Initialiser:
		gpioPort_c(char port);
		//Start the GPIO clock:
		void Begin(void);
		//Set output high/low, and faster inline functions.
		void PinSet(uint32_t pin, bool state);
		inline void PinSetHigh(uint32_t pin);
		inline void PinSetLow(uint32_t pin);
		//Get output state from pin or output registers
		bool PinRead(uint32_t pin);
		bool PinReadSetVal(uint32_t pin);
		//Set pin driving state. From options enumerated above.
		void PinMode(uint32_t pin, uint32_t mode);
		
		char port_id;
		Pio* base;
};

#include "samGPIO.cpp"

// Global declaration:
extern gpioPort_c gpioA;
extern gpioPort_c gpioB;

#endif /* SAMGPIO_H_ */