/*
 * CircBuf.cpp
 * FIFO Circular Buffer with overwrite-oldest on full buffer.
 * Based on an implementation by GitHub's yagihiro.
 * Fixed size, as MALLOC refuses to compile under Atmel Studio.
 *
 * Created: 14/05/2016 5:56:04 PM
 *  Author: Ben Jones
 */ 

#include "sam.h"

CircBuf_c::CircBuf_c(void) {
	//Initialise buffer's other values.
	CircBuf_c::readPtr = 0;
	CircBuf_c::writePtr = 0;
	CircBuf_c::count = 0;
}

void CircBuf_c::Push(data_t data) {
	//Add one value to buffer.
	
	CircBuf_c::bufPtr[CircBuf_c::writePtr++] = data;
	if (CircBuf_c::writePtr >= BUFF_SIZE) {
		CircBuf_c::writePtr = 0;
	}

	// Overflow handled by overwriting oldest data.
	if (this->writePtr == this->readPtr) {
		this->readPtr++;
		if (this->readPtr >= BUFF_SIZE) {
			this->readPtr = 0;
		}
	}
	else {
		CircBuf_c::count++;
	}
}

data_t CircBuf_c::Pop(void) {
	//Read one value from buffer.
	data_t read_data = 0;
	if (CircBuf_c::count) {
		read_data = CircBuf_c::bufPtr[CircBuf_c::readPtr++];
		if (CircBuf_c::readPtr >= BUFF_SIZE) {
			CircBuf_c::readPtr = 0;
		}
		CircBuf_c::count--;
	}
	return read_data;
}

uint32_t CircBuf_c::Available(void) {
	return CircBuf_c::count;
}