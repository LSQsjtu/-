
#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "debug.h"
#include "gpio.h"
#include "hw_i2c.h"
#include "hw_types.h"
#include "pin_map.h"
#include "sysctl.h"
#include "SysTick.h"
#include "interrupt.h"
#include "hw_ints.h"

void S800_Clock_Init(void);
void S800_GPIO_Init(void);
void S800_SysTick_Init(void);

uint32_t ui32SysClock;
uint8_t  btn_cnt = 0;

//systick software counter define
volatile uint8_t	systick_1s_status=0, systick_500ms_status=0; //1s和500ms计时状态

int main(void)
{
	IntMasterDisable();	//关中断

	S800_Clock_Init();
	S800_GPIO_Init();
	S800_SysTick_Init(); 

	IntMasterEnable();	//开中断	

	while (1)
	{	
		if (systick_500ms_status) {
			systick_500ms_status = 0;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_PIN_0);			// Turn off the LED.
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, 0x0);							// Turn on the LED.
		}

		if (systick_1s_status) {
			systick_1s_status = 0;
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);			// Turn off the LED.
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x0);							// Turn on the LED.
		}
		
		if (btn_cnt == 2)
			...
	}
	
}

void S800_Clock_Init(void)
{
	//use internal 16M oscillator, HSI
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_OSC), 16000000);		

	//use extern 25M crystal
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_OSC), 2500000);		

	//use PLL
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 60000000);	
}

void S800_GPIO_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//Enable PortF
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));			//Wait for the GPIO moduleF ready

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));			//Wait for the GPIO moduleJ ready	
	
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_1);	//Set PF0,PF1 as Output pin

	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);		//Set the PJ0 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

//add more codes...

}
	
void GPIOJ_Handler(void) 	//PortJ中断处理
{
	unsigned long intStatus;

	intStatus = GPIOIntStatus(GPIO_PORTJ_BASE, true); //获取中断状态
	GPIOIntClear(GPIO_PORTJ_BASE, intStatus );  //清除中断请求信号

	if (intStatus & GPIO_PIN_0) {	//PJ0?
	     btn_cnt = btn_cnt % 4 + 1;
	} 

}

void S800_SysTick_Init(void)
{
	SysTickPeriodSet(ui32SysClock/2); //定时0.5s
	SysTickEnable();
	SysTickIntEnable();
}

/*
	Corresponding to the startup_TM4C129.s vector table systick interrupt program name
*/
void SysTick_Handler(void)
{
	static uint8_t k = 0;
	
	systick_500ms_status = 1;
	
	if (k) systick_1s_status = 1;

	k = 1 - k;
	
}
