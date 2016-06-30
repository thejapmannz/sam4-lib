/*
 * samUART.h
 *
 * Created: 8/05/2016 12:46:29 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMUART_H_
#define SAMUART_H_

#include "sam.h"
#include "CircBuf.h"

#define UART_BUFFER_SIZE 255

enum {uart_parityEven, uart_parityOdd, uart_parityMark, uart_paritySpace, uart_parityNone};

class samUART_c {
	public:
		//Initialiser - baud rate, parity.
		void Begin(uint32_t baud, uint32_t parity);
		//Updater, to be called at baud rate / 10 Hz.
		void Update(void);
		
		//Write things to internal buffer:
		void Write(uint8_t byte);
		void WriteStr(char buffer[], uint32_t num_bytes);
		//Write formatted things to internal buffer:
		void PrintInt(int64_t value);
		void PrintBits(uint32_t value);
		
		//Check if byte available, and Read a byte(s):
		uint32_t Available(void);
		uint8_t Read(void);
		void ReadStr(char buffer[], uint32_t num_bytes);
		
		//constructor - one instance for UART0 and UART1.
		samUART_c(int id);
		
	private:
		bool uartWriteReady(void);
		void uartWrite(uint8_t byte);
		int16_t uartRead(void);
		bool uartReadReady(void);
		
		bool channel_id; // Channel can be 0 or 1 on SAM4S.
		Uart* base_id; // Base address for peripheral.
		CircBuf_c recieveBuffer;
		CircBuf_c transmitBuffer;
};

#include "samUART.cpp"

// Global declarations:
extern samUART_c samUART0;
extern samUART_c samUART1;

#endif /* SAMUART_H_ */
