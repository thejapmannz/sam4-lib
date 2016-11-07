/*
 * CircBuf.cpp
 * FIFO Circular Buffer with overwrite-oldest on full buffer.
 * Based on an implementation by GitHub's yagihiro.
 *
 * TODO: Make malloc or new compile under Atmel Studio,
 *  and add variable size support.
 *
 * Created: 14/05/2016 5:56:04 PM
 *  Author: Ben Jones
 */ 

//#include "stdlib.h"

template <class data_t>
CircBuf_c<data_t>::CircBuf_c(void) {
	//Initialise array:
	//this->bufPtr = (data_t*) malloc(sizeof(data_t) * buffSize;
	
	//Initialise buffer's other values.
	this->readPtr = 0;
	this->writePtr = 0;
	this->count = 0;
	//this->buffSize = buffSize;
}

template <class data_t>
void CircBuf_c<data_t>::Push(data_t data) {
	//Add one value to buffer.
	
	this->bufPtr[this->writePtr++] = data;
	if (this->writePtr >= BUFF_SIZE) {
		this->writePtr = 0;
	}

	// Overflow handled by overwriting oldest data.
	if (this->writePtr == this->readPtr) {
		this->readPtr++;
		if (this->readPtr >= BUFF_SIZE) {
			this->readPtr = 0;
		}
	}
	else {
		this->count++;
	}
}

template <class data_t>
data_t CircBuf_c<data_t>::Pop(void) {
	//Read one value from buffer.
	data_t read_data = 0;
	if (this->count) {
		read_data = this->bufPtr[this->readPtr++];
		if (this->readPtr >= BUFF_SIZE) {
			this->readPtr = 0;
		}
		this->count--;
	}
	return read_data;
}

template <class data_t>
uint32_t CircBuf_c<data_t>::Available(void) {
	return this->count;
}