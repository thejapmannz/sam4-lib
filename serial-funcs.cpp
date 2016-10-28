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