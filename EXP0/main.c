#include <stdio.h>

int Sum(int);
int LedOnOff(void);

int main(void)						//������
{  
	int x;

//	__asm volatile("MSR	CONTROL, 3"); //��Ȩ->�û�

	__asm volatile("cpsie i");  /* enable IRQ interrupts, PRIMASK��0*/
	__asm volatile("cpsid i");  /* disable IRQ interrupts, PRIMASK��1 */

//	__enable_irq();	  /* enable IRQ interrupts, PRIMASK��0, CMSIS-Core���� */
//	__disable_irq();	/* disable IRQ interrupts, PRIMASK��1, CMSIS-Core���� */	
	
	x = Sum(10);

	__asm volatile("MRS	x, APSR"); //APSR->x

	LedOnOff();

}
