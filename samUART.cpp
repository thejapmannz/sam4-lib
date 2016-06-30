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
	this->base_id->UART_IDR = ~0UL;
	
	//Enable peripheral clock:
	samClock.PeriphClockEnable(ID_UART0 + samUART_c::channel_id);
	
	//Set up UART:
	this->base_id->UART_MR = UART_MR_PAR(parity) | UART_MR_CHMODE(UART_MR_CHMODE_NORMAL);
	//Set up baud rate generator:
	uint16_t baudDivider = samClock.MasterFreqGet() / (16 * (uint64_t)baud);
	this->base_id->UART_BRGR = UART_BRGR_CD(baudDivider);
	
	//Set up and enable interrupts:
	this->base_id->UART_IER = UART_IER_RXRDY; // Enable receive interrupts - transmit enabled when necessary.
	if (this->channel_id)
		NVIC_EnableIRQ(UART1_IRQn); // Enable UART interrupts.
	else
		NVIC_EnableIRQ(UART0_IRQn);
	
	//Enable RX and TX:
	this->base_id->UART_CR |= UART_CR_RXEN | UART_CR_TXEN;
}

bool samUART_c::uartReadReady(void) {
	//Checks read-ready bit.
	return (this->base_id->UART_SR & UART_SR_RXRDY) != 0;
}
int16_t samUART_c::uartRead(void) {
	//Gets a byte from the receive register.
	
	int16_t character = this->base_id->UART_RHR & 0xff;
	
	if (this->base_id->UART_SR & (UART_SR_PARE | UART_SR_FRAME)) {//If error detected.
		character = -1; // Error code.
	}
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
	
	//Enable transmit interrupts, to call update function.
	UART1->UART_IER = UART_IER_TXRDY;
}
void samUART_c::WriteStr(char buffer[], uint32_t num_bytes) {
	//Writes a whole string to internal buffer, to be sent.
	for (uint32_t i = 0; i < num_bytes; i++) {
		this->Write(buffer[i]);
	}
}
void samUART_c::PrintInt(int64_t value) {
	// Sends a number to UART, as we humans would read it.
	uint32_t digit = 0;
	if (value < 0) {
		this->Write('-');
		value = -value;
	}
	
	bool started = 0;
	for (int32_t i = 10; i >= 0; i--) { // Max 2x10^10 for 32-bit.
		digit = value;
		for (int32_t j = 0; j < i; j++) { // No power function available.
			digit /= 10;
		}
		digit %= 10;
		if (digit) {
			started = 1;
		}
		if (started || (i == 0)) {
			this->Write(digit + '0');
		}
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
	//Reads a whole string from internal receive buffer.
	for (uint32_t i = 0; i < num_bytes; i++) {
		buffer[i] = this->Read();
	}
}

void samUART_c::Update(void) {
	//Updates UART, depending on read-ready, write-ready, etc.
	
	if (this->transmitBuffer.Available() > 0 && this->uartWriteReady()) {
		this->uartWrite(this->transmitBuffer.Pop());
	}
	else { // Don't want spontaneous interrupts.
		UART1->UART_IDR = UART_IER_TXRDY;
	}
	
	if (this->uartReadReady()) {
		uint16_t character = this->uartRead();
		if (character >= 0) { // No error detected.
			this->recieveBuffer.Push(character);
		}
	}
}

// Global definition:
samUART_c samUART0(0);
samUART_c samUART1(1);

// Interrupt handlers:
void UART0_Handler(void) {
	samUART0.Update();
}
void UART1_Handler(void) {
	samUART1.Update();
}