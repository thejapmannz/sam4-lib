/*
 * serial-funcs.cpp: A set of serial string manipulation functions, 
 * similar to the Arduino Stream class, and often using functions from 
 * the Arduino libraries, where acknowledged. 
 *
 * This code assumes that any classes using the provided functions have 
 * defined three key functions: Available(), Write(), and Read(). 
 * Since we are string-processing, everything is uint8_t, 
 * except sizes and return-negative-on-error reads.
 *
 * Author: Benjamin Jones
 * Date: 28/10/2016
 */

#include "stdarg.h"


uint32_t SerialStream::ReadStr(char buffer[]) {
	//Reads a whole string from internal receive buffer.
	uint32_t i = 0;
	
	while (this->Available()) {
		buffer[i++] = this->Read();
		
		if (buffer[i-1] == '\n') {
			break;
		}
	}
	
	buffer[i] = '\0'; // Null-terminate.
	
	return i;
}

uint32_t SerialStream::ReadStr(char buffer[], uint32_t num_bytes) {
	//Reads a whole string from internal receive buffer.
	uint32_t i;
	
	for (i = 0; i < num_bytes && this->Available() > 0; i++) {
		buffer[i] = this->Read();
	}
	return i;
}

void SerialStream::WriteStr(char buffer[]) {
	//Writes a whole string to internal buffer, to be sent.
	uint32_t i = 0;;
	
	while (buffer[i] != 0) {
		this->Write(buffer[i++]);
	}
	
}

void SerialStream::WriteStr(char buffer[], uint32_t num_bytes) {
	//Writes a whole string to internal buffer, to be sent.
	uint32_t i;
	
	for (i = 0; i < num_bytes; i++) {
		this->Write(buffer[i]);
	}
}


/*
 * Depreciated functions, replaced by printf:
 * 
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
*/


void SerialStream::PrintNum(int64_t value, bool signedValue, uint32_t base, uint32_t whitespace_pad)
{
	//Prints a number out in human-readable form. 
	
#define buff_len 50         // Note max uint64_t value is 20 digits long, plus whitespace.
	char buff[buff_len]; 
	char *ptr = buff + buff_len - 1; // Will be writing to end of buffer.
	
	if ((value < 0) && signedValue) 
	{
		this->Write('-');
		value = -value;
	}
	
	*ptr = '\0'; // Null terminator
	
	// Convert to ascii:
	do {
		//*(--ptr) = '0' + (value % base);
		*(--ptr) = SerialAsciiTable[value % base];
		value /= base;
	} while ((value > 0 && ptr > buff));
	
	//Whitespace pad, if needed:
	// while used_characters < whitespace_width
	while ((buff - ptr + buff_len) < whitespace_pad && ptr > buff)
	{
		*(--ptr) = ' ';
	}
	
	// Write out buffer:
	this->WriteStr(ptr);
	//this->WriteStr(ptr, -1);
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
					this->WriteStr((char*)va_arg(arg, char*));
					break;
					
				case 'd': // Decimal
				case 'u': // Unsigned
				case 'i': // Integer
					this->PrintNum(va_arg(arg, int), true, 10, 0);
					break;
				case 'b': // Binary/bitfield
					this->PrintNum(va_arg(arg, unsigned int), false, 2, 32);
					break;
				case 'x': // Hex
					this->WriteStr("0x");
					this->PrintNum(va_arg(arg, unsigned int), false, 16, 0);
					break;
				case 'o': // Octal
					this->Write('0');
					this->PrintNum(va_arg(arg, unsigned int), false, 8, 0);
					break;
			}
		}
	}
	
	va_end(arg);
}


int64_t SerialStream::ReadNum(uint32_t base) 
{
	// Reads in characters from buffer and converts to an integer.
	// Float support to come later.
	
	int64_t ret_val = 0;
	int neg_flag = 1;
	
	// Step 1: Eat up whitespace and non-numerics.
	while (this->Available() && (this->Peek() != '-') // While not a negative sign...
			&& ((SerialAsciiInverse[this->Peek() + 1] < 0) // Or within valid characters
			|| (SerialAsciiInverse[this->Peek() + 1] >= base))) {
		this->Read(); // Consume anything we can't convert.
	}
	
	// Step 2: Negative flag.
	if (this->Peek() == '-') {
		this->Read();
		neg_flag = -1;
	}
	
	// Step 3: Convert number.
	while (this->Available() && SerialAsciiInverse[this->Peek() + 1] >= 0 
		&& SerialAsciiInverse[this->Peek() + 1] < base) {
		ret_val *= base; // Shift left one space
		ret_val += SerialAsciiInverse[this->Read() + 1]; // Add next digit
	}
	
	return ret_val * neg_flag;
}
	
uint32_t SerialStream::scanf(const char* format, ...) 
{
	// Formatted read from port.
	// TODO.
	
	uint32_t i = 0;
	uint32_t index = 0;
	uint32_t count = 0;
	
	//Handle argument list:
	va_list arg;
	va_start(arg, format);
	
	while (format && format[i] && this->Available()) 
	{
		if (format[i] == '%') 
		// Convert a value.
		{
			switch (format[++i]) 
			{
				default: {
					break; // Ignore invalid specifiers. 
				}
				case '%': {
					i--; // Pass % character to next segment
					break;
				}
				case 'c': {
					*(char*)va_arg(arg, char*) = this->Read();
					count++;
					break;
				}
				case 's': {
					char* target = (char*)va_arg(arg, char*);
					/*while (this->Peek() > 0 && this->Peek() != '\n') {
						*(target++) = this->Read();
					}
					*target = '\0'; // Null-terminate external buffer.*/
					this->ReadStr(target);
					break;
				}
				case 'd':
				case 'i': {
					*(int*)va_arg(arg, int*) = this->ReadNum(10);
					count++;
					break;
				}
				case 'x': {
					*(uint32_t*)va_arg(arg, uint32_t*) = this->ReadNum(16);
					count++;
					break;
				}
			}
			i++;	
		}
		
		// Otherwise, match text.
		index = i;
		while (this->Available() && format[index] != 0) // While characters available
		{
			if (this->Read() == format[index]) { // If character matches,
				index++; // move along format string
				
				if (format[index] == '%') { // if done matching
					i = index; // Export increment
					break; 
				}
			}
			else { // If character doesn't match, 
				index = i; // go back to start point in format string
			}
			
		}
		
		//i++;
	}
	
	va_end(arg);
	return count;
}








