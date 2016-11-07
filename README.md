# sam4-lib

C++ API for Atmel SAM4S. Developed under and for Atmel Studio.
Intended to be similar to Arduino style of programming. 

Credit to Michael Hayes (mph-) for a lot of help during development.

#Example program
Copy-paste this into a blank Atmel Studio project's main file.

```
/*
 * sam-blinky.cpp
 * Simple blinky and serial debug. Uses 2 LEDs and 2 serial pins only.
 *
 * Created: 26/07/2016 2:50:14 PM
 * Author : Ben Jones
 */ 


//////////////////////////////////////////////////////////////////////////
//Includes and shared classes:
#include "sam.h"
#include "../../sam4-lib/sam4-lib.hpp"


#define led0 20   //PA20
#define led1 21   //PA21
#define uart_rx 2 //PB2
#define uart_tx 3 //PB3

//////////////////////////////////////////////////////////////////////////
// System-related functions:

void clockInit(void) {
	// Sets up clock generator.
	//samClock.CrystalStart(18.432e6);
	//samClock.MainSourceSet(clock_MainSourceXtal);
	//samClock.StartPLLB(2, 13);
	//samClock.PrescalerSet(clock_PrescalerDiv2);
	//samClock.MasterSourceSet(clock_MasterSourcePLLB);
	samClock.RCFreqSet(clock_RCFreq12M);
	
	//Set up flash wait for new frequency.
	system_init_flash(samClock.MasterFreqGet());
}

void gpioInit(void) {
	gpioA.PinMode(led0, gpioOutput);
	gpioA.PinMode(led1, gpioOutput);
	
	gpioB.PinMode(uart_rx, gpioPeriphA); // UART1 Rx
	gpioB.PinMode(uart_tx, gpioPeriphA); // UART1 Tx
}

void welcomeMessage(void) {
	samUART1.WriteStr("SAM4S: Clock freq: ", -1);
	samUART1.PrintInt(samClock.MasterFreqGet());
	samUART1.WriteStr(", Xtal meas: ", -1);
	samUART1.PrintInt(samClock.MainFreqMeasure(true));
	samUART1.Write('\n');
}

void mcu_watchdog_reset (void) {
	WDT->WDT_CR = 0xA5000000 | WDT_CR_WDRSTT;
}


//////////////////////////////////////////////////////////////////////////
//Mainloop:

int main(void) {
	clockInit();
	gpioInit();
	samSysTick.Begin(2, led0); // Paced loop, LED load indicator.
	samUART1.Begin(9600, uart_parityNone);
	samADC.Begin(adc_modeSoftTrigger, adc_gain1);
	samADC.channelEnable(15);
	
	welcomeMessage();
	
	while (1) {
		samSysTick.Wait();
		
		//UART echo:
		while (samUART1.Available()) {
			samUART1.Write(samUART1.Read());
		}
		
		//Temperature log:
		samUART1.WriteStr("Temp: ", -1);
		samUART1.PrintInt(samADC.Read(15));
		samUART1.Write('\n');
		samADC.Trigger();
		
		//Blinky and watchdog kick:
		gpioA.PinSet(led1, !gpioA.PinReadSetVal(led1));
		mcu_watchdog_reset();
		
	}
}
```