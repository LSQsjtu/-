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
#include "pwm.h"

#include "adc.h"
#include <stdlib.h>
//#include "uartstdio.h"


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
void    S800_PWM_Init(uint32_t) ;
void    TEM_Init(void);
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
void ledflash(uint8_t*,uint8_t,uint16_t *);

//void led(uint8_t*,uint8_t);

uint32_t ui32SysClock;
uint8_t seg7[] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x58,0x5e,0x079,0x71,0x5c,0x40};

//systick software counter define
volatile uint16_t systick_10ms_couter=0, systick_100ms_couter=0,systick_1ms_couter=0,systick_1s_couter=0,systick_1ms_couter1=0,systick_500ms_couter=0,systick_1ms_couter2=0; //10ms?100ms???
volatile uint8_t	systick_10ms_status=0, systick_100ms_status=0,systick_1ms_status=0,systick_1s_status=0,systick_1ms_status1=0,systick_500ms_status=0,systick_1ms_status2=0; //10ms?100ms????
volatile uint16_t systick_200ms_couter=0,systick_500ms_couter1=0,systick_500ms_couter2=0,systick_250ms_couter=0,systick_250ms_couter1=0;
volatile uint8_t	systick_200ms_status=0,systick_500ms_status1=0,systick_500ms_status2=0,systick_250ms_status=0,systick_250ms_status1=0;
	
volatile uint8_t uart_receive_status = 0;
uint16_t stop=0;

int main(void)
{
	volatile uint8_t result;
	uint8_t cnt=0; 
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
	uint16_t cntnum=1;
	uint16_t o=0;//ledflash control
	uint16_t o1=0;
	uint16_t o2=0;
	uint16_t o3=0;
	
	uint16_t delaytime=0;
	uint16_t delaytime1=0;
	uint16_t delaytime2=0;
	uint16_t delaytime3=0;
	uint16_t delaytime4=0;
	uint16_t delaytime5=0;
	uint16_t delaytime6=0;
	uint16_t delaytime7=0;
	uint16_t delaytime0=0;
	
	uint16_t whether12hour=0;
	uint16_t PM=0;
	uint16_t PM1=0;
	uint16_t PM2=0;
	
	uint8_t key;//SW1~8
	uint8_t flag1=0;//if tomorrow
	
	uint8_t SW2_status=0;//decide flash
	uint8_t SW2_flash=0;
	uint8_t SW3_status=0;//decide flash
	uint8_t SW3_flash=0;
	uint8_t SW4_status=0;//decide flash
	uint8_t SW4_flash=0;
	uint8_t SW5_status=0;//decide flash
	uint8_t SW5_flash=0;
	
	uint8_t alarm1enable=0;
  uint8_t alarm2enable=0;
	
	uint16_t alarm1_a=0;
	uint16_t alarm1_b=0;
	uint16_t alarm1_c=0;
	uint16_t alarm1_d=0;
	uint16_t alarm1_a_show=0;
	uint16_t alarm1_b_show=0;
	uint16_t alarm1_c_show=0;
	uint16_t alarm1_d_show=0;
	uint16_t alarm1timecount=0;
	uint16_t alarm1flash=0;
	uint16_t alarm2_a=0;
	uint16_t alarm2_b=0;
	uint16_t alarm2_c=0;
	uint16_t alarm2_d=0;
	uint16_t alarm2_a_show=0;
	uint16_t alarm2_b_show=0;
	uint16_t alarm2_c_show=0;
	uint16_t alarm2_d_show=0;
	uint16_t alarm2timecount=0;
	uint16_t alarm2flash=0;
	
	uint16_t a_12_show=0;
	uint16_t b_12_show=0;
	uint16_t alarm1_a_12_show=0;
	uint16_t alarm1_b_12_show=0;
	uint16_t alarm2_a_12_show=0;
	uint16_t alarm2_b_12_show=0;
	
	uint16_t flag=0;
	uint16_t a=1;
	uint16_t b=1;
	uint16_t c=5;
	uint16_t d=9;
	uint16_t e=5;
	uint16_t f=9;
	uint16_t q=17;
	
	uint16_t g=2;
	uint16_t h=0;
	uint16_t i=2;
	uint16_t j=0;
	uint16_t k=0;
	uint16_t l=6;
	uint16_t m=0;
	uint16_t n=1;

  uint32_t pui32ADC0Value[1];//温度
	double ui32TempValueC;
	int tmp1=0;
	int tmp2=0;
	
	IntMasterDisable();	//???

	S800_Clock_Init();
	S800_GPIO_Init();
	
	S800_I2C0_Init();
	S800_SysTick_Init();
	S800_UART_Init();
	TEM_Init();
	
	IntMasterEnable();	//???	
	
	while(num<600)//?????1? 1000???????????1?,?????
	{
		if(systick_1ms_status)
		{
			num++;
			systick_1ms_status	= 0;
			flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[2]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[1]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[9]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[1]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[0]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[4]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[6]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[4]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}
	}
	num=0;
	flag=0;

	
	while (1)
	{
		key = ~I2C0_ReadByte(TCA6424_I2CADDR, TCA6424_INPUT_PORT0);
		
		
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
				
//串口功能		
			if(p[0]=='G'&&p[4]=='T'&&p[5]=='I')
			{
				buf1[0]=numtochar(a);
				buf1[1]=numtochar(b);
				buf1[2]=':';
				buf1[3]=numtochar(c);
				buf1[4]=numtochar(d);
				buf1[5]=':';
				buf1[6]=numtochar(e);
				buf1[7]=numtochar(f);
//				UARTStringPutNonBlocking("TIME");
				UARTStringPutNonBlocking(buf1);
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			else if(p[0]=='G'&&p[4]=='D')
			{
				buf1[0]=numtochar(g);
				buf1[1]=numtochar(h);
				buf1[2]=numtochar(i);;
				buf1[3]=numtochar(j);
				buf1[4]=numtochar(k);
				buf1[5]=numtochar(l);
				buf1[6]=numtochar(m);
				buf1[7]=numtochar(n);
//				UARTStringPutNonBlocking("TIME");
				UARTStringPutNonBlocking(buf1);
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			else if(p[0]=='D'&&p[1]=='S')
			{
				k=chartonum(p[5]);
				l=chartonum(p[6]);
				m=chartonum(p[7]);
				n=chartonum(p[8]);
				UARTStringPutNonBlocking("SUCCESSFUL");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			else if(p[0]=='T'&&p[1]=='S')//设置时间的命令一律不带：
			{
				a=chartonum(p[5]);
				b=chartonum(p[6]);
				c=chartonum(p[7]);
				d=chartonum(p[8]);
				UARTStringPutNonBlocking("SUCCESSFUL");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			else if(p[0]=='R'&&p[1]=='E')
			{
				SysCtlReset();
				UARTStringPutNonBlocking("SUCCESSFUL");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			else if(p[0]=='S'&&p[7]=='1')
			{
				alarm1_a_show=chartonum(p[9]);
				alarm1_b_show=chartonum(p[10]);
				alarm1_c_show=chartonum(p[11]);
				alarm1_d_show=chartonum(p[12]);
				if(alarm1enable==0) alarm1enable=1;
				alarm1_a=alarm1_a_show;
				alarm1_b=alarm1_b_show;
				alarm1_c=alarm1_c_show;
				alarm1_d=alarm1_d_show;
				UARTStringPutNonBlocking("SUCCESSFUL");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			else if(p[0]=='S'&&p[7]=='2')
			{
				alarm2_a_show=chartonum(p[9]);
				alarm2_b_show=chartonum(p[10]);
				alarm2_c_show=chartonum(p[11]);
				alarm2_d_show=chartonum(p[12]);
				if(alarm2enable==0) alarm2enable=1;
				alarm2_a=alarm2_a_show;
				alarm2_b=alarm2_b_show;
				alarm2_c=alarm2_c_show;
				alarm2_d=alarm2_d_show;
				UARTStringPutNonBlocking("SUCCESSFUL");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			else if(p[0]=='G'&&p[4]=='T'&&p[5]=='E')//扩展温度显示
			{
				ADCProcessorTrigger(ADC0_BASE, 3);
				while(!ADCIntStatus(ADC0_BASE, 3, false));
				ADCIntClear(ADC0_BASE, 3);
				ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);
				ui32TempValueC = 147.5-((75*3.3*pui32ADC0Value[0])/4096);
//				sprintf(buf1,"%3f", ui32TempValueC);
//				UARTprintf("%3f", ui32TempValueC);
//				itoa(ui32TempValueC,buf1,10);
				tmp1=ui32TempValueC;
				sprintf(buf1,"%2d",tmp1);
   			UARTStringPutNonBlocking(buf1);
				ui32TempValueC=(ui32TempValueC-tmp1)*100;
				tmp2=ui32TempValueC;
				if(tmp2>=10){
				sprintf(buf3,"%2d",tmp2);
//				if(buf3[0]==0) buf3[0]='0';
//				if(buf3[1]==0) buf3[1]='0';
				UARTStringPutNonBlocking(".");
				UARTStringPutNonBlocking(buf3);
				UARTStringPutNonBlocking("C");}
			  else if(tmp2>=1){
				  sprintf(buf3,"%1d",tmp2);
					UARTStringPutNonBlocking(".0");
					UARTStringPutNonBlocking(buf3);
				  UARTStringPutNonBlocking("C");
				}
				else  UARTStringPutNonBlocking(".00C");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
				ui32TempValueC=0;
				tmp1=0;
				tmp2=0;
			}
			
			else
			{
				UARTStringPutNonBlocking("UNKNOWN ORDER");
				for(num=0;num<20;num++)
				{
					p[num]=0;
					buf1[num]=0;
					buf2[num]=0;
					buf3[num]=0;
				}
				num=0;
			}
			
			
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
		
		if(systick_1s_status) //正常时钟模块
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
								flag1=1;
							}
						}
					}
				}		
			}	
		}
		
		
		if(flag1)//日期变换模块
		{
			flag1=0;
			if((k==0&&l==1)||(k==0&&l==3)||(k==0&&l==5)||(k==0&&l==7)||(k==0&&l==8)||(k==1&&l==0))
			{
				if(m<3&&n<9) n++;
				else if(m<3&&n==9)
				{
					n=0;
					m++;
				}
				else if(m==3&&n==0) n++;
				else if(m==3&&n==1)
				{
					m=0;
					l++;
				}
			}
			else if((k==0&&l==4)||(k==0&&l==6)||(k==1&&l==1))
			{
				if(m<3&&n<9) n++;
				else if(m<3&&n==9)
				{
					n=0;
					m++;
				}
				else if(m==3&&n>=0)
				{
					m=0;
					n=1;
					l++;
				}
			}
			else if(k==0&&l==2)//2?
			{
				if(m<2&&n<9) n++;
				else if(m<2&&n==9)
				{
					n=0;
					m++;
				}
				else if(m==2&&n<8) n++;
				else if(m==2&&n>=8)
				{
					m=0;
					n=1;
					l++;
				}
			}
			else if(k==0&&l==9)//9?
			{
				if(m<3&&n<9) n++;
				else if(m<3&&n==9)
				{
					n=0;
					m++;
				}
				else if(m==3&&n>=0)
				{
					m=0;
					n=1;
					l=0;
					k=1;
				}
			}
			else if(k==1&&l==2)//12?
			{
				if(m<3&&n<9) n++;
				else if(m<3&&n==9)
				{
					n=0;
					m++;
				}
				else if(m==3&&n==0) n++;
				else if(m==3&&n==1)
				{
					m=0;
					k=0;
					l=1;
				}
			}
			
			
		}
		
		
		
		if(systick_1ms_status1)//按键控制模块
		{
			systick_1ms_status1=0;
			
			if(key & 0x01) delaytime++;//SW1
			else delaytime=0;
			if(delaytime==100) 
			{
				delaytime=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??
				}
				else{
				cntnum=cntnum%6+1;
				SW2_status=0;
				SW3_status=0;
				SW4_status=0;
				SW5_status=0;}
			}
			
			
			if(key & 0x02) delaytime1++;//SW2
			else delaytime1=0;
			if(delaytime1==100)
			{
				delaytime1=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??
				}
				else{
				if(cntnum==5)
				{
					SW3_status=0;
					SW2_status=1;
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
					  flag1=1;
				  }
				}
				if(cntnum==3)
				{
					if((alarm1_a_show<2&&alarm1_b_show<9)||(alarm1_a_show==2&&alarm1_b_show<3)) alarm1_b_show++;
				  else if(alarm1_a_show<2&&alarm1_b_show==9)
				  {
					  alarm1_b_show=0;
					  alarm1_a_show++;
				  }					
				  else if(alarm1_a_show==2&&alarm1_b_show==3)
				  {
					  alarm1_a_show=0;
				    alarm1_b_show=0;
				  }
				}
				if(cntnum==4)
				{
					if((alarm2_a_show<2&&alarm2_b_show<9)||(alarm2_a_show==2&&alarm2_b_show<3)) alarm2_b_show++;
				  else if(alarm2_a_show<2&&alarm2_b_show==9)
				  {
					  alarm2_b_show=0;
					  alarm2_a_show++;
				  }					
				  else if(alarm2_a_show==2&&alarm2_b_show==3)
				  {
					  alarm2_a_show=0;
				    alarm2_b_show=0;
				  }
				}
			}}
			
			
			if(key & 0x04) delaytime2++;//SW3
			else delaytime2=0;
			if(delaytime2==100)
			{
				delaytime2=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??
				}
				else{
				if(cntnum==5)
				{
					SW2_status=0;
					SW3_status=1;
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
								flag1=1;
							}
						}
					}
				}
				if(cntnum==3)
				{
					if(alarm1_d_show<9) alarm1_d_show++;
					else if(alarm1_d_show==9)
					{
						alarm1_d_show=0;
						if(alarm1_c_show<5) alarm1_c_show++;
						else if(alarm1_c_show==5)
						{
							alarm1_c_show=0;
							if((alarm1_a_show<2&&alarm1_b_show<9)||(alarm1_a_show==2&&alarm1_b_show<3)) alarm1_b_show++;
				      else if(alarm1_a_show<2&&alarm1_b_show==9)
				      {
					      alarm1_b_show=0;
					      alarm1_a_show++;
				      }					
				      else if(alarm1_a_show==2&&alarm1_b_show==3)
				      {
					      alarm1_a_show=0;
				        alarm1_b_show=0;
				      }
						}
					}
				}
				if(cntnum==4)
				{
					if(alarm2_d_show<9) alarm2_d_show++;
					else if(alarm2_d_show==9)
					{
						alarm2_d_show=0;
						if(alarm2_c_show<5) alarm2_c_show++;
						else if(alarm2_c_show==5)
						{
							alarm2_c_show=0;
							if((alarm2_a_show<2&&alarm2_b_show<9)||(alarm2_a_show==2&&alarm2_b_show<3)) alarm2_b_show++;
				      else if(alarm2_a_show<2&&alarm2_b_show==9)
				      {
					      alarm2_b_show=0;
					      alarm2_a_show++;
				      }					
				      else if(alarm2_a_show==2&&alarm2_b_show==3)
				      {
					      alarm2_a_show=0;
				        alarm2_b_show=0;
				      }
						}
					}
				}
			}}
			
			if(key & 0x08) delaytime3++;//SW4
			else delaytime3=0;
			if(delaytime3==100)
			{
				delaytime3=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??

				}
				else{
				if(cntnum==6)
				{
					SW4_status=1;
					SW5_status=0;
					if(k==0&&l<9) l++;
					else if(k==0&&l==9)
					{
						k=1;
						l=0;
					}
					else if(k==1&&l<2) l++;
					else if(k==1&&l==2)
					{
						k=0;
						l=1;
					}	
				}
			}}
			
			if(key & 0x10) delaytime4++;//SW5
			else delaytime4=0;
			if(delaytime4==100)
			{
				delaytime4=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??
				}
				else{
				if(cntnum==6)
				{
					SW4_status=0;
					SW5_status=1;
					flag1=1;
				}
			}}
			
			
			if(key & 0x20) delaytime5++;//SW6
			else delaytime5=0;
			if(delaytime5==100)
			{
				delaytime5=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??

				}
				else{
//				if(cntnum==3)
//				{
					if(alarm1enable==0) alarm1enable=1;
					else alarm1enable=0;
					if(alarm1enable)
					{
						alarm1_a=alarm1_a_show;
						alarm1_b=alarm1_b_show;
						alarm1_c=alarm1_c_show;
						alarm1_d=alarm1_d_show;
					}
//				}
			}}
			
			
			if(key & 0x40) delaytime6++;//SW7
			else delaytime6=0;
			if(delaytime6==100)
			{
				delaytime6=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??
				}
				else{
//				if(cntnum==4)
//				{
					if(alarm2enable==0) alarm2enable=1;
					else alarm2enable=0;
					if(alarm2enable)
					{
						alarm2_a=alarm2_a_show;
						alarm2_b=alarm2_b_show;
						alarm2_c=alarm2_c_show;
						alarm2_d=alarm2_d_show;
					}
//				}
			}}
			
			if(key & 0x80) delaytime7++;//SW8
			else delaytime7=0;
			if(delaytime7==100)
			{
				delaytime7=0;
				if(alarm1timecount==1||alarm2timecount==1)
				{
					alarm1timecount=0;
					alarm2timecount=0;
					S800_PWM_Init(0);
					PWMGenDisable(PWM0_BASE,PWM_GEN_3);//??PWM??
				}
				else
				{
					if(whether12hour==0) whether12hour=1;
					else whether12hour=0;
				}
			}
			
			
			if(key==0) delaytime0++;//不按键就返回时间显示
			else delaytime0=0;
			if(delaytime0==2600)//5000ms
			{
				delaytime0=0;
				cntnum=1;
			}
			
	}
		
	  
	
	
	  if((alarm1enable==1)&&(alarm1_a==a)&&(alarm1_b==b)&&(alarm1_c==c)&&(alarm1_d==d)) 
		{
			alarm1timecount=1;//??1?????
			alarm2timecount=0;//?????????
			alarm1enable=0;
			S800_PWM_Init(40000);//2khz
			stop=0;
		}
	  if((alarm2enable==1)&&(alarm2_a==a)&&(alarm2_b==b)&&(alarm2_c==c)&&(alarm2_d==d)) 
		{
			alarm2timecount=1;//??2?????
			alarm1timecount=0;
			alarm2enable=0;
			S800_PWM_Init(20000);//4khz
			stop=0;
		}
		
		
	  if(!stop){
		if(!whether12hour){
			GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
	  if(alarm1timecount)//??1?,???????
		{
			ledflash(&systick_500ms_status1,1,&o1);
			if(systick_500ms_status)
			{
				systick_500ms_status=0;
				if(alarm1flash==0) alarm1flash=1;
				else alarm1flash=0;
			}
			if(systick_1ms_status)	
			{
				systick_1ms_status=0;
			if(alarm1flash==1)
				{
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
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[e]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[f]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
		}
				}
			}
		
		else if(alarm2timecount)
		{
			ledflash(&systick_250ms_status1,2,&o2);
			if(systick_250ms_status)
			{
				systick_250ms_status=0;
				if(alarm2flash==0) alarm2flash=1;
				else alarm2flash=0;
			}
			if(systick_1ms_status)	
			{
				systick_1ms_status=0;
			if(alarm2flash==1)
				{
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
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[e]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[f]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
		}
				}
		}
		
	

	else if(alarm1timecount==0&&alarm2timecount==0)
  {		
		switch(cntnum)
		{
			case 1:
				//led
				if(alarm1enable==1&&alarm2enable==0) ledflash(&systick_500ms_status1,1,&o1);
			  if(alarm1enable==0&&alarm2enable==0) I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(0));
			  if(alarm2enable==1&&alarm1enable==0) ledflash(&systick_500ms_status2,2,&o2);
			  if(alarm2enable==1&&alarm1enable==1)
				{
					if (systick_500ms_status1) //100ms???
		      {
		      	systick_500ms_status1	= 0; //??100ms????
			      if(o3==1) o3=0;
            else o3=1;		
		      }	
			    if(o3) I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(3));	
			    else I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(0));
				}
				
				
		if(systick_1ms_status)// show time
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
				break;
		
			case 2:
				if(systick_1ms_status)// show date
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}
				break;
		
			case 3://??1??
					I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(1));	//led1??
					if(systick_1ms_status)// show alarm1 time
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 4;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_a_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_b_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_c_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_d_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}
					break;
					
				
			case 4://??2??
					I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(2));	//led2??
			    if(systick_1ms_status)// show alarm1 time
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 4;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_a_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_b_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_c_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_d_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
	    }
		}
					break;
					
					
			case 5:
				ledflash(&systick_200ms_status,3,&o);
		if(systick_1ms_status)// show time
		{
			systick_1ms_status	= 0;
			if((!SW2_status)&&(!SW3_status))
			{
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
			else if(SW2_status)
			{
				if(SW2_flash<50)
				{
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
			   SW2_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
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
			SW2_flash++;
			if(SW2_flash>100) SW2_flash=0;
				}
			}
			
			else if(SW3_status)
			{
				if(SW3_flash<50)
				{
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
			   SW3_flash++;
				}
				else
				{
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
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
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
			SW3_flash++;
			if(SW3_flash>100) SW3_flash=0;
				}
			}
			} break;
		
				case 6:
					ledflash(&systick_200ms_status,4,&o);
					if(systick_1ms_status)// show time
		{
			systick_1ms_status	= 0;
			if((!SW4_status)&&(!SW5_status))
			{
				flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			}
			else if(SW4_status)
			{
				if(SW4_flash<50)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			   SW4_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			SW4_flash++;
			if(SW4_flash>100) SW4_flash=0;
				}
			}
			
			else if(SW5_status)
			{
				if(SW5_flash<50)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			   SW5_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
			SW5_flash++;
			if(SW5_flash>100) SW5_flash=0;
				}
			}
			}break;
					
					
			}
		}}
		else{
			//I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(16));
			if(a*10+b>=12)
			{
				PM=1;
				if(a==1&&b==2)
				{
					a_12_show=a;
					b_12_show=b;
				}
				else if(a==1&&b>2)
				{
					a_12_show=0;
					b_12_show=b-2;
				}
				else if(a==2&&b<2)
				{
					a_12_show=0;
					b_12_show=b+8;
				}
				else if(a==2&&b<4)
				{
					a_12_show=1;
					b_12_show=b-2;
				}
			}
			else
			{
				PM=0;
				if(a==0&&b==0)
				{
					a_12_show=1;
					b_12_show=2;
				}
				else
				{
					a_12_show=a;
					b_12_show=b;
				}
			}
			
			if(alarm1_a_show*10+alarm1_b_show>=12)
			{
				PM1=1;
				if(alarm1_a_show==1&&alarm1_b_show==2)
				{
					alarm1_a_12_show=alarm1_a_show;
					alarm1_b_12_show=alarm1_b_show;
				}
				else if(alarm1_a_show==1&&alarm1_b_show>2)
				{
					alarm1_a_12_show=0;
					alarm1_b_12_show=alarm1_b_show-2;
				}
				else if(alarm1_a_show==2&&alarm1_b_show<2)
				{
					alarm1_a_12_show=0;
					alarm1_b_12_show=alarm1_b_show+8;
				}
				else if(alarm1_a_show==2&&alarm1_b_show<4)
				{
					alarm1_a_12_show=1;
					alarm1_b_12_show=alarm1_b_show-2;
				}
			}
			else
			{
				PM1=0;
				if(alarm1_a_show==0&&alarm1_b_show==0)
				{
					alarm1_a_12_show=1;
					alarm1_b_12_show=2;
				}
				else
				{
					alarm1_a_12_show=alarm1_a_show;
					alarm1_b_12_show=alarm1_b_show;
				}
			}
			
			if(alarm2_a_show*10+alarm2_b_show>=12)
			{
				PM2=1;
				if(alarm2_a_show==1&&alarm2_b_show==2)
				{
					alarm2_a_12_show=alarm2_a_show;
					alarm2_b_12_show=alarm2_b_show;
				}
				else if(alarm2_a_show==1&&alarm2_b_show>2)
				{
					alarm2_a_12_show=0;
					alarm2_b_12_show=alarm2_b_show-2;
				}
				else if(alarm2_a_show==2&&alarm2_b_show<2)
				{
					alarm2_a_12_show=0;
					alarm2_b_12_show=alarm2_b_show+8;
				}
				else if(alarm2_a_show==2&&alarm2_b_show<4)
				{
					alarm2_a_12_show=1;
					alarm2_b_12_show=alarm2_b_show-2;
				}
			}
			else
			{
				PM2=0;
				if(alarm2_a_show==0&&alarm2_b_show==0)
				{
					alarm2_a_12_show=1;
					alarm2_b_12_show=2;
				}
				else
				{
					alarm2_a_12_show=alarm2_a_show;
					alarm2_b_12_show=alarm2_b_show;
				}
			}
			if(alarm1timecount)//??1?,???????
		{
			if(PM) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
				
			ledflash(&systick_500ms_status1,1,&o1);
			if(systick_500ms_status)
			{
				systick_500ms_status=0;
				if(alarm1flash==0) alarm1flash=1;
				else alarm1flash=0;
			}
			if(systick_1ms_status)	
			{
				systick_1ms_status=0;
			if(alarm1flash==1)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
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
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[e]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[f]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
		}
				}
			}
		
		else if(alarm2timecount)
		{
			if(PM) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
			ledflash(&systick_250ms_status1,2,&o2);
			if(systick_250ms_status)
			{
				systick_250ms_status=0;
				if(alarm2flash==0) alarm2flash=1;
				else alarm2flash=0;
			}
			if(systick_1ms_status)	
			{
				systick_1ms_status=0;
			if(alarm2flash==1)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
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
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[e]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[f]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
		}
				}
		}
		
	

	else if(alarm1timecount==0&&alarm2timecount==0)
  {		
		switch(cntnum)
		{
			case 1:
				//led
				if(alarm1enable==1&&alarm2enable==0) ledflash(&systick_500ms_status1,1,&o1);
			  if(alarm1enable==0&&alarm2enable==0) I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(0));
			  if(alarm2enable==1&&alarm1enable==0) ledflash(&systick_500ms_status2,2,&o2);
			  if(alarm2enable==1&&alarm1enable==1)
				{
					if (systick_500ms_status1) //100ms???
		      {
		      	systick_500ms_status1	= 0; //??100ms????
			      if(o3==1) o3=0;
            else o3=1;		
		      }	
			    if(o3) I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(3));	
			    else I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(0));
				}
				
			if(PM) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
				
		if(systick_1ms_status)// show time
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
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
				break;
		
			case 2:
				if(PM) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			  else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
			
				if(systick_1ms_status)// show date
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}
				break;
		
			case 3://??1??
					I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(1));	//led1??
			
			    if(PM1) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			    else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
			
					if(systick_1ms_status)// show alarm1 time
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 4;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_c_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm1_d_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
		}
					break;
					
				
			case 4://??2??
					I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(2));	//led2??
			
			    if(PM2) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			    else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
			
			    if(systick_1ms_status)// show alarm1 time
		{
			systick_1ms_status	= 0;
			flag=(flag+1) % 4;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_c_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[alarm2_d_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<(flag+4));				//write port 2
				  break;
	    }
		}
					break;
					
					
			case 5:
				ledflash(&systick_200ms_status,3,&o);
			
			  if(PM) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			  else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
			
		if(systick_1ms_status)// show time
		{
			systick_1ms_status	= 0;
			if((!SW2_status)&&(!SW3_status))
			{
				flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
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
			else if(SW2_status)
			{
				if(SW2_flash<50)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
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
			   SW2_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
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
			SW2_flash++;
			if(SW2_flash>100) SW2_flash=0;
				}
			}
			
			else if(SW3_status)
			{
				if(SW3_flash<50)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
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
			   SW3_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
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
			SW3_flash++;
			if(SW3_flash>100) SW3_flash=0;
				}
			}
			} break;
		
				case 6:
					ledflash(&systick_200ms_status,4,&o);
				
				  if(PM) GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,GPIO_PIN_1);
			    else GPIOPinWrite(GPIO_PORTN_BASE,GPIO_PIN_1,0);
				
					if(systick_1ms_status)// show time
		{
			systick_1ms_status	= 0;
			if((!SW4_status)&&(!SW5_status))
			{
				flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			}
			else if(SW4_status)
			{
				if(SW4_flash<50)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			   SW4_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			SW4_flash++;
			if(SW4_flash>100) SW4_flash=0;
				}
			}
			
			else if(SW5_status)
			{
				if(SW5_flash<50)
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	    }
			   SW5_flash++;
				}
				else
				{
					flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[g]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[h]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[i]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[j]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[k]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[l]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,1<<flag);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[m]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[n]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
			SW5_flash++;
			if(SW5_flash>100) SW5_flash=0;
				}
			}
			}break;
					
					
			}
		}}		
		
		
	
		
	}//end while
		else
		{
			flag=(flag+1) % 8;
			result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
			switch(flag)
	    {
		    case 0: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[a_12_show]);	//write port 1 						
  				result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	      case 1: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[b_12_show]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 2:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 3: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[c]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
		    case 4: 
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[d]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 5:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[q]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 6:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[e]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
				case 7:
					result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT1,seg7[f]);	//write port 1 						
				  result = I2C0_WriteByte(TCA6424_I2CADDR,TCA6424_OUTPUT_PORT2,0);				//write port 2
				  break;
	    }
		}
}}
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
//	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);						//Enable PortK
//	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));			//Wait for the GPIO moduleK ready	
	
  GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);			//Set PF0 as Output pin	
  GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);			//Set PN0,PN1 as Output pin
//  GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_5);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK));
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_5);
	//GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_5, 0x0);
	GPIOPinConfigure(GPIO_PK5_M0PWM7);//PWM
  GPIOPinTypePWM(GPIO_PORTK_BASE, GPIO_PIN_5);

	GPIOPinTypeGPIOInput(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1);//Set the PJ0,PJ1 as input pin
	GPIOPadConfigSet(GPIO_PORTJ_BASE,GPIO_PIN_0 | GPIO_PIN_1,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	
	GPIOIntTypeSet(GPIO_PORTJ_BASE, GPIO_PIN_0, GPIO_FALLING_EDGE); 
  GPIOIntEnable(GPIO_PORTJ_BASE, GPIO_PIN_0); 
  IntEnable(INT_GPIOJ);  
	
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
	if (systick_1ms_couter1	== 0) //??1ms?SysTick??1ms????
	{
		systick_1ms_couter1	 = 1;
		systick_1ms_status1  = 1;
	}
	else
		systick_1ms_couter1--;
	if (systick_500ms_couter	== 0) //??1ms?SysTick??500ms????
	{
		systick_500ms_couter	 = 500;
		systick_500ms_status  = 1;
	}
	else
		systick_500ms_couter--;
	if (systick_500ms_couter1	== 0) //??1ms?SysTick??500ms????
	{
		systick_500ms_couter1	 = 500;
		systick_500ms_status1  = 1;
	}
	else
		systick_500ms_couter1--;
	if (systick_500ms_couter2	== 0) //??1ms?SysTick??500ms????
	{
		systick_500ms_couter2	 = 500;
		systick_500ms_status2  = 1;
	}
	else
		systick_500ms_couter2--;
	if (systick_250ms_couter	== 0) //??1ms?SysTick??250ms????
	{
		systick_250ms_couter	 = 250;
		systick_250ms_status  = 1;
	}
	else
		systick_250ms_couter--;
	if (systick_250ms_couter1	== 0) //??1ms?SysTick??250ms????
	{
		systick_250ms_couter1	 = 250;
		systick_250ms_status1  = 1;
	}
	else
		systick_250ms_couter1--;
	if (systick_200ms_couter	== 0) //??1ms?SysTick??200ms????
	{
		systick_200ms_couter	 = 200;
		systick_200ms_status  = 1;
	}
	else
		systick_200ms_couter--;
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
void S800_PWM_Init(uint32_t BeepPeriod) 
{ 
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_PWM0));
    PWMClockSet(PWM0_BASE, PWM_SYSCLK_DIV_1);
    PWMGenConfigure(PWM0_BASE,PWM_GEN_3, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenPeriodSet(PWM0_BASE,PWM_GEN_3,BeepPeriod);
    PWMPulseWidthSet(PWM0_BASE,PWM_OUT_7,BeepPeriod/4);
    PWMOutputState(PWM0_BASE,PWM_OUT_7_BIT,true);   //??PWM??
    PWMGenEnable(PWM0_BASE,PWM_GEN_3);              //??PWM???
}

void TEM_Init(void)
{
		SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_TS | ADC_CTL_IE |ADC_CTL_END);
    ADCSequenceEnable(ADC0_BASE, 3);
    ADCIntClear(ADC0_BASE, 3);
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

void ledflash(uint8_t *freq, uint8_t cnt,uint16_t *o)
{
	if (*freq) //100ms???
		{
			*freq	= 0; //??100ms????
			if(*o==1) *o=0;
      else *o=1;		
		}	
		
			if(*o==1) I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(1<<(cnt-1)));	
			else I2C0_WriteByte(PCA9557_I2CADDR,PCA9557_OUTPUT,~(0));
//				cnt = (cnt+1) % 8;			
}

void GPIOJ_Handler(void) 	//PortJ????
{
	unsigned long intStatus;

	intStatus = GPIOIntStatus(GPIO_PORTJ_BASE, true); //??????
	GPIOIntClear(GPIO_PORTJ_BASE, intStatus );  //????????

	if (intStatus & GPIO_PIN_0) {	//PJ0?
	   if(stop==1) stop=0;
		 else stop=1;
	} 

}
