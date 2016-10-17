/**
  ******************************************************************************
  * @file    Project/OBD/obd_private.c
  * @author
  * @version
  * @date    08-June-2016
  * @brief   Library OBD file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */
	
/* Includes ------------------------------------------------------------------*/
#include "fw_user_if.h"
#include <string.h>


#define MAX_ITEM_BLACK_LIST 3
#define MAX_ITEM_WHITE_LIST 3


char white_buf[MAX_WHITE_LEN];
static int white_used_len = 0;


char black_buf[MAX_BLACK_LEN];
static int black_used_len = 0;


static FW_Read_t Read = NULL;
static FW_Write_t Write = NULL;
static CAN_exec_t CanExec = NULL;

uint16_t IdDefault[MAX_ITEM_BLACK_LIST+MAX_ITEM_WHITE_LIST] = { 0x01, 0x03, 0x04, 0x08, 0x11, 0x77};

/** @defgroup Firewall_Private_Functions
  * @{
  */

static void FW_RegisterReadFileCB (FW_Read_t read)
{
	Read = read;
}

static void FW_RegisterWriteFileCB (FW_Write_t write)
{
	Write = write;
}

static void FW_RegisterCanExecCB (CAN_exec_t can_exec)
{
	CanExec = can_exec;
}

static unsigned short _htons(unsigned short s)
{
	return (s / 256 + (s%256)*256);
}


/**
  * @brief      Execute Black Block Pass
  * @param[in]  *at: pointer to a ATST structure
	* @param[in]  state: current state	
  * @retval     >=0: execute success.
	*             -1: not success
  */		
static int32_t FW_IsPass(uint8_t types,ATST * at, char state)
{
	int pos = sizeof(struct WhitePassHead);	
	int used_len = 0;
	struct WhitePassHead *whiteHead = NULL;
	struct WhitePass * whitePass = NULL;
	unsigned char procHead_flag = FALSE;
	
	if(types == BLACK_LIST)
	{
		whiteHead = (struct WhitePassHead *)black_buf;
	  used_len = black_used_len;
		whitePass = (struct WhitePass*)(black_buf + sizeof(struct WhitePassHead));
	}
	else
	{
		whiteHead = (struct WhitePassHead *)white_buf;
	  used_len = white_used_len;
		whitePass = (struct WhitePass*)(white_buf + sizeof(struct WhitePassHead));
	}	
	if (used_len <= 0)
		return FW_ERROR;
	if (whiteHead->busType != at->bustype)
		return FW_ERROR;
	for (; pos < used_len - sizeof(struct WhitePassTail);)
	{		
		whitePass = (struct WhitePass*)(whitePass + whitePass->whiteLen);
		pos += whitePass->whiteLen;


		/* Check CAN ID */
		if (whitePass->canId != _htons(at->stdid))
			continue;		
		
		switch(whitePass->procHead1)
		{
			case 0:
				//still
				procHead_flag = TRUE;
			break;
			case 1:
				if (state == (AT_STATE_STILL<<1))
					procHead_flag = TRUE;					
				else
					procHead_flag = FALSE;
			break;
			case 2:
				if (state == (AT_STATE_POWERON<<1))
					procHead_flag = TRUE;					
				else
					procHead_flag = FALSE;
			break;
			case 3:
				if (state == (AT_STATE_RUNNING<<1))
					procHead_flag = TRUE;					
				else
					procHead_flag = FALSE;
			break;
			default:
				procHead_flag = FALSE;
			break;
		}
		
		if( procHead_flag == FALSE)
			continue;
		
		if (memcmp(at->data + whitePass->offset, &whitePass->data[0] + whitePass->offset, whitePass->compareLen) == 0)			
			//blocked cmd
		  return ATCAN_TBOX_PREVENT;
		else
			continue;
		
	}

	return FW_ERROR;
}



/**
  * @brief      Execute ATCAN command
  * @param[in]  *filename: pointer to a name of file
	* @param[in]  fsize: size of file
	* @param[in]  * cmd: pointer to a ATCAN structure.
  * @param[in]  CAN_exec: is callback function.
  * @retval     0: execute success.
	*             -1: not success
  */		
static int32_t FW_atcan_exec_command(int32_t  buf_size, ATCAN *cmd,char state)
{
	ATST at;
	at.stdid = cmd->stdid;
	at.filter = cmd->filterid;
	at.bustype = BUS_CAN;
	memcpy(at.data, cmd->data,8);	
	if(at.bustype == BUS_CAN)
	{
		if (black_used_len > 0)
		{
			if (FW_IsPass(BLACK_LIST,&at, state) != ATCAN_TBOX_PREVENT)
			{
				cmd->atExeced = 1;
				if(CanExec != NULL)
					CanExec(at.stdid, at.filter, (char *)at.data, (char *)at.data);
			}
		}
		else 
		{
			cmd->atExeced = 1;
			if(CanExec != NULL)
				CanExec(at.stdid, at.filter, (char *)at.data, (char *)at.data);
		}
		return FW_SUCCESS;
	}
	return FW_ERROR;		
}

static int32_t WriteDefaultList(char *whichfile,int32_t maxLen, int32_t maxItem, uint16_t *buff, uint16_t procHead)
{
	unsigned short size =0;
	int i = 0;	
	uint16_t * pBuf = buff;
	struct WhitePassHead head= {0};
	struct WhitePass whitelist ={0};
	struct WhitePassTail tail = {0};
	
	/* Calculate Length of whitelist */
	size = sizeof(struct WhitePassHead) + (sizeof(struct WhitePass) * maxItem) + sizeof(struct WhitePassTail);
	if(size > maxLen)
	{
		//printf("Black size bigger than max_black_size");
	  return -1;
	}
	
	/* Write White pass head to flash */
	head.procHead = procHead;
	head.commandType = 0;
	head.commandLen = maxItem;
	head.busType = BUS_CAN;	
	/*Write data */
	Write(whichfile, 0, (void *)&head, (int)sizeof(struct WhitePassHead));
	
	/* Write all WhitePass data to flash */
	for(i=0;i < maxItem; i++)
	{
		whitelist.whiteLen = sizeof(struct WhitePass);
		whitelist.procHead1 = 0; /* AT state */
		whitelist.canType = 0x00;
		whitelist.canId = _htons(*pBuf);
		whitelist.filterId = _htons(0x0000);
		whitelist.hasOffset = 1;
		whitelist.offset = 0;
		whitelist.compareLen = 1;
		whitelist.data[0] = 0;
		/*Write data */
		Write(whichfile, ((sizeof(struct WhitePass) * i) + sizeof(struct WhitePassHead)), (void *)&whitelist, (int)sizeof(struct WhitePass));
	}	
	
	/* Write WhitePassTail */
	tail.chedk = 0;
	tail.procEnd = procHead;
	/*Write data */
	Write(whichfile, ((sizeof(struct WhitePass) * maxItem) + sizeof(struct WhitePassHead)), (void *)&tail, (int)sizeof(struct WhitePassTail));	
	
	/* Write file size */
	Write(whichfile,maxLen,&size,2);
	
	return size;	
}

static int32_t GetFileSize(char *whichfile,int32_t maxLen)
{
	int size;
	unsigned short data;
	
	/* Calculate Length of blacklist */
	size = Read(whichfile,maxLen,&data,2);
	if(size == -1)	
	{
		/* Write data default for WhiteList and Black List to flash */ 
		if (strcmp(BLACK_FILE, whichfile) == 0)
		{ 
			size = WriteDefaultList(whichfile,maxLen, MAX_ITEM_BLACK_LIST, &IdDefault[0], 0xAAAA);	
		}
		else
		{
			size = WriteDefaultList(whichfile,maxLen, MAX_ITEM_WHITE_LIST, &IdDefault[MAX_ITEM_BLACK_LIST], 0xBBBB);
		}
	}		
	else
		size = data;
	return size;	
}

/**
  * @brief  Enables While List or Black List
  * @param  None
  * @retval None
  */	
static void FW_Enable(uint8_t types)
{
	int fsize =0;
	
	if(Read != NULL) 
	{	 		
		switch (types)
		{
			case BLACK_LIST:			
				/* Calculate Length of whitelist */
			  fsize = GetFileSize(BLACK_FILE,MAX_BLACK_LEN);				
				/* Read black list data */
				black_used_len = Read(BLACK_FILE, 0, black_buf, fsize);	      
				break;
			case WHITE_LIST:
				fsize = GetFileSize(WHITE_FILE, MAX_WHITE_LEN);
				/* Read white list data */
				white_used_len = Read(WHITE_FILE, 0, white_buf, fsize);	
				break;
			default:
				fsize = GetFileSize(BLACK_FILE,MAX_BLACK_LEN);
				black_used_len = Read(BLACK_FILE, 0, black_buf, fsize);
				fsize = GetFileSize(WHITE_FILE, MAX_WHITE_LEN);
				white_used_len = Read(WHITE_FILE, 0, white_buf, fsize);	
				break;
		}			
	}	
}

/**
  * @brief  Disables WhileList or Black List
  * @param[in]  types select type is BLACK_LIST, WHITE_LIST or both
  * @retval None
  */	
static void FW_Disable(uint8_t types)
{
	switch (types)
	{
		case BLACK_LIST:			
			/* Read black list data */
			black_used_len = 0;			
			break;
		case WHITE_LIST:
			/* Read white list data */
			white_used_len = 0;				
			break;		
		default:
			black_used_len = 0;
			white_used_len = 0;	
			break;
	}
}

/**
  * @brief  Get length of White list or Black list
  * @param  None
  * @retval size of White list or Black list if success
  *         FW_ERROR: not success
  */	
static int32_t FW_GetLen(uint8_t types)
{
	if(types == BLACK_LIST)
		return black_used_len;
	if(types == WHITE_LIST)
		return white_used_len;
	
	return FW_ERROR;
}

/* Create Firewall function */
VT_FW_API VT_FW_Framework = {	
  FW_Enable,   
  FW_Disable,	
	FW_GetLen,		
	FW_IsPass,
	FW_atcan_exec_command, 
	FW_RegisterReadFileCB,
	FW_RegisterWriteFileCB,
	FW_RegisterCanExecCB	
};

/**
  * @}
  */

/******************* END OF FILE************************************/
