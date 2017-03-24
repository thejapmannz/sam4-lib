/*
 * CircBuf.hpp
 * FIFO Circular Buffer with overwrite-oldest-on-full.
 * Based on an implementation by GitHub's yagihiro.
 *
 * Created: 14/05/2016
 * Modified: 04/11/2016
 * Author: Ben Jones
 */ 

#ifndef CIRCBUFF_HPP_
#define CIRCBUFF_HPP_

//#define CIRCBUFF_DEFAULT_SIZE 256

template <class data_t, uint32_t BuffSize> 
class CircBuf_c {
	public:
		//Initialiser:
		CircBuf_c();
	
		//Write:
		void Push(data_t data);
	
		//Read:
		data_t Pop(void);
	
		//Bytes available:
		uint32_t Available(void);
	
	private:
		data_t bufPtr[BuffSize];
		uint32_t readPtr;
		uint32_t writePtr;
		//uint32_t count;
};

#include "CircBuf.cpp"

#endif /* CIRCBUF_HPP_ */
