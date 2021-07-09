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

//#define	I2C_FLASHTIME							500				//500ms,I2C????
#define GPIO_FLASHTIME						50				//50ms,GPIO????


void 		S800_Clock_Init(void);
void 		S800_GPIO_Init(void);
void		S800_I2C0_Init(void);
void 		Delay(uint32_t value);
uint8_t I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData);
uint8_t I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr);
void 		S800_SysTick_Init(void);

uint32_t ui32SysClock;
uint16_t seg7[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x58,0x5e,0x079,0x71,0x5c,0x40};

//systick software counter define
volatile uint16_t systick_10ms_couter=0, systick_100ms_couter=0,systick_3ms_couter=0,systick_1s_couter=0,systick_1ms_couter=0,systick_50ms_couter=0; //10ms?100ms???
volatile uint8_t	systick_10ms_status=0, systick_100ms_status=0,systick_3ms_status=0,systick_1s_status=0,systick_1ms_status=0,systick_50ms_status=0; //10ms?100ms????
uint8_t key;//SW1~8
uint8_t num=0;
uint16_t count=0;
uint16_t count1=0;
uint16_t I2C_FLASHTIME=500;

int main(void)
{
	volatile uint8_t result;
	volatile uint8_t cnt=0; 
	volatile uint16_t	i2c_flash_cnt=0, gpio_flash_cnt=0;
	uint16_t flag=0;
	uint16_t a=0;
	uint16_t b=0;
	uint16_t c=0;
	uint16_t d=0;
	uint16_t p=17;
	
	IntMasterDisable();	//???

	S800_Clock_Init();
	S800_GPIO_Init();
	S800_I2C0_Init();
	S800_SysTick_Init();

  IntMasterEnable();	//???	
	result = I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~((1<<cnt)+(1<<(cnt+1))));//initialize the led

	while (1)
	{
		if(num==1)//press PJ0
		{
			num=0;
			if(d<9) d++;
			else if(d==9)
		  {
				d=0;
				if(c<5) c++;
				else if(c==5)
				{
					c=0;
					if(b<9) b++;
					else if(b==9)
					{
						b=0;
						if(a<5) a++;
						else if(a==5) a=0;
					}
				}		
			}	
		}
		if(num==2)//press PJ1
		{
			num=0;
			if(b<9) b++;
			else if(b==9)
			{
				b=0;
				if(a<5) a++;
				else if(a==5) a=0;
			}
		}
		if(systick_50ms_status) //detect every 50ms
		{
			systick_50ms_status=0;
			if(count==4) //200ms
			{
				count=0;
				if(d<9) d++;
			  else if(d==9)
		    {
				  d=0;
				  if(c<5) c++;
				  else if(c==5)
				  {
					  c=0;
					  if(b<9) b++;
					  else if(b==9)
					  {
						  b=0;
						  if(a<5) a++;
						  else if(a==5) a=0;
					  }
				  }		
			  }
			}
			if(count1==4)//new
			{
				count1=0;
				if(b<9) b++;
			  else if(b==9)
			  {
				  b=0;
				  if(a<5) a++;
				  else if(a==5) a=0;
			  }
			}
			if((!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_0))) count++;
			else count=0;
			if((!GPIOPinRead(GPIO_PORTJ_BASE, GPIO_PIN_1))) count1++;
			else count1=0;
		}
		
		if(systick_1s_status&&count==0)
		{
			systick_1s_status=0;
			if(d<9) d++;
			else if(d==9)
		  {
				d=0;
				if(c<5) c++;
				else if(c==5)
				{
					c=0;
					if(b<9) b++;
					else if(b==9)
					{
						b=0;
						if(a<5) a++;
						else if(a==5) a=0;
					}
				}		
			}	
		}
		
		if (systick_10ms_status) //10ms???
		{
			systick_10ms_status		= 0; //??10ms????
			
			if (++gpio_flash_cnt >= GPIO_FLASHTIME/10)	//5*10ms=50ms
			{
				gpio_flash_cnt			= 0;
				
				//PF0???
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, ~GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0 )); 
			}
		}
		if(systick_3ms_status)
		{
			systick_3ms_status	= 0;
			flag=(flag+1) % 5;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[p]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}
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
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//Enable PortF
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));			//Wait for the GPIO moduleF ready
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));			//Wait for the GPIO moduleJ ready	
	
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);			//Set PF0 as Output pin
	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1);//Set the PJ0,PJ1 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	
	GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_FALLING_EDGE); //GPIO interrupt for PJ0
  GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0|GPIO_PIN_1); 
  IntEnable(INT_GPIOJ);  
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
	
	while(I2CMasterBusy(I2C0_BASE)){}; //???
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false); //?????,?
	I2CMasterDataPut(I2C0_BASE, RegAddr);	//?????
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START); //??????
	while(I2CMasterBusy(I2C0_BASE)){};
	rop = (uint8_t)I2CMasterErr(I2C0_BASE);

	I2CMasterDataPut(I2C0_BASE, WriteData); //????
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH); //??????,?????
	while(I2CMasterBusy(I2C0_BASE)){};
	rop = (uint8_t)I2CMasterErr(I2C0_BASE);

		return rop;
}

uint8_t I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr)
{
	uint8_t value;
	
	while(I2CMasterBusy(I2C0_BASE)){};	//???
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false); //?????,?
	I2CMasterDataPut(I2C0_BASE, RegAddr); //?????
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_SEND); //??????
	while(I2CMasterBusBusy(I2C0_BASE));
	if (I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
		return 0; //??
	Delay(100);

	//receive data
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, true); //?????,?
	I2CMasterControl(I2C0_BASE,I2C_MASTER_CMD_SINGLE_RECEIVE); //??????
	while(I2CMasterBusBusy(I2C0_BASE));
	if (I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
		return 0; //??
	Delay(100);

	value=I2CMasterDataGet(I2C0_BASE);

	return value;
}

void Delay(uint32_t value)
{
	uint32_t ui32Loop;
	
	for(ui32Loop = 0; ui32Loop < value; ui32Loop++){};
}

void S800_SysTick_Init(void)
{
	SysTickPeriodSet(ui32SysClock/SYSTICK_FREQUENCY); //??1ms
	SysTickEnable();
	SysTickIntEnable();
}

/*
	Corresponding to the startup_TM4C129.s vector table systick interrupt program name
*/
void SysTick_Handler(void)
{
	if (systick_100ms_couter == 0) //??1ms?SysTick??100ms????
	{
		systick_100ms_couter = 100;
		systick_100ms_status = 1;
	}
	else
		systick_100ms_couter--;
	
	if (systick_10ms_couter	== 0) //??1ms?SysTick??10ms????
	{
		systick_10ms_couter	 = 10;
		systick_10ms_status  = 1;
	}
	else
		systick_10ms_couter--;
	if (systick_3ms_couter	== 0) //??1ms?SysTick??5ms????
	{
		systick_3ms_couter	 = 3;
		systick_3ms_status  = 1;
	}
	else
		systick_3ms_couter--;
	if (systick_1s_couter	== 0) //??1ms?SysTick??1s????
	{
		systick_1s_couter	 = 1000;
		systick_1s_status  = 1;
	}
	else
		systick_1s_couter--;
	if (systick_1ms_couter	== 0) //??1ms?SysTick??1ms????
	{
		systick_1ms_couter	 = 1;
		systick_1ms_status  = 1;
	}
	else
		systick_1ms_couter--;
	if (systick_50ms_couter	== 0) //??1ms?SysTick??50ms????
	{
		systick_50ms_couter	 = 50;
		systick_50ms_status  = 1;
	}
	else
		systick_50ms_couter--;
}
void GPIOJ_Handler(void) 	//PortJ????
{
	unsigned long intStatus;

	intStatus = GPIOIntStatus(GPIO_PORTJ_BASE, true); //??????
	GPIOIntClear(GPIO_PORTJ_BASE, intStatus);  //????????

	if (intStatus & GPIO_PIN_0) {	//PJ0?
	     num = 1;
	} 
	if (intStatus & GPIO_PIN_1) {	//PJ0?
	     num = 2;
	} 

}