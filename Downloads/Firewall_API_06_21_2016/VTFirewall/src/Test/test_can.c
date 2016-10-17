#include "test_can.h"

char tx_buf[8]= { 0xAB,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B};

void can12_recv_test(void)
{
	int cnt;
	CanRxMsg rx;

	CAN1_init(500000);
	CAN2_init(500000);

  CAN1_tx_data(0xAA, 0, tx_buf);
  CAN2_tx_data(0xBB, 0, tx_buf);	
	while(1)
	{
		cnt = Get_CAN1_Queue(&rx, 10);
		if (cnt > 0)
		{
			CAN2_tx_msg((CanTxMsg*)&rx);
		}

		cnt = Get_CAN2_Queue(&rx, 10);
		if (cnt > 0)
		{			
			CAN1_tx_msg((CanTxMsg*)&rx);
		}		
	}
}
