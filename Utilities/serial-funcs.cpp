/*
 * serial-funcs.cpp: A set of serial string manipulation functions, 
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

#include "stdarg.h"

void SerialStream::WriteStr(char buffer[], int32_t num_bytes) {
	//Writes a whole string to internal buffer, to be sent.
	int32_t i;
	
	if (num_bytes > 0) { // strncpy mode, number of bytes.
		for (i = 0; i < num_bytes; i++) {
			this->Write(buffer[i]);
		}
	}
	else if (num_bytes == -1) { // strcpy mode, null terminate.
		i = 0;
		while (buffer[i] != 0) {
			this->Write(buffer[i++]);
		}
	}
	
}

void SerialStream::PrintInt(int64_t value) {
	// Sends a number to UART, as we humans would read it.
	uint32_t digit = 0;
	if (value < 0) {
		this->Write('-');
		value = -value;
	}
	
	bool started = 0;
	for (int32_t i = 10; i >= 0; i--) { // Max 2x10^10 for 32-bit.
		digit = value;
		for (int32_t j = 0; j < i; j++) { // No power function available.
			digit /= 10;
		}
		digit %= 10;
		if (digit) {
			started = 1;
		}
		if (started || (i == 0)) {
			this->Write(digit + '0');
		}
	}
}

void SerialStream::PrintBits(uint32_t value) {
	//Sends a number to UART, as 32 bits.
	for (int i = 31; i >= 0; i--) {
		this->Write((value & (1 << i)) ? '1' : '0');
	}
}

void SerialStream::ReadStr(char buffer[], uint32_t num_bytes) {
	//Reads a whole string from internal receive buffer.
	for (uint32_t i = 0; i < num_bytes && this->Available() > 0; i++) {
		buffer[i] = this->Read();
	}
}

void SerialStream::PrintNum(int64_t value, bool signedValue, uint32_t base)
{
	//Prints a number out in human-readable form. 
	
	char buff[25]; // Max uint64_t value is 20 digits long.
	char *ptr = buff + 24; // Will be writing to end of buffer.
	
	if ((value < 0) && signedValue) 
	{
		this->Write('-');
		value = -value;
	}
	
	*ptr = '\0'; // Null terminator
	
	// Convert to ascii:
	do {
		*(--ptr) = '0' + (value % base);
		value /= base;
	} while ((value > 0 && ptr > buff));
	
	// Write out buffer:
	this->WriteStr(ptr, -1);
}



void SerialStream::printf(const char* format, ...) 
{
	// Basic implementation of formatted text printing. 
	char cc;
	
	//Handle argument list:
	va_list arg;
	va_start(arg, format);
	
	//Iterate through format string:
	while ((cc = *(format++))) 
	{
		if (cc != '%') 
		{
			this->Write(cc); // Pass through normal text
		}
		
		else 
		{
			cc = *(format++); // cc will now be the data type to print
			
			switch (cc) {
				default:
					break; // Handle unknown format specifiers nicely.
				case 0:
					format--; // So while loop will catch the zero
					break;
					
				case '%': // Print percent.
					this->Write('%');
					break;
				
				case 'c': // Character
					this->Write((char)va_arg(arg, unsigned int));
					break;
				
				case 's': // String
					char* strPtr;
					strPtr = va_arg(arg, char*);
					this->WriteStr(strPtr, -1);
					break;
					
				case 'd': // Decimal
				case 'u': // Unsigned
				case 'i': // Integer
					this->PrintNum(va_arg(arg, unsigned int), true, 10);
					break;
				case 'b': // Binary/bitfield
					this->PrintNum(va_arg(arg, unsigned int), false, 2);
					break;
				case 'x': // Hex
					this->PrintNum(va_arg(arg, unsigned int), false, 16);
					break;
				case 'o': // Octal
					this->PrintNum(va_arg(arg, unsigned int), false, 8);
					break;
			}
		}
	}
	
	va_end(arg);
}

















