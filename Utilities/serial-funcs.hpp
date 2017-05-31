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

#include "stdarg.h"

// Ascii lookup table for number conversions of arbitrary bases:
char SerialAsciiTable[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
// Values of ASCII table letters when converted to integers:
//                          0 thru 47 invalid, and extra value for index -1 case:
int SerialAsciiInverse[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
//                          0 thru 9 at 48 thru 57, 
							0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 
//							58 thru 64 invalid, 
							-1, -1, -1, -1, -1, -1, -1, 
//							A thru Z at 65 thru 90, 
							10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
//							91 thru 96 invalid, 
							-1, -1, -1, -1, -1, -1, 
//							a thru z at 97 thru 122, 
							10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 
							//123 thru 255 invalid.
							-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
							-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 
							-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

class SerialStream {
	public:
		//Template for inheriting classes' functions:
		virtual int16_t Read(void); // Note returns -1 if receive buffer empty.
		virtual int16_t Peek(void);
		virtual void Write(uint8_t byte);
		virtual uint32_t Available(void);
		
		//Iterative extensions for basic Read and Write, with 
		//  optional number-of-bytes specifier.
		uint32_t ReadStr(char buffer[]);
		uint32_t ReadStr(char buffer[], uint32_t num_bytes);
		void WriteStr(char buffer[]);
		void WriteStr(char buffer[], uint32_t num_bytes);
		
		//Works in progress: printf, scanf
		void printf(const char* format, ...);
		uint32_t scanf(const char* format, ...);
		
	private:
		//Convert integer etc to ascii and send:
		void PrintNum(int64_t value, bool signedValue, uint32_t base, uint32_t whitespace_pad);
		//Convert ascii to integer (non-blocking):
		int64_t ReadNum(uint32_t base);
		
};


#include "serial-funcs.cpp"

#define SERIAL_FUNCS_H
#endif
