/*
 * samI2C.cpp
 *
 * Created: 15/05/2016 3:05:10 PM
 *  Author: Ben Jones
 */ 


#include "sam.h"
#include "samClock.hpp"

/* General i2c info:

	The master controls the clock although the slave can stretch it
	when it can't keep up.

	A master write consists of the slave address, followed by the
	internal address, and the data payload.  Each byte needs to be
	acknowledged by the slave.

	A master read consists of a write followed by a read.  The write
	consists of the slave address followed by the internal address.  A
	repeated START is then sent followed by the slave address.  The
	master the reads the data payload from the slave.  The master
	acknowledges each byte and controls the number of bytes read by
	sending a STOP.
*/

#define I2C_FAST_MODE_SPEED  400000
#define TWI_CLK_DIV_MAX      0xFF
#define TWI_CLK_DIV_MIN      7
#define TWI_CLK_DIVIDER      2
#define TWI_TIMEOUT          150000


samI2C_c::samI2C_c(int channelNr) {
	samI2C_c::channelNumber = channelNr;
	if (channelNr) {
		TWIbase = TWI1;
	}
	else {
		TWIbase = TWI0;
	}
}

void samI2C_c::Begin(uint32_t mode, uint8_t hostAddress, uint32_t speed_kHz) {
	//Sets up TWI peripheral. 
	
	samI2C_c::TWIbase->TWI_IDR = ~0UL;/* Disable TWI interrupts */
	samI2C_c::Reset(); // Clear everything.
	
	// Set up clock generator.
	samI2C_c::clockSetup(speed_kHz * 1000);
	if (samI2C_c::channelNumber)
		samClock.PeriphClockEnable(ID_TWI1);
	else
		samClock.PeriphClockEnable(ID_TWI0);
			
	//Set up host address.
	samI2C_c::TWIbase->TWI_SMR = TWI_SMR_SADR(hostAddress>>1);
	
	//Set up master/slave:
	switch (mode) {
		case mode_Master:
			/* Set Master Disable bit and Slave Disable bit */
			samI2C_c::TWIbase->TWI_CR = TWI_CR_MSDIS;
			samI2C_c::TWIbase->TWI_CR = TWI_CR_SVDIS;
			/* Set Master Enable bit */
			samI2C_c::TWIbase->TWI_CR = TWI_CR_MSEN;
			break;
		case mode_MultiMaster:
			// None yet.
			break;
		case mode_Slave:
			/* Set Master Disable bit and Slave Disable bit */
			samI2C_c::TWIbase->TWI_CR = TWI_CR_MSDIS;
			samI2C_c::TWIbase->TWI_CR = TWI_CR_SVDIS;
			/* Set Slave Enable bit */
			samI2C_c::TWIbase->TWI_CR = TWI_CR_SVEN;
			break;
	}
	samI2C_c::mode = mode;
}

int32_t samI2C_c::MasterReadBytes(uint8_t slaveAddress, uint32_t internalAddress, int16_t intlAddrLen, uint8_t buffer[], uint16_t numBytes) {
	//Reads a byte from a specified slave. Blocking. Automatic internal address length with manual length option.
	//Routine copied/adapted from Michael Hayes' example code.
	
	if (samI2C_c::mode != mode_Master)
		return TWI_ERROR_MODE; // Can't write if not in master mode...
	
	uint8_t i;
    uint8_t *data = buffer;
    int32_t ret;

    if (numBytes == 0)
        return 0;
	 
	if (intlAddrLen < 0 || intlAddrLen > 3)
		intlAddrLen = samI2C_c::addressLength(internalAddress);
    /* The flowchart Fig 33-17 suggests this order for MMR and IADR.  */
	samI2C_c::TWIbase->TWI_MMR = TWI_MMR_DADR (slaveAddress >> 1) | TWI_MMR_IADRSZ(intlAddrLen) | TWI_MMR_MREAD;
    samI2C_c::TWIbase->TWI_IADR = internalAddress;

    if (numBytes == 1)
        samI2C_c::TWIbase->TWI_CR = TWI_CR_START | TWI_CR_STOP;
    else
        samI2C_c::TWIbase->TWI_CR = TWI_CR_START;

    /* The slave address and optional internal address is sent. 
       Each sent byte should be acknowledged.  See Figure 33-20
       for a flowchart.  */

    for (i = 0; i < numBytes; i++) {
        /* The master does not acknowledge receipt of the last byte.  */
        if ((i == numBytes - 1) && (numBytes > 1))
            samI2C_c::TWIbase->TWI_CR = TWI_CR_STOP;

        ret = samI2C_c::MasterWaitAck(false);
        if (ret < 0) {
            /* A STOP is automatically performed if we get a NACK.
               But what about a timeout, say while the clock is being
               stretched?  */
            return ret;
        }

        *data++ = samI2C_c::TWIbase->TWI_RHR;
    }

    ret = samI2C_c::MasterWaitTxcomp();
    if (ret < 0)
        return ret;

    /* Clear flags.  I'm not sure why!  */
    samI2C_c::TWIbase->TWI_SR;

    return i;
}

int32_t samI2C_c::MasterWriteBytes(uint8_t slaveAddress, uint32_t internalAddress, int16_t intlAddrLen, uint8_t buffer[], uint16_t numBytes) {
	//Writes a byte to a specified slave. Blocking. Automatic internal address length.
	
	if (samI2C_c::mode != mode_Master) 
		return TWI_ERROR_MODE; // Can't write if not in master mode...
	
	uint8_t i;
    uint8_t *data = buffer;
    int32_t ret;

    /* If addr size is zero, need to set start and stop bits at same
       time.  */
	if (intlAddrLen < 0 || intlAddrLen > 3)
	intlAddrLen = samI2C_c::addressLength(internalAddress);
    /* The flowchart Fig 33-17 suggests this order for MMR and IADR.  */
    samI2C_c::TWIbase->TWI_MMR = TWI_MMR_DADR (slaveAddress >> 1) | TWI_MMR_IADRSZ(intlAddrLen); // | TWI_MMR_MREAD;
    samI2C_c::TWIbase->TWI_IADR = internalAddress;

    /* Perhaps check that both the clock and data lines are high?  A
       common mistake is not to have pullup resistors for these
       lines.  */

    /* A START command is sent followed by the 7 bit slave address
       (MSB first) the read/write bit (0 for write, 1 for read), the
       acknowledge bit, then the optional internal address.  This is
       initiated by writing to THR.  Each of the sent bytes needs to
       be acknowledged by the slave.  There are two error scenarios 1)
       another master transmits at the same time with a higher
       priority 2) no slave responds to the desired address.
    */

    for (i = 0; i < numBytes; i++) {
        samI2C_c::TWIbase->TWI_THR = *data++;

        ret = samI2C_c::MasterWaitAck(true);
        if (ret < 0) {
            /* FIXME.  The datasheet does not say what to do here!
               The slave has not responded in time.  Perhaps we need
               to reset the controller to prevent bus being left in a
               weird state.  */
               
            return ret;
        }
    }

    /* Figure 33-16 says if there is a single data byte then write to
       THR, set TW_CR_STOP, check TXRDY, then check TXCOMP.  */

    samI2C_c::TWIbase->TWI_CR = TWI_CR_STOP;
    ret = samI2C_c::MasterWaitTxcomp();
    if (ret < 0)
        return ret;

    return i;
}

int32_t samI2C_c::SlavePoll(void) {
	//Checks registers to see what the bus master wants.
	uint32_t status;

	/* Ensure in slave mode.  */
	if (samI2C_c::mode != mode_Slave)
		return TWI_ERROR_MODE;

	status = samI2C_c::TWIbase->TWI_SR;

	/* SVACC is set when our address matches until a STOP or repeated START.  */
	if (! (status & TWI_SR_SVACC))
		return TWI_IDLE;

	/*  SVREAD is curiously named.  It is high when the master wants to read!  */
	if (status & TWI_SR_SVREAD)
		return TWI_SLAVE_READ;

	return TWI_SLAVE_WRITE;
}

int32_t samI2C_c::SlaveUpdate(void) {
	//Checks status of i2c bus, and sends/receives data as necessary.
	
	int32_t status = samI2C_c::SlavePoll();
	int32_t ret = TWI_SUCCESS;
	uint8_t addr = 0;
	switch (status) {
		case TWI_SLAVE_READ:
			ret = SlaveReadBytes(&addr, 1, false);
			if (ret > 0)
				ret = SlaveWriteBytes(samI2C_c::InternalData + (uint32_t)addr, TWI_BUFFER_SIZE - addr);
			break;
		case TWI_SLAVE_WRITE:
			ret = SlaveReadBytes(&addr, 1, false);
			if (ret > 0)
				ret = SlaveReadBytes(samI2C_c::InternalData + (uint32_t)addr, TWI_BUFFER_SIZE - addr, true);
// 				samUART1.PrintInt(addr);
// 				samUART1.WriteStr(" : ", 3);
// 				samUART1.PrintInt(samI2C_c::InternalData[addr]);
// 				samUART1.WriteStr(" : ", 3);
// 				samUART1.PrintInt(ret);
// 				samUART1.Write('\n');
			break;
		default:
			return status;
	}
	return ret;
}
uint8_t samI2C_c::SlaveDataRead(uint16_t addr) {
	//Returns buffer value at addr.
	return samI2C_c::InternalData[addr];
}
void samI2C_c::SlaveDataWrite(uint16_t addr, uint8_t byte) {
	//Writes buffer value at addr.
	samI2C_c::InternalData[addr] = byte;
}

int32_t samI2C_c::SlaveWriteBytes(uint8_t buffer[], uint16_t maxBytes) {
	//Writes bytes to the master.
	
	uint8_t i;
    uint8_t *data = buffer;
    int32_t ret;

    ret = samI2C_c::SlavePoll();
    if (ret <= 0)
        return ret;
    if (ret != TWI_SLAVE_READ)
        return TWI_ERROR_READ_EXPECTED;

    /* See Figure 33-31 for repeated START and reversal from write to
       read mode.  It appears that TXRDY goes high unexpectedly before
       TXCOMP goes low.  Thus we will load the THR prematurely.  This
       is not sent until the next master read.  To avoid this, we need
       to check NACK since the master does not acknowledge the last
       byte and this will set NACK.  */

    for (i = 0; i < maxBytes; i++) {
        ret = samI2C_c::SlaveWriteWait();
        if (ret < 0)
            return ret;
        if (ret == TWI_DONE)
            return i;
            
        samI2C_c::TWIbase->TWI_THR = *data++;
    }

    /* Until the master sends a STOP, send dummy data.  */
    for (i = 0; ; i++)
    {
        ret = samI2C_c::SlaveWriteWait();
        if (ret < 0)
            return ret;
        if (ret == TWI_DONE)
            break;
            
        /* Send recognisable sequence for debugging.  */
        samI2C_c::TWIbase->TWI_THR = 'A' + i;
    }

    /* Return number of bytes written.  */
    return maxBytes + i;
}

int32_t samI2C_c::SlaveReadBytes(uint8_t buffer[], uint16_t maxBytes, bool ReadExtraData) {
	//Reads bytes into buffer. Will block until master sends STOP condition!
	
	uint8_t i;
	uint8_t *data = buffer;
	int32_t ret;

	ret = samI2C_c::SlavePoll();
	if (ret <= 0)
		return ret;
	if (ret != TWI_SLAVE_WRITE)
		return TWI_ERROR_WRITE_EXPECTED;

	/* SVACC=1, SVREAD=0.  */

	for (i = 0; i < maxBytes; i++) {
		ret = samI2C_c::SlaveReadWait();
		if (ret < 0)
			return ret;
		if (ret == TWI_DONE)
			return i;
		
		*data++ = samI2C_c::TWIbase->TWI_RHR;
	}

	/* Read any other pending data.  */
	while (ReadExtraData) {
		ret = samI2C_c::SlaveReadWait();
		if (ret < 0)
			return ret;
		if (ret == TWI_DONE)
			break;

		/* Discard data.  */
		samI2C_c::TWIbase->TWI_RHR;
	}
	return maxBytes;
}

void samI2C_c::Reset(void) {
	/* Dummy read of status register.  */
	samI2C_c::TWIbase->TWI_SR;
	/* Perform software reset of peripheral.  */
	samI2C_c::TWIbase->TWI_CR = TWI_CR_SWRST;
}

void samI2C_c::clockSetup(uint32_t freq) {
	//Picks clock generator values for desired frequency.
	uint32_t ckdiv = 0;
	uint32_t c_lh_div;

	if (freq > I2C_FAST_MODE_SPEED) {
		freq = I2C_FAST_MODE_SPEED;
	}

	c_lh_div = samClock.MasterFreqGet() / (freq * TWI_CLK_DIVIDER) - 4;

	/* cldiv must fit in 8 bits, ckdiv must fit in 3 bits */
	while ((c_lh_div > TWI_CLK_DIV_MAX) && (ckdiv < TWI_CLK_DIV_MIN)) {
		/* Increase clock divider */
		ckdiv++;
		/* Divide cldiv value */
		c_lh_div /= TWI_CLK_DIVIDER;
	}

	/* set clock waveform generator register */
	samI2C_c::TWIbase->TWI_CWGR = TWI_CWGR_CLDIV(c_lh_div) | 
		TWI_CWGR_CHDIV(c_lh_div) | TWI_CWGR_CKDIV(ckdiv);
}

uint8_t samI2C_c::addressLength(uint32_t address) {
	//Calculates address length from address.
	
	uint8_t addressSize = 0;
	for (int i = 0; i < 3; i++) {
		if (address & (0xff << (8*i))) {
			addressSize = i+1;
		}
	}
	return addressSize;
}

int32_t samI2C_c::MasterWaitAck(bool writing) {
	//Waits for ACK flag, while watching for other errors.
	uint32_t status;
	uint32_t retries = TWI_TIMEOUT;

	while (retries--) {
		status = samI2C_c::TWIbase->TWI_SR;

		/* TXCOMP set at same time as NACK.  */
		if (status & TWI_SR_NACK)
			return TWI_ERROR_NACK;

		if (status & TWI_SR_ARBLST)
			return TWI_ARBITRATION_LOST;
		
		if (writing) {
			if (status & TWI_SR_TXRDY)
				return TWI_SUCCESS;
		} else {
			if (status & TWI_SR_RXRDY)
				return TWI_SUCCESS;
		}
		samClock.delay_us(1);
	}
	return TWI_ERROR_TIMEOUT;
}

int32_t samI2C_c::MasterWaitTxcomp(void) {
	//Waits for TXCOMP flag, while watching for other errors.
	uint32_t status;
	uint32_t retries = TWI_TIMEOUT;

	while (retries--) {
		status = samI2C_c::TWIbase->TWI_SR;

		if (status & TWI_SR_TXCOMP)
			return TWI_SUCCESS;

		samClock.delay_us(1);
	}
	return TWI_ERROR_TIMEOUT;
}

int32_t samI2C_c::SlaveReadWait(void) {
	//Waits for various flags.
	
	uint32_t status;
    uint32_t retries = TWI_TIMEOUT;

    while (retries--) {
        status = samI2C_c::TWIbase->TWI_SR;
        
        if (status & TWI_SR_SVACC) {
            /* Look for GACC 0, SVREAD 0, RXRDY 1.  */
            if (!(status & (TWI_SR_GACC | TWI_SR_SVREAD)) && (status & TWI_SR_RXRDY))
                return TWI_SUCCESS;            
        }
        else {
            /* Look for EOSACC 1 or TXCOMP 1. 

               When have a reversal from write to read mode
               EOSACC=1, TXCOMP=0.

               When receive STOP EOSACC=1, TXCOMP=1.  */
            if (status & (TWI_SR_EOSACC | TWI_SR_TXCOMP))
                return TWI_DONE;    
        }
        samClock.delay_us(1);
    }
    return TWI_ERROR_TIMEOUT;
}

int32_t samI2C_c::SlaveWriteWait(void) {
	//Waits for various flags.
	
	uint32_t status;
    uint32_t retries = TWI_TIMEOUT;

    while (retries--) {
        status = samI2C_c::TWIbase->TWI_SR;
        
        if (status & TWI_SR_SVACC) {
            /* Look for GACC 0, SVREAD 1, TXRDY 1.  */
            if (!(status & TWI_SR_GACC) && ((status & (TWI_SR_SVREAD | TWI_SR_TXRDY))
                    == (TWI_SR_SVREAD | TWI_SR_TXRDY))) {
                /* The master does not acknowledge the last byte of a
                   read and so NACK=1.  Even though TXRDY=1 we do not
                   want to write to THR so indicate that the transfer
                   is finished.  */
                if (status & TWI_SR_NACK)
                    return TWI_DONE;
                return TWI_SUCCESS;            
            }
        }
        else {
            /* Look for EOSACC 1, TXCOMP 1.  */
            if ((status & (TWI_SR_EOSACC | TWI_SR_TXCOMP)) == (TWI_SR_EOSACC | TWI_SR_TXCOMP))
                return TWI_DONE;    
        }
        samClock.delay_us(1);
    }
    return TWI_ERROR_TIMEOUT;
}

//Global definitions:
samI2C_c samTWI0(0);
samI2C_c samTWI1(1);