/*
 * CircBuf.h
 * FIFO Circular Buffer with overwrite-oldest on full buffer.
 * Based on an implementation by GitHub's yagihiro.
 * Fixed size, as MALLOC refuses to compile under Atmel Studio.
 *
 * Created: 14/05/2016 5:45:18 PM
 *  Author: Ben Jones
 */ 

#ifndef CIRCBUF_H_
#define CIRCBUF_H_

//Change this to change the buffer's data type and size.
typedef uint8_t data_t;
#define BUFF_SIZE 256

class CircBuf_c {
	public:
		//Initialiser:
		CircBuf_c(void);
	
		//Write:
		void Push(data_t data);
	
		//Read:
		data_t Pop(void);
	
		//Bytes available:
		uint32_t Available(void);
	
	private:
		data_t bufPtr[BUFF_SIZE];
		uint32_t readPtr;
		uint32_t writePtr;
		uint32_t count;
};

#include "CircBuf.cpp"

#endif /* CIRCBUF_H_ */
