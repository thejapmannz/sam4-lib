/*
 * samUART.cpp
 *
 * Created: 8/05/2016 12:46:10 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samClock.h"
extern samClock_c samClock;

samUART_c::samUART_c(int id) {
	samUART_c::channel_id = id;
	if (id) {
		samUART_c::base_id = UART1;
	}
	else {
		samUART_c::base_id = UART0;
	}
}

void samUART_c::Begin(uint32_t baud, uint32_t parity) {
	//Initialise UART, with given data parameters.
	
	//Reset and disable interrupts::
	this->base_id->UART_CR = UART_CR_RSTRX | UART_CR_RSTTX | UART_CR_RXDIS | UART_CR_TXDIS;
	this->base_id->UART_IDR = ~0;
	
	//Enable peripheral clock:
	samClock.PeriphClockEnable(ID_UART0 + samUART_c::channel_id);
	//Set up UART:
	this->base_id->UART_MR = UART_MR_PAR(parity) | UART_MR_CHMODE(UART_MR_CHMODE_NORMAL);
	//Set up baud rate generator:
	uint16_t baudDivider = samClock.MasterFreqGet() / (16 * (uint64_t)baud);
	this->base_id->UART_BRGR = UART_BRGR_CD(baudDivider);
	//Reset, then enable RX and TX:
	this->base_id->UART_CR |= UART_CR_RXEN | UART_CR_TXEN;
}

bool samUART_c::uartReadReady(void) {
	//Checks read-ready bit.
	return (this->base_id->UART_SR & UART_SR_RXRDY) != 0;
}
uint8_t samUART_c::uartRead(void) {
	//Gets a byte from the receive register.
	uint8_t character = samUART_c::base_id->UART_RHR & 0xff;
	//Reset error flags:
	this->base_id->UART_CR |= UART_CR_RSTSTA;
	return character;
}
bool samUART_c::uartWriteReady(void) {
	//Checks write-ready bit.
	return (this->base_id->UART_SR & UART_SR_TXRDY) != 0;
}
void samUART_c::uartWrite(uint8_t byte) {
	//Write a byte to transmit register.
	this->base_id->UART_THR = byte;
}

uint32_t samUART_c::Available(void) {
	//Returns number of bytes available to read.
	return this->recieveBuffer.Available();
}

void samUART_c::Write(uint8_t byte) {
	//Write a byte into internal buffer.
	this->transmitBuffer.Push(byte);
}
void samUART_c::WriteStr(char buffer[], uint32_t num_bytes) {
	//Writes a whole string to internal buffer, to be sent.
	for (uint32_t i = 0; i < num_bytes; i++) {
		this->Write(buffer[i]);
	}
}
void samUART_c::PrintInt(int64_t value) {
	// Sends a number to UART, as we would read it.
	uint32_t digit = 0;
	if (value < 0) {
		this->Write('-');
		value = -value;
	}
	for (int32_t i = 10; i >= 0; i--) { // Max 2x10^10 for 32-bit.
		digit = value;
		for (int32_t j = 0; j < i; j++) {
			digit /= 10;
		}
		digit %= 10;
		this->Write(digit + '0');
	}
}
void samUART_c::PrintBits(uint32_t value) {
	//Sends a number to UART, as 32 bits.
	for (int i = 31; i >= 0; i--) {
		this->Write((value & (1 << i)) ? '1' : '0');
	}
}
uint8_t samUART_c::Read(void) {
	//Returns a byte from the internal buffer.
	return this->recieveBuffer.Pop();
}
void samUART_c::ReadStr(char buffer[], uint32_t num_bytes) {
	//Writes a whole string to internal buffer, to be sent.
	for (uint32_t i = 0; i < num_bytes; i++) {
		this->Write(buffer[i]);
	}
}

void samUART_c::Update(void) {
	//Updates UART, depending on read-ready, write-ready, etc.
	
	if (this->transmitBuffer.Available() > 0 && this->uartWriteReady()) {
		this->uartWrite(this->transmitBuffer.Pop());
	}
	if (this->uartReadReady()) {
		this->recieveBuffer.Push(this->uartRead());
	}
}

// Global definition:
samUART_c samUART0(0);
samUART_c samUART1(1);