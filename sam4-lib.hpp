/*
 * sam4-lib.hpp
 * Header file to include all drivers and utilities written for the SAM4S. 
 *
 * This doesn't initialise any of the peripherals, so will (probably) cooperate with 
 *  other driver libraries for the same peripherals. 
 *
 * The files listed here can also be included one-by-one if code size becomes an issue.
 *
 * Created: 04/11/2016
 * Author : Ben Jones
 */ 


#ifndef SAM4_LIB_HPP
#define SAM4_LIB_HPP

//////////////////////////////////////////////////////////////////////////
//Include Atmel-provided register definitions:
#include "sam.h"

//////////////////////////////////////////////////////////////////////////
//Include drivers - one for each peripheral.
#include "Drivers/samADC.h"				// Analog-to-Digital converter
#include "Drivers/samClock.h"			// CPU peripheral clock generator, and delay functions.
#include "Drivers/samGPIO.h"			// GPIO pins, and peripheral output enables
#include "Drivers/samI2C.h"				// I2C serial bus
#include "Drivers/samPWM.h"				// Pulse Width Modulation
#include "Drivers/samSystick.h"			// ARM built-in SysTick timer
#include "Drivers/samUART.h"			// Universal Asynchronous Receive and Transmit - simple serial.
#include "Drivers/samUSART.h"			// Powerful serial peripheral - (a)synchronous, Manchester, SPI.

//The drivers define the following instances. See example code in README.md for typical usage.
// extern samADC_c samADC;
// extern samClock_c samClock;
// extern gpioPort_c gpioA;
// extern gpioPort_c gpioB;
// extern samI2C_c samTWI0;
// extern pwmCore_c pwmCore;
// extern pwmChannel_c pwmChannel0;
// extern pwmChannel_c pwmChannel1;
// extern pwmChannel_c pwmChannel2;
// extern pwmChannel_c pwmChannel3;
// extern samSysTick_c samSysTick;
// extern samUART_c samUART0;
// extern samUART_c samUART1;
// extern samUSART_c samUSART0;
// extern samUSART_c samUSART1;

//////////////////////////////////////////////////////////////////////////
//Include utilities - Software wrappers on top of drivers. 
#include "Utilities/arduino-funcs.h"	// Some of the common Arduino functions e.g. map
#include "Utilities/CircBuf.h"			// Circular buffer class
#include "Utilities/samServo.h"			// Arduino style servo wrapper for PWM peripheral.
//#include "Utilities/serial-funcs.h"	// Private. Used by UART and USART for printf, scanf implementation.


#endif //SAM4_LIB_HPP