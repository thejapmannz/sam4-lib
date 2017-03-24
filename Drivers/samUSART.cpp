/*
 * samUSART.cpp
 * API for synchronous-asynchronous serial peripheral. Based on Arduino style.
 *
 * Created: 2/07/2016 5:13:07 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"
#include "samClock.hpp"


//Initialiser: sets up peripheral. Use bitwise OR for multiple options.
void samUSART_c::Begin(uint32_t mode, uint32_t baud_clockrate, uint32_t parity, uint32_t options) {
	
	//Disable and reset:
	this->base->US_IDR = ~0UL; // Disable all interrupts
	this->base->US_MR = 0;
	this->base->US_CR = US_CR_RSTRX | US_CR_RSTTX | US_CR_RXDIS | US_CR_TXDIS | US_CR_RSTSTA;
	this->base->US_BRGR = 0;
	
	
	// note: Fractional clock divider available, but not used here.
	uint32_t clockDivider = samClock.MasterFreqGet() / baud_clockrate; 
	
	switch (mode) {
		case usart_modeSerialAsync: // Set up to be a UART.
			//Clock generator: need 16x oversampling from baudrate. Faster speeds => use synch mode.
			clockDivider /= 16;
			
			//Options:
			if (options & usart_optionAsyncMSBFirst) {
				this->base->US_MR |= US_MR_MSBF;
			}
			if (options & usart_optionAsyncInvertData) {
				this->base->US_MR |= US_MR_INVDATA;
			}
			
			//Mode register: Asynchronous, internal clock for all, 8-bit for UART.
			this->base->US_MR |= US_MR_USART_MODE_NORMAL | US_MR_CHRL_8_BIT | US_MR_USCLKS_MCK | 
				US_MR_PAR(parity) | US_MR_NBSTOP_1_BIT;
				
			break;
		
		case usart_modeSerialSynch: // Set up as synchronous serial.
		
			//Clock generator: Local at baudrate, or remote. 
			if (options & usart_optionSynchClockRemote) {
				this->base->US_MR |= US_MR_USCLKS_SCK; // Select external clock
				clockDivider = 0; // Don't want local clock running.
			}
			else {
				// Select local clock and enable clock output.
				this->base->US_MR |= US_MR_USCLKS_MCK | US_MR_CLKO;
			}
			
			//Options:
			if (options & usart_optionSynchMSBFirst) {
				this->base->US_MR |= US_MR_MSBF;
			}
			if (options & usart_optionSynchInvertData) {
				this->base->US_MR |= US_MR_INVDATA;
			}
			
			//Mode register: Synchronous, 8-bit for UART, external/internal clock.
			this->base->US_MR |= US_MR_USART_MODE_NORMAL | US_MR_CHRL_8_BIT | US_MR_SYNC | 
				US_MR_PAR(parity) | US_MR_NBSTOP_1_BIT;
				
			break;
		
		case usart_modeManchester: // Manchester encoded signal (clockless).
			uint32_t parameter;
			
			//Clock generator: use 16x oversampling, and drift compensation!
			clockDivider /= 16;
			this->base->US_MAN |= US_MAN_DRIFT | US_MAN_ONE;
			
			//Preamble:
			parameter = (options & 0xf00) >> 2;
			this->base->US_MAN |= US_MAN_TX_PL(parameter) | US_MAN_RX_PL(parameter);
			parameter = (options & 0xf0) >> 1;
			this->base->US_MAN |= US_MAN_TX_PP(parameter) | US_MAN_RX_PP(parameter);
			
			//Polarity:
			if (options & usart_optionManchInvertPolarity)
				this->base->US_MAN |= US_MAN_TX_MPOL | US_MAN_RX_MPOL;
			if (options & usart_optionManchInvertStartbit)
				this->base->US_MR |= US_MR_MODSYNC;
			
			//USART main register:
			this->base->US_MR |= US_MR_USART_MODE_NORMAL | US_MR_CHRL_8_BIT | 
				US_MR_PAR(parity) | US_MR_NBSTOP_1_BIT | US_MR_MAN | US_MR_ONEBIT;
			
			break;
			
		default:
			break; // Don't set up anything.
	}
	
	//Baud rate generator and clock:
	samClock.PeriphClockEnable(this->ch_id ? ID_USART1 : ID_USART0);
	this->base->US_BRGR |= US_BRGR_CD(clockDivider);
	
	//Set up and enable interrupts:
	this->base->US_IER = US_IER_RXRDY; // Enable receive interrupts - transmit enabled when necessary.
	if (this->ch_id)
		NVIC_EnableIRQ(USART1_IRQn); // Enable USART interrupts.
	else
		NVIC_EnableIRQ(USART0_IRQn);
	
	//Enable Tx and Rx, reset any errors:
	this->base->US_CR = US_CR_RXEN | US_CR_TXEN | US_CR_RSTSTA;
}


//Updater: manages peripheral and buffers. Later: call as interrupt handler.
void samUSART_c::Update(void) {
	//Moves data from buffer to hardware registers, as appropriate.
	if (this->usartReadReady()) {
		this->recieveBuffer.Push(this->usartRead());
	}
	
	if (this->usartWriteReady() && this->transmitBuffer.Available()) {
		this->usartWrite(this->transmitBuffer.Pop());
	}
	else { // Don't want spontaneous interrupts.
		this->base->US_IDR = US_IDR_TXRDY;
	}
}

//Write a single byte to internal buffer.
void samUSART_c::Write(uint8_t byte) {
	this->transmitBuffer.Push(byte);
	
	//Enable interrupts, to call updater:
	this->base->US_IER = US_IER_TXRDY;
}
//Check if data has been received:
uint32_t samUSART_c::Available(void) {
	return this->recieveBuffer.Available();
}
//Read a single byte from internal buffer.
int16_t samUSART_c::Read(void) {
	if (this->recieveBuffer.Available())
		return this->recieveBuffer.Pop();
	else
		return -1;
}


//Constructor - allows instances for each peripheral. Not for general use.
samUSART_c::samUSART_c(int id) : ch_id(id) 
{
	if (id) {
		this->base = USART1;
	}
	else {
		this->base = USART0;
	}
}


//Single read or write from registers:
void samUSART_c::usartWrite(uint32_t data) {
	this->base->US_THR = US_THR_TXCHR(data);
}
uint32_t samUSART_c::usartRead(void) {
	return this->base->US_RHR & US_RHR_RXCHR_Msk;
}

//Check interrupt status register to see if ready to read/write:
bool samUSART_c::usartWriteReady(void) {
	return (this->base->US_CSR & US_CSR_TXRDY) != 0;
}
bool samUSART_c::usartReadReady(void) {
	return (this->base->US_CSR & US_CSR_RXRDY) != 0;
}



//Global definitions:
samUSART_c samUSART0(0);
samUSART_c samUSART1(1);

void USART0_Handler(void) {
	samUSART0.Update();
}
void USART1_Handler(void) {
	samUSART1.Update();
}


