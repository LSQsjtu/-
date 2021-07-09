#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "debug.h"
#include "gpio.h"
#include "hw_types.h"
#include "pin_map.h"
#include "sysctl.h"


//????
uint32_t ui32SysClock;
uint16_t num=0;

void S800_Clock_Init(void);
void S800_GPIO_Init(void);
void Delay(uint32_t value);


int main(void)
{
	S800_Clock_Init();
	S800_GPIO_Init();

//??????,PF3~0????,PF0~3????
	while(1)
  {   num++;
		  if(num>1025) num=0;
		  if(num<=512)
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);			// Turn on the LED. DA
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
			  SysCtlDelay(5*ui32SysClock/3000); //??5ms
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);							// Turn off the LED.
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);

		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);			// Turn on the LED. CD
	  	  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
			  SysCtlDelay(5*ui32SysClock/3000); //??5ms
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);							// Turn off the LED.
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);			// Turn on the LED. BC
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
			  SysCtlDelay(5*ui32SysClock/3000); //??5ms
		  	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);							// Turn off the LED.
	    	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);			// Turn on the LED. AB
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
		   	SysCtlDelay(5*ui32SysClock/3000); //??5ms
		  	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);							// Turn off the LED.
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);
				if(num==512) Delay(10000000);
			}
			else
			{
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);			// Turn on the LED. AB
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
			  SysCtlDelay(2*ui32SysClock/3000); //??2ms
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);							// Turn off the LED.
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);

				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);			// Turn on the LED. BC
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);
			  SysCtlDelay(2*ui32SysClock/3000); //??2ms
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);							// Turn off the LED.
	  	  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);

				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, GPIO_PIN_2);			// Turn on the LED. CD
	  	  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);
			  SysCtlDelay(2*ui32SysClock/3000); //??2ms
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 0x0);							// Turn off the LED.
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);

				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, GPIO_PIN_3);			// Turn on the LED. DA
		    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);
			  SysCtlDelay(2*ui32SysClock/3000); //??2ms
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x0);							// Turn off the LED.
			  GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);
				if(num==1025) Delay(10000000);
			}
	}
}

void S800_Clock_Init(void)
{
	//use internal 16M oscillator, HSI
  ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_OSC), 16000000);

	//use extern 25M crystal
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_OSC), 25000000);

	//use PLL
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 60000000);
}

void S800_GPIO_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//Enable PortF
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));			//Wait for the GPIO moduleF ready

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));			//Wait for the GPIO moduleJ ready

  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);			//Set PF0~3 as Output pin

	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0);			//Set PJ0 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

void Delay(uint32_t value)
{
	uint32_t ui32Loop;

	for(ui32Loop = 0; ui32Loop < value; ui32Loop++){};
}
