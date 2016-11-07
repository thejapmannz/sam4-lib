/*
 * samUSART.hpp
 * API for synchronous-asynchronous serial peripheral. Based on Arduino style.
 *
 * Created: 2/07/2016 5:55:22 PM
 *  Author: Ben Jones
 */ 

/* This implementation is far from complete; the USART contains a LOT of options. 
   Only the most commonly used options are implemented in code. */
/* Consult the datasheet for information on what the following options are doing. 
   The pinout meanings also change depending on the mode of operation.*/

//If compiler issues are met, copy the following into your files after the #include:
// extern samUSART_c samUSART0;
// extern samUSART_c samUSART1;


#ifndef SAMUSART_HPP_
#define SAMUSART_HPP_

#include "sam.h"
#include "../Utilities/CircBuf.hpp"
#include "../Utilities/serial-funcs.hpp"


// Defined options for function arguments:
enum {usart_modeSerialSynch, usart_modeSerialAsync, usart_modeManchester};


//Use options from only one group of the following, as relevant for your selected mode:

//For synchronous serial mode:
enum {usart_optionSynchClockLocal = 0x00, usart_optionSynchClockRemote = 0x01,
	usart_optionSynchMSBFirst = 0x10,
	usart_optionSynchInvertData = 0x20};

//For asynchronous serial (UART) mode:
enum {usart_optionAsync8xSample = 0x10, // 8 samples per bit instead of 16. Higher bauds => use synch mode.
	usart_optionAsyncMSBFirst = 0x20,
	usart_optionAsyncInvertData = 0x40,
	usart_optionAsyncParityNone = 0x04, usart_optionAsyncParityEven = 0x00, usart_optionAsyncParityOdd = 0x01, usart_optionAsyncParityMark = 0x03, usart_optionAsyncParitySpace = 0x02};

//For Manchester mode:
enum {usart_optionManchesterPositive = 0x01, usart_optionManchesterNegative = 0x02};


class samUSART_c: public SerialStream {
	public:
		//Initialiser: sets up peripheral. Use bitwise OR for multiple options.
		void Begin(uint32_t mode, uint32_t baud_clockrate, uint32_t options);
		
		//Write things to internal buffer:
		void Write(uint8_t byte);
		
		//Check if byte available, and Read a byte(s):
		uint32_t Available(void);
		int16_t Read(void); // Note returns -1 if receive buffer empty.
		
		//Updater - called as interrupt handler, but can be polled also.
		void Update(void);
		
		//Constructor - allows instances for each peripheral. Not for general use.
		samUSART_c(int id);
	
	private:
		//Single read or write from registers:
		void usartWrite(uint32_t data);
		uint32_t usartRead(void);
		
		//Checks interrupt register for status.
		bool usartReadReady(void);
		bool usartWriteReady(void);
		
		//State variables:
		int ch_id;
		Usart* base;
		CircBuf_c<uint8_t> recieveBuffer;
		CircBuf_c<uint8_t> transmitBuffer;
	
};

#include "samUSART.cpp"

//Global declarations:
extern samUSART_c samUSART0;
extern samUSART_c samUSART1;

#endif /* SAMUSART_HPP_ */