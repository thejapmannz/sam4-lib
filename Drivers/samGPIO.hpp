/*
 * samGPIO.hpp
 * Generic GPIO high/low/input/pullup driver, similar to Arduino style.
 *
 * Created: 4/05/2016 8:50:55 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMGPIO_HPP_
#define SAMGPIO_HPP_

#include "sam.h"

//Global instances are gpioA and gpioB (no port C on sam4s8b).

//The following options are available for the pinMode function:
enum {gpio_modeOutput, 
	gpio_modeInput, 
	gpio_modePeriphA, 
	gpio_modePeriphB, 
	gpio_modePeriphC, 
	gpio_modePeriphD
	};
//The following options are available for the pinMode function, 
//		and can be ORed together for multiple selections.
enum {gpio_propNormal = 0x00, 
	gpio_propPullup = 0x01, 
	gpio_propPulldown = 0x02, 
	gpio_propSchmittTrig = 0x04, 
	gpio_propDebounceFilter = 0x08 // Hardware debouncing using slow clock.
	};

class gpioPort_c {
	public:
		//Initialiser:
		gpioPort_c(char port);
		
		//Initialise port. Necessary to provide clock to input pins.
		void Begin(void);
		//Set pin behaviour, from options enumerated above.
		void PinMode(uint32_t pin, uint32_t mode, uint32_t properties);
		
		//Set output high/low, and faster inline functions.
		void PinSet(uint32_t pin, bool state);
		inline void PinSetHigh(uint32_t pin);
		inline void PinSetLow(uint32_t pin);
		
		//Get output state from pin or output registers
		bool PinRead(uint32_t pin);
		bool PinReadSetVal(uint32_t pin);
	
	private:
		char port_id;
		Pio* base;
};

#include "samGPIO.cpp"

// Global declaration:
extern gpioPort_c samGPIOA;
extern gpioPort_c samGPIOB;

#endif /* SAMGPIO_HPP_ */