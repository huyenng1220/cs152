#include "test_firewall_functions.h"


void Delay(__IO uint32_t nCount);



int main ( void )
{
	/* UART1 Init for console */
	USART1_Init ();
	printf("\r\n");
	/* Init CAN 1 */	
  CAN1_init(500000);
  printf(" Init CAN 1 for received CAN Id \r\n");
	/* Init CAN 2 */	
  CAN2_init(500000);  	
  printf(" Init CAN 2 for send CAN Id output from firewall\r\n");	
  /* Init Emulator Eeprom */
	EEL_Init();
	printf(" Init EEProm emulator to access to data in internal flash\r\n");
	printf("\r\n\r\n");	
	
	while(1)
	{		
		menu();		
		Delay(5000);
	}
}

void Delay(__IO uint32_t nCount)
{
    uint8_t x;
    for(; nCount != 0; nCount--)
	    for(x=0;x<100;x++);
}

