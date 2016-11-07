/*
 * CircBuf.hpp
 * FIFO Circular Buffer with overwrite-oldest-on-full.
 * Based on an implementation by GitHub's yagihiro.
 *
 * TODO: Make malloc or new compile under Atmel Studio, 
 *  and add variable size support.
 *
 * Created: 14/05/2016
 * Modified: 04/11/2016
 * Author: Ben Jones
 */ 

#ifndef CIRCBUF_HPP_
#define CIRCBUF_HPP_

//Change this to change the buffer's and size - due to Malloc issues :(
#define BUFF_SIZE 256

template <class data_t> 
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
		//data_t* bufPtr;
		uint32_t readPtr;
		uint32_t writePtr;
		uint32_t count;
		//uint32_t buffSize;
};

#include "CircBuf.cpp"

#endif /* CIRCBUF_HPP_ */
