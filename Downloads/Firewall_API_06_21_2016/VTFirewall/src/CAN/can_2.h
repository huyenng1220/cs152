#ifndef __CAN_2_H__
#define __CAN_2_H__

#include "stm32f10x.h"
#include "stm32f10x_can.h"

void CAN2_init(long baud);
int CAN2_tx_data(unsigned short stdid, unsigned extid, char * data);
int CAN2_tx_msg(CanTxMsg *TxMessage);
uint8_t Get_CAN2_Queue(CanRxMsg * rx, int timeoutms);
void Clear_CAN2_Queue(void);
#endif
