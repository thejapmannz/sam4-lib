/*
 * CircBuf.cpp
 * FIFO Circular Buffer with overwrite-oldest-when-full.
 * Based on an implementation by GitHub's yagihiro.
 *
 * Created: 14/05/2016 5:56:04 PM
 *  Author: Ben Jones
 */ 


//Default constructor:
template <class data_t, uint32_t BuffSize>
CircBuf_c<data_t, BuffSize>::CircBuf_c() {
	
	//Initialise buffer's values.
	this->readPtr = 0;
	this->writePtr = 0;
	
	//this->count = 0;
}

template <class data_t, uint32_t BuffSize>
void CircBuf_c<data_t, BuffSize>::Push(data_t data) {
	//Add one value to buffer.
	
	this->bufPtr[this->writePtr++] = data;
	if (this->writePtr >= BuffSize) {
		this->writePtr = 0;
	}

	// Overflow handled by overwriting oldest data.
	if (this->writePtr == this->readPtr) {
		this->readPtr++;
		if (this->readPtr >= BuffSize) {
			this->readPtr = 0;
		}
	}
	/*else {
		this->count++;
	}*/
}

template <class data_t, uint32_t BuffSize>
data_t CircBuf_c<data_t, BuffSize>::Pop(void) {
	//Read one value from buffer.
	data_t read_data = 0;
	if (this->Available()) {
		read_data = this->bufPtr[this->readPtr++];
		if (this->readPtr >= BuffSize) {
			this->readPtr = 0;
		}
		//this->count--;
	}
	return read_data;
}

template <class data_t, uint32_t BuffSize>
data_t CircBuf_c<data_t, BuffSize>::Peek(void) {
	//Read one value from buffer.
	data_t read_data = 0;
	if (this->Available()) {
		read_data = this->bufPtr[this->readPtr];
	}
	return read_data;
}

template <class data_t, uint32_t BuffSize>
uint32_t CircBuf_c<data_t, BuffSize>::Available(void) 
{
	int64_t count = this->writePtr - this->readPtr;
	if (count < 0) { // If write pointer has wrapped before read pointer
		count += BuffSize;
	}
	return (uint32_t)count;
	
	//return this->count;
}
