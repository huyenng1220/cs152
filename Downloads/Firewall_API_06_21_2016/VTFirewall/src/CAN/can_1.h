#ifndef __CAN_1_H__
#define __CAN_1_H__

#include "stm32f10x.h"
#include "stm32f10x_can.h"

void CAN1_init(long baud);
int CAN1_tx_data(unsigned short stdid, unsigned extid, char * data);
int CAN1_tx_msg(CanTxMsg *TxMessage);
uint8_t Get_CAN1_Queue(CanRxMsg * rx, int timeoutms);
void Clear_CAN1_Queue(void);
#endif
