#include <stdio.h>

int Sum(int);
int LedOnOff(void);

int main(void)						//主函数
{  
	int x;

//	__asm volatile("MSR	CONTROL, 3"); //特权->用户

	__asm volatile("cpsie i");  /* enable IRQ interrupts, PRIMASK置0*/
	__asm volatile("cpsid i");  /* disable IRQ interrupts, PRIMASK置1 */

//	__enable_irq();	  /* enable IRQ interrupts, PRIMASK置0, CMSIS-Core函数 */
//	__disable_irq();	/* disable IRQ interrupts, PRIMASK置1, CMSIS-Core函数 */	
	
	x = Sum(10);

	__asm volatile("MRS	x, APSR"); //APSR->x

	LedOnOff();

}
