/*
 * samUSART.h
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


#ifndef SAMUSART_H_
#define SAMUSART_H_

#include "sam.h"
#include "CircBuf.h"
#include "serial-funcs.h"

//Parameters and options for functions:
enum {usart_modeSerialSynch, usart_modeSerialAsync, usart_modeManchester};

//Use options from only one group of the following, as relevant for your selected mode:

enum {usart_optionSynchClockLocalAlways = 0x01, usart_optionSynchClockLocalTx = 0x02, usart_optionSynchClockRemote = 0x04, usart_optionSynchMSBFirst = 0x08};

enum {usart_optionAsync8xSample = 0x10, 
	usart_optionAsyncMSBFirst = 0x20, 
	usart_optionAsyncParityNone = 0x04, usart_optionAsyncParityEven = 0x00, usart_optionAsyncParityOdd = 0x01, usart_optionAsyncParityMark = 0x03, usart_optionAsyncParitySpace = 0x02};

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
		CircBuf_c recieveBuffer;
		CircBuf_c transmitBuffer;
	
};

#include "samUSART.cpp"

//Global declarations:
extern samUSART_c samUSART0;
extern samUSART_c samUSART1;

#endif /* SAMUSART_H_ */