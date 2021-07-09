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
#include "systick.h"
#include "interrupt.h"
#include "uart.h"
#include "hw_ints.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>


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

#define SYSTICK_FREQUENCY		1000			//1000hz

#define	I2C_FLASHTIME				500				//500mS
#define GPIO_FLASHTIME			500				//500mS

void 		S800_Clock_Init(void);
void 		S800_GPIO_Init(void);
void		S800_I2C0_Init(void);
void 		Delay(uint32_t value);
uint8_t I2C0_WriteByte(uint8_t DevAddr, uint8_t RegAddr, uint8_t WriteData);
uint8_t I2C0_ReadByte(uint8_t DevAddr, uint8_t RegAddr);
void 		S800_SysTick_Init(void);
void 		S800_UART_Init(void);
void 		UARTStringPut(const char *cMessage);
void 		UARTStringPutNonBlocking(const char *cMessage);
uint16_t    chartonum(char);
char numtochar(uint16_t);

uint32_t ui32SysClock;
uint8_t seg7[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x58,0x5e,0x079,0x71,0x5c,0x40};

//systick software counter define
volatile uint16_t systick_10ms_couter=0, systick_100ms_couter=0,systick_1ms_couter=0,systick_1s_couter=0; //10ms?100ms???
volatile uint8_t	systick_10ms_status=0, systick_100ms_status=0,systick_1ms_status=0,systick_1s_status=0; //10ms?100ms????

volatile uint8_t uart_receive_status = 0;

int main(void)
{
	volatile uint8_t result;
	volatile uint8_t cnt=0; 
	volatile uint16_t	i2c_flash_cnt=0;
	char uart_receive_char;
	char p[20]={0};
	uint16_t num=0;
	int16_t a1=0;
	int16_t b1=0;
  char buf1[20]={0};
	uint16_t buf2[20]={0};
	char buf3[20]={0};
  //char buf4[20]={0};
	
	uint16_t flag=0;
	uint16_t a=1;
	uint16_t b=1;
	uint16_t c=5;
	uint16_t d=9;
	uint16_t e=5;
	uint16_t f=9;
	uint16_t q=17;
	
	IntMasterDisable();	//???

	S800_Clock_Init();
	S800_GPIO_Init();
	S800_I2C0_Init();
	S800_SysTick_Init();
	S800_UART_Init();
	
	IntMasterEnable();	//???	
	
	while (1)
	{
		//Task1: uart echo
		if (uart_receive_status)
		{
			GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,GPIO_PIN_1 );	//turn on PN1

			while(UARTCharsAvail(UART0_BASE))    										// Loop while there are characters in the receive FIFO.
			{
				//Read the next character from the UART and write it back to the UART.
				uart_receive_char = UARTCharGetNonBlocking(UART0_BASE);
				p[num]=uart_receive_char;
				num++;
				//UARTCharPutNonBlocking(UART0_BASE,uart_receive_char);
			}
			
			if(p[0]=='S')
			{
				a=chartonum(p[3]);
				b=chartonum(p[4]);
				c=chartonum(p[6]);
				d=chartonum(p[7]);
				e=chartonum(p[9]);
				f=chartonum(p[10]);
				
				buf1[0]=p[3];
				buf1[1]=p[4];
				buf1[2]=':';
				buf1[3]=p[6];
				buf1[4]=p[7];
				buf1[5]=':';
				buf1[6]=p[9];
				buf1[7]=p[10];
				UARTStringPutNonBlocking("TIME");
				UARTStringPutNonBlocking(buf1);
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
				}
				num=0;
			}
			
			if(p[0]=='I')
			{
				buf2[0]=chartonum(p[3]);
				buf2[1]=chartonum(p[4]);
				buf2[2]=chartonum(p[6]);
				buf2[3]=chartonum(p[7]);
				buf2[4]=chartonum(p[9]);
				buf2[5]=chartonum(p[10]);
				
				for(num=0;num<buf2[1]+10*buf2[0];num++)
				{
					if((a<2&&b<9)||(a==2&&b<3)) b++;
					else if(a<2&&b==9)
					{
						b=0;
						a++;
					}					
					else if(a==2&&b==3)
					{
						b=0;
						a=0;
					}
				}
				for(num=0;num<buf2[2];num++)
				{
					if(c<5) c++;
						else if(c==5)
						{
							c=0;
							if((a<2&&b<9)||(a==2&&b<3)) b++;
							else if(a<2&&b==9)
							{
								b=0;
								a++;
							}					
							else if(a==2&&b==3)
							{
								b=0;
						    a=0;
							}
						}
				}
				for(num=0;num<buf2[3];num++)
				{
					if(d<9) d++;
					    else if(d==9)
					    {
					    	d=0;
						    if(c<5) c++;
						    else if(c==5)
						    {
							    c=0;
						    	if(b<3) b++;
							    else if(b==3)
						    	{
							    	b=0;
						    		if(a<2) a++;
								    else if(a==2) a=0;
							    }
						    }
					    }
				}
				for(num=0;num<buf2[4];num++)
				{
					if(e<5) e++;
				else if(e==5)
				{
					e=0;
					if(d<9) d++;
					else if(d==9)
					{
						d=0;
						if(c<5) c++;
						else if(c==5)
						{
							c=0;
							if((a<2&&b<9)||(a==2&&b<3)) b++;
							else if(a<2&&b==9)
							{
								b=0;
								a++;
							}					
							else if(a==2&&b==3)
							{
								b=0;
						    a=0;
							}
						}
					}
				}		
				}
				for(num=0;num<buf2[5];num++)
				{
					if(f<9) f++;
			else if(f==9)
		  {
				f=0;
				if(e<5) e++;
				else if(e==5)
				{
					e=0;
					if(d<9) d++;
					else if(d==9)
					{
						d=0;
						if(c<5) c++;
						else if(c==5)
						{
							c=0;
							if((a<2&&b<9)||(a==2&&b<3)) b++;
							else if(a<2&&b==9)
							{
								b=0;
								a++;
							}					
							else if(a==2&&b==3)
							{
								b=0;
						    a=0;
							}
						}
					}
				}		
			}	
				}
				buf1[0]=numtochar(a);
				buf1[1]=numtochar(b);
				buf1[2]=':';
				buf1[3]=numtochar(c);
				buf1[4]=numtochar(d);
				buf1[5]=':';
				buf1[6]=numtochar(e);
				buf1[7]=numtochar(f);
				UARTStringPutNonBlocking("TIME");
				UARTStringPutNonBlocking(buf1);
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
				}
				num=0;
			}
			
			if(p[0]=='G')
			{
				buf1[0]=numtochar(a);
				buf1[1]=numtochar(b);
				buf1[2]=':';
				buf1[3]=numtochar(c);
				buf1[4]=numtochar(d);
				buf1[5]=':';
				buf1[6]=numtochar(e);
				buf1[7]=numtochar(f);
				UARTStringPutNonBlocking("TIME");
				UARTStringPutNonBlocking(buf1);
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
				}
				num=0;
			}
//			buf3[0]=p[3];
		
//			for(num=0;num<3;num++) buf1[num]=p[num];
//			for(num=0;num<2;num++) buf2[num]=p[num+4];
			
			//UARTStringPut("\r\n");
			GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_1,0 );		//turn off PN1
			//if(a==5) 
			
//			for(num=0;num<20;num++) p[num]=toupper(p[num]);
//			if(strcmp(p,cla)==0) a=1;
//			if(strcmp(p,code)==0) a=2;
//			if(a==0) UARTStringPutNonBlocking("Unknow");
//			if(a==1) UARTStringPutNonBlocking("CLASSF1803202");
//			if(a==2) UARTStringPutNonBlocking("CODE518021910464");
//			for(num=0;num<20;num++) p[num]=0;
			
//			num=0;
			uart_receive_status = 0;
		}
		
		if(systick_1s_status) //clock
		{
			systick_1s_status=0;
			if(f<9) f++;
			else if(f==9)
		  {
				f=0;
				if(e<5) e++;
				else if(e==5)
				{
					e=0;
					if(d<9) d++;
					else if(d==9)
					{
						d=0;
						if(c<5) c++;
						else if(c==5)
						{
							c=0;
							if((a<2&&b<9)||(a==2&&b<3)) b++;
							else if(a<2&&b==9)
							{
								b=0;
								a++;
							}					
							else if(a==2&&b==3)
							{
								b=0;
						    a=0;
							}
						}
					}
				}		
			}	
		}

		//Task2: ???
		if (systick_100ms_status) //100ms???
		{
			systick_100ms_status	= 0; //??100ms????
			
			if (++i2c_flash_cnt		>= I2C_FLASHTIME/100)  //5*100ms=500ms
			{
				i2c_flash_cnt				= 0;

				//??????
				//LED???
				I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(1<<cnt));	
				cnt = (cnt+1) % 8;
			}
		}
		if(systick_1ms_status)
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 8;
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
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
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
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[e]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[f]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}

	}//end while
}

//------------- System Clock -------------------
void S800_Clock_Init(void)
{
	//use internal 16M oscillator, HSI
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_OSC_INT |SYSCTL_USE_OSC), 16000000);		

	//use extern 25M crystal
	//ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |SYSCTL_OSC_MAIN |SYSCTL_USE_OSC), 25000000);		

	//use PLL
	ui32SysClock = SysCtlClockFreqSet((SYSCTL_XTAL_16MHZ |SYSCTL_OSC_INT | SYSCTL_USE_PLL |SYSCTL_CFG_VCO_480), 20000000);
}

//------------- GPIO -------------------
void S800_GPIO_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);						//Enable PortF
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));			//Wait for the GPIO moduleF ready
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOJ);						//Enable PortJ	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOJ));			//Wait for the GPIO moduleJ ready	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);						//Enable PortN	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION));			//Wait for the GPIO moduleN ready		
	
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);			//Set PF0 as Output pin
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);			//Set PN0,PN1 as Output pin

	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1);//Set the PJ0,PJ1 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
}

//-------------- I2C ------------------
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
	while(I2CMasterBusy(I2C0_BASE)){};
	I2CMasterSlaveAddrSet(I2C0_BASE, DevAddr, false);
	I2CMasterDataPut(I2C0_BASE, RegAddr);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_START);
	while(I2CMasterBusy(I2C0_BASE)){};
	rop = (uint8_t)I2CMasterErr(I2C0_BASE);

	I2CMasterDataPut(I2C0_BASE, WriteData);
	I2CMasterControl(I2C0_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
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
	value=I2CMasterDataGet(I2C0_BASE);
	if (I2CMasterErr(I2C0_BASE) != I2C_MASTER_ERR_NONE)
		return 0; //??
	Delay(100);

	return value;
}

void Delay(uint32_t value)
{
	uint32_t ui32Loop;
	
	for(ui32Loop = 0; ui32Loop < value; ui32Loop++){};
}

//--------------- SysTick -----------------
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
	if (systick_1ms_couter	== 0) //??1ms?SysTick??1ms????
	{
		systick_1ms_couter	 = 1;
		systick_1ms_status  = 1;
	}
	else
		systick_1ms_couter--;
	if (systick_1s_couter	== 0) //??1ms?SysTick??1ms????
	{
		systick_1s_couter	 = 1000;
		systick_1s_status  = 1;
	}
	else
		systick_1s_couter--;
	if (GPIOPinRead(GPIO_PORTJ_BASE,GPIO_PIN_0) == 0)
	{
		systick_100ms_status	= systick_10ms_status = 0; //????1?2???
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0,GPIO_PIN_0);		//??PN0
	}
	else
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0,0);		//??PN0

}

//----------- UART ---------------------
void S800_UART_Init(void)
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);						//Enable PortA
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));			//Wait for the GPIO moduleA ready

	GPIOPinConfigure(GPIO_PA0_U0RX);												// Set GPIO A0 and A1 as UART pins.
  GPIOPinConfigure(GPIO_PA1_U0TX);    			

  GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	// Configure the UART for 115,200, 8-N-1 operation.
  UARTConfigSetExpClk(UART0_BASE, ui32SysClock,115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |UART_CONFIG_PAR_NONE));

	UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX2_8,UART_FIFO_RX7_8);//set FIFO Level  #####

  UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);	//Enable UART0 RX,TX interrupt
	IntEnable(INT_UART0);
	
	UARTStringPut("\r\nHello, world!\r\n");
}

void UARTStringPut(const char *cMessage)
{
	while(*cMessage!='\0')
		UARTCharPut(UART0_BASE,*(cMessage++));
}

void UARTStringPutNonBlocking(const char *cMessage)
{
	while(*cMessage!='\0')
		UARTCharPutNonBlocking(UART0_BASE,*(cMessage++));
}

/*
	Corresponding to the startup_TM4C129.s vector table UART0_Handler interrupt program name
*/
void UART0_Handler(void)
{
	char uart_receive_char;
	int32_t uart0_int_status;
//	char uart_receive_char;
	
  uart0_int_status = UARTIntStatus(UART0_BASE, true);			// Get the interrrupt status.
  UARTIntClear(UART0_BASE, uart0_int_status);							//Clear the asserted interrupts

	if (uart0_int_status & (UART_INT_RX | UART_INT_RT)) 	//???????
	{
		uart_receive_status = 1;
	}
	
}

uint16_t chartonum(char a)
{
	uint16_t b;
	if(a=='0') b=0;
	if(a=='1') b=1;
	if(a=='2') b=2;
	if(a=='3') b=3;
	if(a=='4') b=4;
	if(a=='5') b=5;
	if(a=='6') b=6;
	if(a=='7') b=7;
	if(a=='8') b=8;
	if(a=='9') b=9;
	return b;
}

char numtochar(uint16_t a)
{
	char b;
	if(a==0) b='0';
	if(a==1) b='1';
	if(a==2) b='2';
	if(a==3) b='3';
	if(a==4) b='4';
	if(a==5) b='5';
	if(a==6) b='6';
	if(a==7) b='7';
	if(a==8) b='8';
	if(a==9) b='9';
	return b;
}
