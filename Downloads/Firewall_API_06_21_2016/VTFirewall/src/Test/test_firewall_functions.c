#include "test_firewall_functions.h"

#define  ptrFW          (&VT_FW_Framework)
#define Esc 27
#ifndef NULL
#define NULL 0
#endif

/****************Structure storage data for WHITE LIST AND BLACK LIST*************************** 
											|--------------------------------------|
											|       WhitePassHead                  |
											|--------------------------------------|
											|       WhitePass  1                   |
											|--------------------------------------|
											|       WhitePass  2                   |
											|--------------------------------------|
											|       ..........                     |
											|--------------------------------------|
											|       WhitePass  n-1                 |
											|--------------------------------------|
											|       WhitePass  n                   |
											|--------------------------------------|
											|       WhitePassTail                  |
											|--------------------------------------|
************************************************************************************************/

static unsigned char state = 0, number = 0, index = 0, type = 0;	
struct WhitePassHead head_1;

void CAN_exec(unsigned short stdid, unsigned short filter, char *data, char * retData)
{	
		CAN2_tx_data(stdid, 0, data);		
}


void print_menu(void)
{
		//printf("\r\n This is menu test for Firewall\r\n");
		//printf(" ------------------------------\r\n");
		printf(" 1 - Run test\r\n");
	  printf(" 2 - Updated Policy\r\n");
		//printf(" ------------------------------\r\n");
		printf(" Please select from 1 to 2\r\n > ");	 
}

void print_menu_update(void)
{
	//printf(" \r\nRunning Update Policy\r\n");
	//printf(" Press Escape to exit\r\n");
	//printf(" -------------------------\r\n");	
	printf(" 1 - Write new Black List\r\n");
	printf(" 2 - Write new White List\r\n");
	//printf(" -------------------------\r\n");
	printf(" Please select from 1 to 2\r\n: >");	
}

static unsigned short _htons(unsigned short s)
{
	return (s / 256 + (s%256)*256);
}

//void SaveWhitePass(unsigned short tmp)
//{
//	struct WhitePass whitepass;
//	
//	whitepass.whiteLen = sizeof(struct WhitePass);
//	whitepass.procHead1 = 0;
//	whitepass.canType = 0x00;
//	whitepass.canId = _htons(tmp);
//	whitepass.filterId = _htons(0x0000);
//	whitepass.hasOffset = 1;
//	whitepass.offset = 0;
//	whitepass.compareLen = 1;
//	whitepass.data[0] = 0;
//	
//	/*Write data */
//	if(type == 0)
//		Write(BLACK_FILE, ((sizeof(struct WhitePass) * index) + sizeof(struct WhitePassHead)), (void *)&whitepass, (int)sizeof(struct WhitePass));
//	else
//		Write(WHITE_FILE, ((sizeof(struct WhitePass) * index) + sizeof(struct WhitePassHead)), (void *)&whitepass, (int)sizeof(struct WhitePass));
//}

//void SaveTail(void)
//{	
//	struct WhitePassTail tail;
//	
//	/* Write WhitePassTail */
//	tail.chedk = 0;
//	if(type == 0)
//		tail.procEnd = 0xBBBB;
//	else
//		tail.procEnd = 0xAAAA;
//	/*Write data */
//	if(type == 0)
//		Write(BLACK_FILE, ((sizeof(struct WhitePass) * number) + sizeof(struct WhitePassHead)), (void *)&tail, (int)sizeof(struct WhitePassTail));
//	else
//		Write(WHITE_FILE, ((sizeof(struct WhitePass) * number) + sizeof(struct WhitePassHead)), (void *)&tail, (int)sizeof(struct WhitePassTail));
//}

//int SaveHead(void)
//{
//	unsigned short size, t_size;
//	struct WhitePassHead head;
//	/* Calculate Length of blacklist */
//	size = sizeof(struct WhitePassHead) + (sizeof(struct WhitePass) * number) + sizeof(struct WhitePassTail);
//	if(type == 0)
//		t_size = MAX_BLACK_LEN;
//	else
//		t_size = MAX_WHITE_LEN;;
//	if(size > t_size)
//	{
//		//printf("Black size bigger than max_black_size");
//	  return -1;
//	}
//	
//	/* Write White pass head to flash */
//	if(type == 0)
//		head.procHead = 0xBBBB;
//	else
//		head.procHead = 0xAAAA;
//	head.commandType = 0;
//	head.commandLen = number;
//	head.busType = BUS_CAN;	
//	/*Write data */
//	if(type == 0)
//	{
//		Write(BLACK_FILE, 0, (void *)&head, (int)sizeof(struct WhitePassHead));
//		Write(BLACK_FILE,MAX_BLACK_LEN,&size,2);
//	}
//	else
//	{
//		Write(WHITE_FILE, 0, (void *)&head, (int)sizeof(struct WhitePassHead));
//		Write(WHITE_FILE,MAX_WHITE_LEN,&size,2);
//	}
//	return 0;
//}

//void Change_State(unsigned short tmp)
//{
//	if(state == 0)
//	{
//		if((tmp >0) && (tmp <= 100))
//		{
//			number = tmp;			
//			if( SaveHead() != -1)
//			{
//				state++;
//				index = 0;
//				printf("\r\n Enter Data for element 1\r\n");				
//				printf(" > ");
//			}
//			else
//			{				
//				if(type == 0)
//					printf("\r\n Enter number of element for Black List\r\n");
//				else
//					printf("\r\n Enter number of element for White List\r\n");
//				printf(" > ");
//			}
//		}
//		else
//		{
//			if(type == 0)
//				printf("\r\n Enter number of element for Black List\r\n");
//			else
//				printf("\r\n Enter number of element for White List\r\n");
//			printf(" > ");
//		}
//	}else	if(state == 1)
//	{			
//		SaveWhitePass(tmp);		
//		index++;		
//		if(index >= number)
//		{
//			SaveTail();
//			state++;	
//      return;			
//		}
//		printf("\r\n Enter Data for element %d\r\n",index+1);				
//		printf(" > ");
//	}
//}


//void Create_List(void)
//{
//	int c;	
//	unsigned short tmp;
//	if(type == 0)
//		printf("\r\n Enter number of element for Black List\r\n");
//	else
//		printf("\r\n Enter number of element for White List\r\n");
//	printf(" > ");
//	state = 0;	
//	number = 0;
//	tmp = 0;
//	index = 0;
//	
//	while(1)
//	{
//		c = GetKey();
//		if(c == Esc)					
//			break;    		
//    if(	c == 0x0D)
//		{
//			Change_State(tmp);
//			tmp = 0;
//			if(state >= 2)				
//			{
//				state = 0;
//				break;
//			}
//		}			
//    if((c >= 0x30) && (c <= 0x39))
//		{
//			SendChar((unsigned char)c);
//			tmp = (tmp*10)+ (c-0x30);
//		}		
//	}
//	print_menu_update();
//}

void WriteList(unsigned char *buff, int size)
{
	
	/*Write data */
	if(type == 0)
	{
		Write(BLACK_FILE, 0, (void *)buff, (int)size);
		Write(BLACK_FILE,MAX_BLACK_LEN,&size,2);
	}
	else
	{
		Write(WHITE_FILE, 0, (void *)buff, size);
		Write(WHITE_FILE,MAX_WHITE_LEN,&size,2);
	}
	
}

void ReceiveData(void){
	int c,size = 0, f=0;
  unsigned char tmp = 0;
	unsigned char bufRev[700];
	
	while(1)
	{
		c = GetKey();
		if(	c == 0x0D)					
			break;    		
    
    if((c >= 0x30) && (c <= 0x39))
		{
			
			SendChar((unsigned char)c);
			tmp = (tmp*16)+ (c-0x30);
			f++;
			if(f == 2)
			{
				bufRev[size++] = tmp;
				tmp = 0;
				f = 0;
			}
		}			
	}
	if(size > 0)
		WriteList(bufRev, size);
}

void updatePolicy(void)
{
	int c;
	
	
	print_menu_update();
	while(1)
	{
		c = GetKey();
		if(c == Esc)					
			break;		
    switch(c)
		{
			case WRITE_BLACK_LIST:				
				type = 0;			
				ReceiveData();			 
				break;
			case WRITE_WHITE_LIST:
				type = 1;
				ReceiveData();
				break;
			default:				
				break;
		}			
	}
	print_menu();
}

void run_test(void)
{
	int c,i;
	int cnt;
	CanRxMsg rx;
	ATCAN cmd;
	
//	printf("\r\n Running test Firewall\r\n\r\n");
//	printf(" Please send a message at CAN1 and then receive message from CAN2\r\n");
//	printf(" Note:Don't have any ouput at CAN2 if CAN Id is blocked\r\n");
//	printf(" Press Escape to exit\r\n");
//	printf("\r\n");
	
	/* You must register Read File Call back function before calling EnableBlack or EnableWhite function */
	/* Register read file and write file to read and write Black list and White list data from flash */
	ptrFW->RegisterReadFileCB(&Read);
	ptrFW->RegisterWriteFileCB(&Write);
	/* Register Can execute function to send ouput via CAN port */
	ptrFW->RegisterCanExecCB(&CAN_exec);	
	/* Enable Black List and White list */
	ptrFW->Enable(BLACK_AND_WHITE);
//  printf(" Enable Black List and White List\r\n");	
//	printf(" Please send a message at CAN1\r\n");
	while(1)
	{
		c = GetKey();
    if(c == Esc)
		{
			ptrFW->Disable(BLACK_AND_WHITE);			
			//printf(" Disable Black list and White List\r\n");
			print_menu();
			break;
		}
		cnt = Get_CAN1_Queue(&rx, 10);
		if (cnt > 0)
		{
			cmd.filterid = 0;
			cmd.stdid = rx.StdId;
			memcpy(cmd.data, rx.Data,8);
			ptrFW->ExecATCANCommand(8, &cmd,0);
			printf("\r\n Call ExecATCANCommand function with data:\r\n");
			printf(" Can Id: 0x%x\r\n",cmd.stdid);
			printf(" Data: ");
			for(i = 0; i < 8;i++)
			{
				printf("0x%x ,",cmd.data[i]);
			}
			printf("\r\n");
		}
	}
}


void menu(void)
{
	int c;
	print_menu();
	while(1)
	{
		c = GetKey();
    switch(c)
		{
			case 0x31:
				run_test();
				break;
			case 0x32:
				updatePolicy();
				break;			
			case Esc:
				print_menu();
			  break;
			default:				
				break;
		}			
	}
}

