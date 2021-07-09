
#include <stdint.h>
#include <stdbool.h>
#include "hw_memmap.h"
#include "debug.h"
#include "gpio.h"
#include "hw_i2c.h"
#include "hw_types.h"
#include "i2c.h"
#include "pin_map.h"
#include "sysctl.h"
#include "SysTick.h"
#include "interrupt.h"
#include "hw_ints.h"

//*****************************************************************************
//
//I2C GPIO chip address and resigster define
//
//*****************************************************************************
#define TCA6424_I2CADDR 					0x22
#define PCA9557_I2CADDR						0x18

#define PCA9557_INPUT							0x00
#define	PCA9557_OUTPUT						0x01
#define PCA9557_POLINVERT					0x02
#define PCA9557_CONFIG						0x03

#define TCA6424_CONFIG_PORT0			0x0c
#define TCA6424_CONFIG_PORT1			0x0d
#define TCA6424_CONFIG_PORT2			0x0e

#define TCA6424_INPUT_PORT0				0x00
#define TCA6424_INPUT_PORT1				0x01
#define TCA6424_INPUT_PORT2				0x02

#define TCA6424_OUTPUT_PORT0			0x04
#define TCA6424_OUTPUT_PORT1			0x05
#define TCA6424_OUTPUT_PORT2			0x06

#define SYSTICK_FREQUENCY					1000			//1000hz

#define	I2C_FLASHTIME							500				//500ms��I2C��ʱʱ��
#define GPIO_FLASHTIME						50				//50ms��GPIO��ʱʱ��

#define WAITTIME			    (uint32_t)100000

void 		S800_Clock_Init(void);
void 		S800_GPIO_Init(void);
void		S800_I2C0_Init(void);
void 		Delay(uint32_t value);
uint8_t I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData);
uint8_t I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr);
void 		S800_SysTick_Init(void);
void    Increment(void);  //The function that increments the clock

uint32_t ui32SysClock;
uint8_t action = 0;
uint8_t seg7[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x58,0x5e,0x079,0x71,0x5c};
uint8_t btn_state0 = 0x00, btn_state1 = 0x00;
uint8_t sw0_pressed = 0, sw1_pressed = 0;  //Check if the button is pressed
uint8_t btn_cnt = 0;
uint16_t count=500;  //The number of countdowns
uint8_t minute = 0;
uint8_t second = 0;
uint8_t stepup = 0;
uint8_t addon = 0;   //number added to the minute and second, if either button is pressed for a long time

//systick software counter define
volatile uint16_t systick_200ms_counter=200; //200ms timer
volatile uint8_t systick_200ms_status=0;   //200ms timing status
volatile uint16_t systick_counter=0;       //1s timer
volatile uint8_t systick_status=0;         //1s timing status

int main(void)
{
	volatile uint8_t result;
	volatile uint8_t cnt=0; 
	volatile uint16_t	i2c_flash_cnt=0, gpio_flash_cnt=0;
	
	IntMasterDisable();	//���ж�

	S800_Clock_Init();
	S800_GPIO_Init();
	S800_I2C0_Init();
	S800_SysTick_Init();

  IntMasterEnable();	//���ж�	
	
	while (1)
	{
		if (systick_status)
		{
			systick_status = 0;
			Increment();
		}
		cnt = minute / 10;
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[cnt]);	//write port 1 				
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<0);				//write port 2
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(0));
		cnt = minute % 10;
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[cnt]);	//write port 1 				
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<1);				//write port 2
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(0));
		
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,0x40);  //write port 1
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<2);   //write port 2
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(0));
		
		cnt = second / 10;
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[cnt]);	//write port 1 				
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<3);				//write port 2
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(0));
		cnt = second % 10;
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[cnt]);	//write port 1 				
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<4);				//write port 2
		result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,(uint8_t)(0));
	}
}

void S800_Clock_Init(void)
{
	//use internal 16M oscillator, HSI
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_OSC), 16000000);		

	//use extern 25M crystal
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_OSC), 25000000);		

	//use PLL
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_INT | SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 20000000);
}

void S800_GPIO_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ)){};			//Wait for the GPIO moduleJ ready	
	
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1);//Set the PJ0,PJ1 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
		
	GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1, GPIO_BOTH_EDGES);  //Set PJ0 and PJ1 as interrupt source
	
	GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0 | GPIO_PIN_1);                 //Enable interrupt at GPIO level
	
	IntEnable(INT_GPIOJ);	                                      //Enable interrupt at NVIC level

}

void S800_I2C0_Init(void)
{
	uint8_t result;
	
  SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	GPIOPinConfigure(GPIO_PB2_I2C0SCL);
  GPIOPinConfigure(GPIO_PB3_I2C0SDA);
  GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
  GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

	I2CMasterInitExpClk(I2C0_BASE,ui32SysClock, true);										//config I2C0 400k
	I2CMasterEnable(I2C0_BASE);	

	result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_CONFIG_PORT0,0x0ff);		//config port 0 as input
	result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_CONFIG_PORT1,0x0);			//config port 1 as output
	result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_CONFIG_PORT2,0x0);			//config port 2 as output 

	result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_CONFIG,0x00);					//config port as output
	result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,0x0ff);				//turn off the LED1-8
	
}

uint8_t I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData)
{
	uint8_t rop;
	
	while(I2CMasterBusy(I2C0_BASE)){}; //æ�ȴ�
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false); //��ӻ���ַ��д
	I2CMasterDataPut(I2C0_BASE, RegAddr);	//�����ݵ�ַ
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); //�������߷���
	while(I2CMasterBusy(I2C0_BASE)){};
	rop = (uint8_t)I2CMasterErr(I2C0_BASE);

	I2CMasterDataPut(I2C0_BASE, WriteData); //������ֵ
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); //�������߷��ͣ����ͺ�ֹͣ
	while(I2CMasterBusy(I2C0_BASE)){};
	rop = (uint8_t)I2CMasterErr(I2C0_BASE);

	return rop;
}

uint8_t I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr)
{
	uint8_t value;
	
	while(I2CMasterBusy(I2C0_BASE)){};	//æ�ȴ�
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false); //��ӻ���ַ��д
	I2CMasterDataPut(I2C0_BASE, RegAddr); //�����ݵ�ַ
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_SEND); //�������߷���
	while(I2CMasterBusBusy(I2C0_BASE));
	if (I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
		return 0; //����
	Delay(100);

	//receive data
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, true); //��ӻ���ַ����
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE); //�������߽���
	while(I2CMasterBusBusy(I2C0_BASE));
	if (I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
		return 0; //����
	Delay(100);

	value=I2CMasterDataGet(I2C0_BASE);

	return value;
}

void S800_SysTick_Init(void)
{
	SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); //��ʱ1ms
	SysTickEnable();
	SysTickIntEnable();
}

/*
	Corresponding to the startup_TM4C129.s vector table systick interrupt program name
*/
void SysTick_Handler(void)
{
	if (systick_counter == 0) //����1ms��SysTick����1s�Ķ�ʱ��
	{
		systick_counter = 1000;
		systick_status = 1;
	}
	else
		systick_counter--;
	
	if (sw0_pressed)
	{
		if (systick_200ms_counter == 0)
		{
			systick_200ms_counter = 200;
			Increment();
		}
		else
			systick_200ms_counter--;
	}
	
	if(sw1_pressed)
	{
		if (systick_200ms_counter == 0)
		{
			systick_200ms_counter = 200;
			minute = (minute + 1) % 60;
		}
		else
			systick_200ms_counter--;
	}
}

void GPIOJ_Handler(void)
{
	unsigned long intStatus;

	intStatus = GPIOIntStatus(GPIO_PORTJ_BASE, true); //��ȡ�ж�״̬
	GPIOIntClear(GPIO_PORTJ_BASE, intStatus );  //����ж������ź�
	Delay(WAITTIME);
	btn_state0 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0);  //Recheck the state of PJ0
	btn_state1 = GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1);  //Recheck the state of PJ1 
	if (btn_state0 == 0x00)
	{
		Increment();
		sw0_pressed = 1;
	}
	else
	  sw0_pressed = 0;
	
	if (btn_state1 == 0x00)
	{
		minute = (minute + 1) % 60;
		sw1_pressed = 1;
	}
	else 
	  sw1_pressed = 0;
}

void Delay(uint32_t value)
{
	uint32_t ui32Loop;
	
	for(ui32Loop = 0; ui32Loop < value; ui32Loop++){};
}

void Increment(void)
{
	second = (second + 1) % 60;
	if (stepup)
	{
		minute = (minute + 1) % 60;
		stepup = 0;
	}	
	if (second == 59)
		stepup = 1;
}
