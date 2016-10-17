#ifndef _CAN_QUEUE_H_
#define _CAN_QUEUE_H_

#define MAX_CAN_SIZE 50


#include "stm32f10x.h"
#include "stm32f10x_can.h"

typedef struct _CANQueue
{
	CanRxMsg Elem[MAX_CAN_SIZE];
	uint16_t front;
	uint16_t rear;
}CANQueue;

void ClearCanQueue(CANQueue * queue);
uint8_t IsEmptyCanQueue(CANQueue * queue);
uint8_t IsFullCanQueue(CANQueue * queue);
uint8_t InsertCanQueue(CANQueue* queue, CanRxMsg element);
uint8_t GetCanQueue(CANQueue * queue, CanRxMsg *element);

#endif
