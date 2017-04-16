# sam4-lib

C++ API for Atmel SAM4S. Developed under and for Atmel Studio.
Intended to be similar to Arduino style of programming. 

Credit to Michael Hayes (mph-) for a lot of help during development.

# Example program

Copy-paste this into a blank Atmel Studio project's main file.

```
/*
 * sam-blinky.cpp
 * Simple blinky and serial debug. Uses 2 LEDs and 2 serial pins only.
 *
 * Created: 26/07/2016
 * Updated: 16/04/2017
 * Author : Ben Jones
 */ 


//////////////////////////////////////////////////////////////////////////
//Includes and shared classes:
#include "sam.h"
#include "../../sam4-lib/sam4-lib.hpp" // All drivers written to date.

#define led0 20   //PA20
#define led1 21   //PA21
#define uart_rx 2 //PB2
#define uart_tx 3 //PB3

//////////////////////////////////////////////////////////////////////////
// System-related functions:

void clockInit(void) {
	// Sets up clock generator.
	/*samClock.CrystalStart(18.432e6);
	samClock.MainSourceSet(clock_MainSourceXtal);
	samClock.StartPLLB(2, 13);
	samClock.PrescalerSet(clock_PrescalerDiv2);
	samClock.MasterSourceSet(clock_MasterSourcePLLB);*/
	samClock.RCFreqSet(clock_RCFreq12M);
}

void gpioInit(void) {
	
	samGPIOA.Begin();
	samGPIOB.Begin();
	
	samGPIOA.PinMode(led0, gpio_modeOutput, gpio_propNormal);
	samGPIOA.PinMode(led1, gpio_modeOutput, gpio_propNormal);
	
	samGPIOA.PinMode(uart_rx, gpio_modePeriphA, gpio_propPullup); // UART1 Rx
	samGPIOA.PinMode(uart_tx, gpio_modePeriphA, gpio_propNormal); // UART1 Tx
}

void welcomeMessage(void) 
{
	samUART1.printf("SAM4S: Init success. Clock freq: %d\n", samClock.MasterFreqGet());
}

//////////////////////////////////////////////////////////////////////////
//Mainloop:

int main(void) 
{
	clockInit();
	gpioInit();
	samWatchdog.Begin(2000); // Watchdog timeout after 2 seconds
	samSysTick.Begin(2); // Paced loop at 2 Hz
	samUART1.Begin(9600, uart_parityNone);
	samADC.Begin(adc_modeSoftTrigger);
	samADC.channelEnable(15);
	
	welcomeMessage();
	
	while (1) {
		
		//LED load indicator and SysTick pacer:
		samGPIOA.PinSetLow(led0);
		samSysTick.Wait();
		samGPIOA.PinSetHigh(led0);
		
		//UART echo:
		while (samUART1.Available()) {
			samUART1.Write(samUART1.Read());
		}
		
		//Temperature log:
		samUART1.printf(Temperature: %d\n", samADC.Read(15));
		samADC.Trigger();
		
		//Blinky and watchdog:
		samGPIOA.PinSet(led1, !samGPIOA.PinReadSetVal(led1));
		samWatchdog.Kick();
	}
}
```