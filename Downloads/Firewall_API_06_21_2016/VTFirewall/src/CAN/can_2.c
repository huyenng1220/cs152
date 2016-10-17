#include "can_queue.h"
#include "can_2.h"
#include "misc.h"
#include <string.h>

#define GPIO_CAN                   GPIOB
#define RCC_APB2Periph_GPIO_CAN    RCC_APB2Periph_GPIOB
#define GPIO_Pin_RX                GPIO_Pin_12
#define GPIO_Pin_TX                GPIO_Pin_13
#define GPIO_Remap_CAN             GPIO_Remap1_CAN2

#define MAX_MAIL_NUM  3

static u8 CAN_msg_num[MAX_MAIL_NUM];
static struct _CANQueue canQueue;

#ifdef UCOSII
OS_EVENT *sem_CAN2 = NULL;
#endif

void CAN2_config_init(long baud)
{

	CAN_InitTypeDef        CAN_InitStructure;
	CAN_FilterInitTypeDef  CAN_FilterInitStructure;
	/* CAN register init */
	CAN_DeInit(CAN2);
	CAN_StructInit(&CAN_InitStructure);
	/* CAN cell init */  // 36M 250k??
	CAN_InitStructure.CAN_TTCM=DISABLE;
	CAN_InitStructure.CAN_ABOM=DISABLE;
	CAN_InitStructure.CAN_AWUM=DISABLE;
	CAN_InitStructure.CAN_NART=DISABLE;
	CAN_InitStructure.CAN_RFLM=DISABLE;
	CAN_InitStructure.CAN_TXFP=DISABLE;
	CAN_InitStructure.CAN_Mode=CAN_Mode_Normal;//CAN_Mode_LoopBack;//
  /* 36MHz/CAN_Prescaler/(CAN_SJW+CAN_BS1+CAN_BS2) */
  switch(baud)
	{
		case 5000:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;   
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;    
			CAN_InitStructure.CAN_Prescaler = 2 * 200;    
			break;
		case 50000:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;  
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;   
			CAN_InitStructure.CAN_Prescaler = 2 * 20;  
			break;
		case 125000:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;  
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;  
			CAN_InitStructure.CAN_Prescaler = 2 * 8; 
			break;
		case 250000:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;  
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;   
			CAN_InitStructure.CAN_Prescaler = 2 * 4;  
			break;
		case 500000:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;  
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;   
			CAN_InitStructure.CAN_Prescaler = 2 * 2;  
			break;
		case 1000000:
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;   
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;  
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;   
			CAN_InitStructure.CAN_Prescaler = 2 * 1;   
			break;	
		default:  /* default 500000 */
			CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;    
			CAN_InitStructure.CAN_BS1 = CAN_BS1_10tq;  
			CAN_InitStructure.CAN_BS2 = CAN_BS2_7tq;   
			CAN_InitStructure.CAN_Prescaler = 2 * 2;  
			break;
	}
  CAN_Init(CAN2, &CAN_InitStructure);
  /* Config   filter */
  CAN_FilterInitStructure.CAN_FilterNumber=14;
  CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
  CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
  CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;
  CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
  CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_FIFO1;
  CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;
  CAN_FilterInit(&CAN_FilterInitStructure); 
}




void CAN2_init(long baud)
{
  
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;    
	 
	/* GPIO for CAN clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO |RCC_APB2Periph_GPIO_CAN, ENABLE);
	/* CAN2 Periph clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN2, ENABLE);	

	/* Enable CAN2 RX0 interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_RX1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enable CAN2 TX0 interrupt IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = CAN2_TX_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Configure CAN pin: RX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_RX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
	/* Configure CAN pin: TX */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_TX;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIO_CAN, &GPIO_InitStructure);
	/* Config pin remap for CAN */
	//GPIO_PinRemapConfig(GPIO_Remap_CAN , ENABLE);
	
	CAN2_config_init(baud); 
	//CAN_ITConfig(CAN2, CAN_IT_FMP0 | CAN_IT_FF0 | CAN_IT_FOV0, ENABLE);  // fifo0
	CAN_ITConfig(CAN2, CAN_IT_FMP1 | CAN_IT_FF1 | CAN_IT_FOV1, ENABLE);  // fifo1
	CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);                // ????
	CAN_ITConfig(CAN2, CAN_IT_EWG | CAN_IT_EPV | CAN_IT_BOF | CAN_IT_LEC 
									 | CAN_IT_ERR | CAN_IT_WKU | CAN_IT_SLK, ENABLE);  // ERR	
#ifdef UCOSIIs
	sem_CAN2 = OSSemCreate(0);
#endif	
  CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP0);
	CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP1);
	CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP2);
	memset(CAN_msg_num,0,MAX_MAIL_NUM);
	ClearCanQueue(&canQueue);   
}

int CAN2_tx_msg(CanTxMsg *TxMessage)
{
    uint8_t TransmitMailbox = 0;
   
    TransmitMailbox = CAN_Transmit(CAN2, TxMessage);
    if(TransmitMailbox == CAN_NO_MB)
    {
        //printf("tx can fail\r\n");
        return -1;
    }
    else
    {            
        CAN_msg_num[TransmitMailbox] = 1;        
    }    
    CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);

    return 0;
}

int CAN2_tx_data(unsigned short stdid, unsigned extid, char * data)
{

	CanTxMsg TxMessage;
	uint8_t i = 0;

	/* transmit */
	TxMessage.StdId=stdid;//0x7DF;
	TxMessage.RTR=CAN_RTR_DATA;
	TxMessage.IDE=CAN_ID_STD;
	TxMessage.ExtId = extid;//0;
	TxMessage.DLC=8;

	for (i = 0; i < 8; i++)
	{	
		TxMessage.Data[i] = data[i];
	}
	i = CAN_Transmit(CAN2, &TxMessage);
	if(CAN_NO_MB == i)
	{
			//printf("tx can fail\r\n");
			return -1;
	}
	else
	{         
			CAN_msg_num[i] = 1;      
	}    
	CAN_ITConfig(CAN2, CAN_IT_TME, ENABLE);

	return 0;
}

void CAN2_TX_IRQHandler(void)
{
    if(CAN_msg_num[0])
    {
        if(SET == CAN_GetITStatus(CAN2,CAN_IT_RQCP0))
        {
            CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP0);
            CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);
            CAN_msg_num[0] = 0;
        }
    }
   
    if(CAN_msg_num[1])
    {
        if(SET == CAN_GetITStatus(CAN2,CAN_IT_RQCP1))
        {
            CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP1);
            CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);
            CAN_msg_num[1] = 0;
        }
    }
   
    if(CAN_msg_num[2])
    {
        if(SET == CAN_GetITStatus(CAN2,CAN_IT_RQCP2))
        {
            CAN_ClearITPendingBit(CAN2,CAN_IT_RQCP2);
            CAN_ITConfig(CAN2, CAN_IT_TME, DISABLE);
            CAN_msg_num[2] = 0;
        }
    }    
}

/**
  * @brief  This function handles USB Low Priority or CAN RX0 interrupts 
  *   requests.
  * @param  None
  * @retval : None
  */
void CAN2_RX0_IRQHandler(void)
{
	CanRxMsg RxMessage;

	if(SET == CAN_GetITStatus(CAN2,CAN_IT_FF0))
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_FF0);
	}
	else if(SET == CAN_GetITStatus(CAN2,CAN_IT_FOV0))
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_FOV0);
	}
	else
	{
		CAN_Receive(CAN2, CAN_FIFO0, &RxMessage);
		InsertCanQueue(&canQueue, RxMessage);  
#ifdef UCOSII
		if (sem_CAN2 != NULL)
			OSSemPost(sem_CAN2);
#endif		 
	}  
}
void CAN2_RX1_IRQHandler(void)
{
   
	CanRxMsg RxMessage;
	if(SET == CAN_GetITStatus(CAN2,CAN_IT_FF1))
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_FF1);
	}
	else if(SET == CAN_GetITStatus(CAN2,CAN_IT_FOV1))
	{
		CAN_ClearITPendingBit(CAN2,CAN_IT_FOV1);
	}
	else
	{
		CAN_Receive(CAN2, CAN_FIFO1, &RxMessage);
		InsertCanQueue(&canQueue, RxMessage);
#ifdef UCOSII
		if (sem_CAN2 != NULL)
			OSSemPost(sem_CAN2);
#endif
	}   
}

static void delay_ms(int time)
{    
   int i=0;  
   while(time--)
   {
      i=1200;  
      while(i--) ;    
   }
}

uint8_t Get_CAN2_Queue(CanRxMsg * rx, int timeoutms)
{
#ifdef UCOSII
	INT8U err = 0;	
	OSSemPend(sem_CAN2,0,&err);
#endif
	do
	{
		if (!IsEmptyCanQueue(&canQueue))
		{
			return (GetCanQueue(&canQueue,rx));
		}
		
#ifdef UCOSII
		OSTimeDly(1);
#else
		delay_ms(5);
#endif
		timeoutms -= 5;		
	}while(timeoutms > 5);
	return 0;
}


void Clear_CAN2_Queue(void)
{
	ClearCanQueue(&canQueue);
}

