#include "can_queue.h"
#include <string.h>

void ClearCanQueue(CANQueue * queue)
{
	int i;

	for(i = 0; i < MAX_CAN_SIZE; i++)
	{
		memset(&queue->Elem[i],0,sizeof(CanRxMsg)); 
	}			 
	queue->front = 0; 
	queue->rear = 0;
}

uint8_t IsEmptyCanQueue(CANQueue * queue)
{    
if(queue->front == queue->rear)          
{
	return 1;
}    
else
{
	return 0;    
}    
}

uint8_t IsFullCanQueue(CANQueue * queue)
{   
	if( queue->front == (queue->rear+1) % MAX_CAN_SIZE)
	{
		return 1;
	}    
	else
	{
		return 0;    
	}   
}

uint8_t InsertCanQueue(CANQueue * queue, CanRxMsg element)
{
	if(!IsFullCanQueue(queue))                               
	{
		memcpy(&queue->Elem[queue->rear],&element,sizeof(CanRxMsg));       
		queue->rear = (queue->rear + 1) % MAX_CAN_SIZE;          
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t GetCanQueue(CANQueue * queue,CanRxMsg *element)
{   
	memcpy(element,&queue->Elem[queue->front],sizeof(CanRxMsg));    
	queue->front = (queue->front + 1) % MAX_CAN_SIZE;	
	return 1;
}
