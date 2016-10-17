/*----------------------------------------------------------------------------
 *      RL-ARM - TCPnet
 *----------------------------------------------------------------------------
 *      Name:    SERIAL.C
 *      Purpose: Serial Port Driver for STM32F10x
 *      Rev.:    V4.50
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2012 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <stm32f10x.h>                              /* STM32F10x definitions */
#include "serial.h"
#include <stdio.h>
#include "stm32f10x_usart.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
/*----------------------------------------------------------------------------
  Notes:
  The length of the receive and transmit buffers must be a power of 2.
  Each buffer has a next_in and a next_out index.
  If next_in = next_out, the buffer is empty.
  (next_in - next_out) % buffer_size = the number of characters in the buffer.
 *----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
#if TBUF_SIZE < 2
#error TBUF_SIZE is too small.  It must be larger than 1.
#elif ((TBUF_SIZE & (TBUF_SIZE-1)) != 0)
#error TBUF_SIZE must be a power of 2.
#endif

#if RBUF_SIZE < 2
#error RBUF_SIZE is too small.  It must be larger than 1.
#elif ((RBUF_SIZE & (RBUF_SIZE-1)) != 0)
#error RBUF_SIZE must be a power of 2.
#endif

/*----------------------------------------------------------------------------

 *----------------------------------------------------------------------------*/
struct buf_st {
  unsigned int in;                        /* Next In Index                    */
  unsigned int out;                       /* Next Out Index                   */
  char buf [RBUF_SIZE];                   /* Buffer                           */
};

static struct buf_st rbuf = { 0, 0, };
#define SIO_RBUFLEN ((unsigned short)(rbuf.in - rbuf.out))


//! Buffer counter    
//static int rx_i = 0;

/************* STM32F105 Dev Kit  ****************
*   UART 1: TX : PA9   RX  PA10  none remap
*   UART 2: TX : PA2   RX  PA3  none remap
**************************************************/

/*----------------------------------------------------------------------------
  Initialize UART pins, Baudrate
 *----------------------------------------------------------------------------*/
void USART1_Init (void) {
	
	/* USART configuration structure for USART1 */
	USART_InitTypeDef USART1_init_struct;
	/* Bit configuration structure for gpiod PIN5 and PIN6 */
	GPIO_InitTypeDef gpiod_init_struct;	
	 
	/* Enalbe clock for USART1, AFIO and gpiod */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
	
	//GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);												
	/* gpiod PIN5 alternative function Tx */
	gpiod_init_struct.GPIO_Pin = GPIO_Pin_9;
	gpiod_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpiod_init_struct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &gpiod_init_struct);
	/* gpiod PIN6 alternative function Rx */
	gpiod_init_struct.GPIO_Pin = GPIO_Pin_10;
	gpiod_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
	gpiod_init_struct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &gpiod_init_struct);

	/* Enable USART1 */
	USART_Cmd(USART1, ENABLE);  
	/* Baud rate 9600, 8-bit data, One stop bit
	 * No parity, Do both Rx and Tx, No HW flow control
	 */	
	USART_StructInit(&USART1_init_struct);
	/* Configure USART1 */
	USART_Init(USART1, &USART1_init_struct);
	
	/* Enable RXNE interrupt */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* Enable USART1 global interrupt */
	NVIC_EnableIRQ(USART1_IRQn);
	rx_reset();
}
/*----------------------------------------------------------------------------
  USART1_IRQHandler
  Handles USART1 global interrupt request.
 *----------------------------------------------------------------------------*/
void USART1_IRQHandler (void) {  
  struct buf_st *p;
	unsigned char data;
    
		/* RXNE handler */
    if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
    {   
				USART_ClearFlag(USART1,USART_FLAG_RXNE);
        data = (unsigned char )USART_ReceiveData(USART1);
        p = &rbuf;
				if (((p->in - p->out) & ~(RBUF_SIZE-1)) == 0) {
					p->buf [p->in & (RBUF_SIZE-1)] = data;
					p->in++;
				}	
    }
}



void rx_reset(void) {
		//USART1->CR1 &= ~(1<<5);           // Disable RX interrupt 
		rbuf.in = 0;
		rbuf.out = 0;
		//rx_i =0;
}

/*------------------------------------------------------------------------------
  SenChar
  transmit a character
 *------------------------------------------------------------------------------*/

int SendChar (unsigned char value) {	
	USART_SendData(USART1, (uint16_t)value);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC)==RESET); /* Wait transmit completed */
	return (int)value;
}
/*------------------------------------------------------------------------------
  GetKey
  receive a character
 *------------------------------------------------------------------------------*/
int GetKey (void) {
  struct buf_st *p = &rbuf;

  if (SIO_RBUFLEN == 0)
    return (-1);

  return (p->buf [(p->out++) & (RBUF_SIZE - 1)]);
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/

