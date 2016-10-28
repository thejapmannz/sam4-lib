/*
 * serial-funcs.h: A set of serial string manipulation functions, 
 * similar to the Arduino Stream class, and often using functions from 
 * the Arduino libraries, where acknowledged. 
 *
 * This code assumes that any classes using the provided functions have 
 * defined three key functions: Available(), Write(), and Read(). 
 * Since we are string-processing, everything is uint8_t except sizes.
 *
 * Author: Benjamin Jones
 * Date: 28/10/2016
 */

#ifndef SERIAL_FUNCS_H

class SerialStream {
	public:
		//Template for inheriting classes:
		virtual void Write(uint8_t byte);
		virtual uint32_t Available(void);
		virtual int16_t Read(void); // Note returns -1 if receive buffer empty.
		
		
		//For direct-copy writes:
		void WriteStr(char buffer[], int32_t num_bytes); // Use num_bytes=-1 for null--terminated strings :)
		
		//For formatted text writes e.g. printf:
		void PrintInt(int64_t value);
		void PrintBits(uint32_t value);
		
		//For direct-copy reads:
		void ReadStr(char buffer[], uint32_t num_bytes);
		
		//For formatted text reads e.g. scanf:
		//None
};


#include "serial-funcs.cpp"

#define SERIAL_FUNCS_H
#endif