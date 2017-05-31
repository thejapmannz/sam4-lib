/*
 * samUART.hpp
 *
 * Created: 8/05/2016 12:46:29 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMUART_HPP_
#define SAMUART_HPP_

#include "sam.h"
#include "../Utilities/CircBuf.hpp"
#include "../Utilities/serial-funcs.hpp"

#define UART_BUFF_LENGTH 256

enum {uart_parityEven, uart_parityOdd, uart_parityMark, uart_paritySpace, uart_parityNone};

class samUART_c: public SerialStream {
	public:
		//Initialiser - baud rate, parity.
		void Begin(uint32_t baud, uint32_t parity);
		
		//Access internal buffer to send/recieve bytes. 
		// I recommend using the functions in the SerialStream class instead!
		uint32_t Available(void); // How many bytes in buffer
		int16_t Read(void); // Note returns -1 if receive buffer empty.
		int16_t Peek(void); // Same as read but doesn't consume data.
		void Write(uint8_t byte); // Write a byte to the internal buffer
		
		
		//Updater - called as interrupt handler, but can be polled also.
		void Update(void);
		//constructor - one instance for UART0 and UART1.
		samUART_c(int id);
		
	private:
		bool uartWriteReady(void);
		void uartWrite(uint8_t byte);
		int16_t uartRead(void);
		bool uartReadReady(void);
		
		bool channel_id; // Channel can be 0 or 1 on SAM4S.
		Uart* base_id; // Base address for peripheral.
		CircBuf_c<uint8_t, UART_BUFF_LENGTH> recieveBuffer;
		CircBuf_c<uint8_t, UART_BUFF_LENGTH> transmitBuffer;
};

#include "samUART.cpp"

// Global declarations:
extern samUART_c samUART0;
extern samUART_c samUART1;

#endif /* SAMUART_HPP_ */
