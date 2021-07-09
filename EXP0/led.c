#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "debug.h"
#include "gpio.h"
#include "hw_types.h"
#include "pin_map.h"
#include "sysctl.h"

int LedOnOff(void)
{
	uint32_t ui32Loop;
	//Enable PortF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//Set PF0 as Output pin
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
	
	while(1) {
		// Turn on the LED.
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
   	// Delay
		for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){ };
		// Turn off the LED.
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);
   	// Delay
		for(ui32Loop = 0; ui32Loop < 800000; ui32Loop++){ };
  }
}

