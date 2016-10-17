
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
#include "gen_data_test.h"

int black_file_size =0;
int white_file_size =0;

unsigned short BlackID[] = {1,3,11,77,66,99,911,444,666,22};
unsigned short WhiteID[] = {101,108,200,300,555,500,111,999,888,333};

static unsigned short _htons(unsigned short s)
{
	return (s / 256 + (s%256)*256);
}

int Gen_BLACK_LIST_Data_To_Test(void)
{
	unsigned short size =0;
	int i = 0;
	unsigned char at_state = 0;
	struct WhitePassHead head= {0};
	struct WhitePass blacklist ={0};
	struct WhitePassTail tail = {0};
	
	/* Calculate Length of blacklist */
	size = sizeof(struct WhitePassHead) + (sizeof(struct WhitePass) * MAX_ITEM_BLACK_LIST) + sizeof(struct WhitePassTail);
	if(size > MAX_BLACK_LEN)
	{
		//printf("Black size bigger than max_black_size");
	  return -1;
	}
	
	/* Write White pass head to flash */
	head.procHead = 0xBBBB;
	head.commandType = 0;
	head.commandLen = MAX_ITEM_BLACK_LIST;
	head.busType = BUS_CAN;	
	/*Write data */
	Write(BLACK_FILE, 0, (void *)&head, (int)sizeof(struct WhitePassHead));
	
	/* Write all WhitePass data to flash */
	for(i=0;i < MAX_ITEM_BLACK_LIST; i++)
	{
		blacklist.whiteLen = sizeof(struct WhitePass);
		blacklist.procHead1 = (at_state <<1);
//		at_state++;
//		if(at_state > AT_STATE_RUNNING)
//			at_state = 0;
		blacklist.canType = 0x00;
		blacklist.canId = _htons(BlackID[i]);
		blacklist.filterId = _htons(0x0000);
		blacklist.hasOffset = 1;
		blacklist.offset = 0;
		blacklist.compareLen = 1;
		blacklist.proc[0] = 0;
		/*Write data */
		Write(BLACK_FILE, ((sizeof(struct WhitePass) * i) + sizeof(struct WhitePassHead)), (void *)&blacklist, (int)sizeof(struct WhitePass));
	}
	
	/* Write WhitePassTail */
	tail.chedk = 0;
	tail.procEnd = 0xBBBB;
	/*Write data */
	Write(BLACK_FILE, ((sizeof(struct WhitePass) * MAX_ITEM_BLACK_LIST) + sizeof(struct WhitePassHead)), (void *)&tail, (int)sizeof(struct WhitePassTail));
	
	black_file_size = size;
	Write(BLACK_FILE,MAX_BLACK_LEN,&size,2);
	return 0;
}

int Gen_WHITE_LIST_Data_To_Test(void)
{
	unsigned short size =0;
	int i = 0;
	unsigned char at_state = 0;
	struct WhitePassHead head= {0};
	struct WhitePass whitelist ={0};
	struct WhitePassTail tail = {0};
	
	/* Calculate Length of whitelist */
	size = sizeof(struct WhitePassHead) + (sizeof(struct WhitePass) * MAX_ITEM_WHITE_LIST) + sizeof(struct WhitePassTail);
	if(size > MAX_WHITE_LEN)
	{
		//printf("Black size bigger than max_black_size");
	  return -1;
	}
	
	/* Write White pass head to flash */
	head.procHead = 0xAAAA;
	head.commandType = 0;
	head.commandLen = MAX_ITEM_WHITE_LIST;
	head.busType = BUS_CAN;	
	/*Write data */
	Write(WHITE_FILE, 0, (void *)&head, (int)sizeof(struct WhitePassHead));
	
	/* Write all WhitePass data to flash */
	for(i=0;i < MAX_ITEM_WHITE_LIST; i++)
	{
		whitelist.whiteLen = sizeof(struct WhitePass);
		whitelist.procHead1 = (at_state <<1);
//		at_state++;
//		if(at_state > AT_STATE_RUNNING)
//			at_state = 0;
		whitelist.canType = 0x00;
		whitelist.canId = _htons(WhiteID[i]);
		whitelist.filterId = _htons(0x0000);
		whitelist.hasOffset = 1;
		whitelist.offset = 0;
		whitelist.compareLen = 1;
		whitelist.proc[0] = 0;
		/*Write data */
		Write(WHITE_FILE, ((sizeof(struct WhitePass) * i) + sizeof(struct WhitePassHead)), (void *)&whitelist, (int)sizeof(struct WhitePass));
	}
	
	/* Write WhitePassTail */
	tail.chedk = 0;
	tail.procEnd = 0xAAAA;
	/*Write data */
	Write(WHITE_FILE, ((sizeof(struct WhitePass) * MAX_ITEM_WHITE_LIST) + sizeof(struct WhitePassHead)), (void *)&tail, (int)sizeof(struct WhitePassTail));
	
	white_file_size = size;
	Write(WHITE_FILE,MAX_WHITE_LEN,&size,2);
	return 0;
	
}

void Get_File_Size(void)
{
	int size;
	unsigned short data;
	/* Calculate Length of blacklist */
	size = Read(BLACK_FILE,MAX_BLACK_LEN,&data,2);
	if(size == -1)
	{
		Gen_BLACK_LIST_Data_To_Test();
		black_file_size = sizeof(struct WhitePassHead) + (sizeof(struct WhitePass) * MAX_ITEM_BLACK_LIST) + sizeof(struct WhitePassTail);
	}
	else
	{
		black_file_size = data;
	}
	/* Calculate Length of whitelist */
	size = Read(WHITE_FILE,MAX_WHITE_LEN,&data,2);
	if(size == -1)
	{
		Gen_WHITE_LIST_Data_To_Test();
		white_file_size = sizeof(struct WhitePassHead) + (sizeof(struct WhitePass) * MAX_ITEM_WHITE_LIST) + sizeof(struct WhitePassTail);
	}
	else
	{
		white_file_size = data;
	}
	
}

