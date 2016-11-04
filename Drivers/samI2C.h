/*
 * samI2C.h - TWI/I2C peripheral driver.
 * Main routines copied from Atmel C files, and 
 * rewritten into C++ format.
 *
 * Created: 14/05/2016 4:16:32 PM
 *  Author: Ben Jones
 */ 


#ifndef SAMI2C_H_
#define SAMI2C_H_

// Instances are samTWI0 and samTWI1.

//TWI's are on the following pins:
//	TWI:	Pin:	Peripheral:
//	TWCK0	PA4		A
//	TWD0	PA3		A
//	TWCK1	PB5		A
//	TWD1	PB4		A

#include "sam.h"

//Size of internal RAM buffer:
#define TWI_BUFFER_SIZE 256

//Return values for functions.
#define TWI_DONE                  3
#define TWI_SLAVE_READ            2 // Master performing a read, we write.
#define TWI_SLAVE_WRITE           1 // Master performing a write, we read.
#define TWI_IDLE                  0
#define TWI_SUCCESS               0
#define TWI_INVALID_ARGUMENT     -1
#define TWI_ARBITRATION_LOST     -2
#define TWI_NO_CHIP_FOUND        -3
#define TWI_ERROR_OVERRUN        -4
#define TWI_ERROR_NACK           -5
#define TWI_BUSY                 -6
#define TWI_ERROR_TIMEOUT        -7
#define TWI_ERROR_MODE           -8
#define TWI_ERROR_WRITE_EXPECTED -9
#define TWI_ERROR_READ_EXPECTED  -10

class samI2C_c {
	public:
		//Constructor (not for general use):
		samI2C_c(int channelNr);
		
		//Initialiser: master/slave, host address, speed (kHz).
		enum{mode_Master, mode_MultiMaster, mode_Slave};
		void Begin(uint32_t mode, uint8_t hostAddress, uint32_t speed_kHz);
		
		//Master mode: Read/write byte from address, blocking.
		int32_t MasterReadBytes(uint8_t slaveAddress, uint32_t internalAddress, int16_t intlAddrLen, uint8_t buffer[], uint16_t numBytes);
		int32_t MasterWriteBytes(uint8_t slaveAddress, uint32_t internalAddress, int16_t intlAddrLen, uint8_t buffer[], uint16_t numBytes);
		
		//Slave mode: Poll bus for status.
		int32_t SlavePoll(void);
		//Slave mode: Update, handles RAM buffer and master accesses.
		//  Call at approx 1 to 10 kHz.
		int32_t SlaveUpdate(void);
		//FUnctions to access internal slave data. 
		uint8_t SlaveDataRead(uint16_t addr);
		void SlaveDataWrite(uint16_t addr, uint8_t byte);
		
		// Slave mode: Read/write from local buffer.
		int32_t SlaveWriteBytes(uint8_t buffer[], uint16_t maxBytes);
		int32_t SlaveReadBytes(uint8_t buffer[], uint16_t maxBytes, bool ReadExtraData);
		
		
	private:
		void Reset(void);
		void clockSetup(uint32_t freq_kHz);
		uint8_t addressLength(uint32_t address);
		
		int32_t MasterWaitAck(bool writing);
		int32_t MasterWaitTxcomp(void);
		int32_t SlaveReadWait(void);
		int32_t SlaveWriteWait(void);
		
		int channelNumber;
		uint32_t mode;
		Twi* TWIbase;
		// Polled RAM buffer. Later: interrupt-based RAM buffer.
		uint8_t InternalData[TWI_BUFFER_SIZE];
};

#include "samI2C.cpp"

//Global declarations:
extern samI2C_c samTWI0;
extern samI2C_c samTWI1;

#endif /* SAMI2C_H_ */