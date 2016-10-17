#ifndef SERIAL_H_INCLUDED 
#define SERIAL_H_INCLUDED 

#define TBUF_SIZE   2	     /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/
#define RBUF_SIZE   128      /*** Must be a power of 2 (2,4,8,16,32,64,128,256,512,...) ***/

extern void rx_reset(void);
extern void USART1_Init (void);
extern int SendChar (unsigned char value);
extern int GetKey (void);
#endif
