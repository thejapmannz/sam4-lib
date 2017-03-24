/*
 * samUSART.hpp
 * API for synchronous-asynchronous serial peripheral. Based on Arduino style.
 *
 * This implementation is far from complete; the USART contains a LOT of options. 
 * Only the most commonly used protocols are implemented in code.
 *
 * TODO list:
 *     - Add SPI mode support
 *     - Add hardware handshaking (CTS/RTS) to serial modes.
 * 
 * Created: 2/07/2016 5:55:22 PM
 *  Author: Ben Jones
 */


#ifndef SAMUSART_HPP_
#define SAMUSART_HPP_

#include "sam.h"
#include "../Utilities/CircBuf.hpp"
#include "../Utilities/serial-funcs.hpp"


#define USART_BUFF_LENGTH 256

// Defined options for function arguments:
enum {usart_modeSerialSynch, usart_modeSerialAsync, usart_modeManchester};

enum {usart_parityNone = 0x04, usart_parityEven = 0x00, usart_parityOdd = 0x01, usart_parityMark = 0x03, usart_paritySpace = 0x02};


// Consult the datasheet for information on what the following options are doing. 
//   The pinout meanings also change depending on the mode of operation.
//Use options from only one group of the following, as relevant for your selected mode:

//For synchronous serial mode:
enum {usart_optionSynchClockLocal = 0x00, usart_optionSynchClockRemote = 0x01,
	usart_optionSynchMSBFirst = 0x10,
	usart_optionSynchInvertData = 0x20};

//For asynchronous serial (UART) mode:
enum {usart_optionAsyncMSBFirst = 0x10,
	usart_optionAsyncInvertData = 0x20};

//For Manchester mode:
enum {usart_optionManchInvertPolarity = 0x01, usart_optionManchInvertStartbit = 0x02, 
	usart_optionManchPreambleAllOne = 0x00, usart_optionManchPreambleAllZero = 0x10, usart_optionManchPreambleZeroOne = 0x20, usart_optionManchPreambleOneZero = 0x30};
#define usart_optionManchPreambleLength(length) (length & 0xf) << 2


class samUSART_c: public SerialStream {
	public:
		//Initialiser: sets up peripheral. Use bitwise OR for multiple options.
		void Begin(uint32_t mode, uint32_t baud_clockrate, uint32_t parity, uint32_t options);
		
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
		CircBuf_c<uint8_t, USART_BUFF_LENGTH> recieveBuffer;
		CircBuf_c<uint8_t, USART_BUFF_LENGTH> transmitBuffer;
	
};

#include "samUSART.cpp"

//Global declarations:
extern samUSART_c samUSART0;
extern samUSART_c samUSART1;

#endif /* SAMUSART_HPP_ */