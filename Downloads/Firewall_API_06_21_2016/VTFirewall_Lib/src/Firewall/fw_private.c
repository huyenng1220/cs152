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


#define FW_API_VERSION 1
#define FW_API_NAME "FIREWALL FRAMEWORK"


static char white_buf[MAX_WHITE_LEN];
static int white_used_len = 0;


static char black_buf[MAX_BLACK_LEN];
static int black_used_len = 0;


static FW_Read_t Read = NULL;
static CAN_exec_t CanExec = NULL;

/** @defgroup Firewall_Private_Functions
  * @{
  */
static const FIREWALL_VERSION fw_version = { FW_API_VERSION, FW_API_NAME };

/**
  @fn          FIREWALL_VERSION FW_GetVersion (void)
  @brief       Get driver version.
  @return      FIREWALL_VERSION
*/
static FIREWALL_VERSION FW_GetVersion (void) { return fw_version; }

static void FW_RegisterReadFileCB (FW_Read_t read)
{
	Read = read;
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
  * @brief      Execute ATCMD command
  * @param[in]  *filename: pointer to a name of file
	* @param[in]  fsize: size of file
	* @param[in   * cmd: pointer to a ATCMD structure.
  * @retval     1: execute success.
	*             0: not success
  */		
static int32_t FW_AtWhiteCanPass(ATST * at, char state)
{
	int pos = sizeof(struct WhitePassHead);
	struct WhitePass * whitePass;
	struct WhitePassHead *whiteHead = (struct WhitePassHead *)white_buf;
	unsigned char procHead_flag = FALSE;
		
	if (white_used_len <= 0)
		return FW_ERROR;
	if (whiteHead->busType != at->bustype)
		return FW_ERROR;
	for (; pos < white_used_len - sizeof(struct WhitePassTail);)
	{		
		whitePass = (struct WhitePass*)(white_buf + pos);
		pos += whitePass->whiteLen;

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
		//canid ??
		if (whitePass->canId != _htons(at->stdid))
			continue;
		//filter ??
		//if (whitePass->filterId != _htons(at->filter))
		//continue;
				
		if (memcmp(at->data + whitePass->offset, &whitePass->proc[0]/* + whitePass->offset*/, whitePass->compareLen) == 0)
			return FW_SUCCESS;
		else
			continue;
	}
	return FW_ERROR;
}

/**
  * @brief      Execute Black Block Pass
  * @param[in]  *at: pointer to a ATST structure
	* @param[in]  state: current state	
  * @retval     >=0: execute success.
	*             -1: not success
  */		
static int32_t FW_AtBlackBlockPass(ATST * at, char state)
{
	int pos = sizeof(struct WhitePassHead);	
	struct WhitePassHead *whiteHead = (struct WhitePassHead *)black_buf;
	struct WhitePass * whitePass;
	unsigned char procHead_flag = FALSE;
	
	int b_find_cmd = 0;
	if (black_used_len <= 0)
		return FW_ERROR;
	if (whiteHead->busType != at->bustype)
		return FW_ERROR;
	for (; pos < black_used_len - sizeof(struct WhitePassTail);)
	{		
		whitePass = (struct WhitePass*)(black_buf + pos);
		pos += whitePass->whiteLen;


		//canid ??
		if (whitePass->canId != _htons(at->stdid))
			continue;
		//filter ??
		//if (whitePass->filterId != _htons(at->filter))
		//continue;
		
		if (memcmp(at->data + whitePass->offset, &whitePass->proc[0] + whitePass->offset, whitePass->compareLen) == 0)			
			b_find_cmd = 1; /*find command*/
		else
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
		
		//blocked cmd
		return ATCAN_TBOX_PREVENT;
	}

	if (b_find_cmd)
		return ATCAN_TBOX_EXECED;
	else
		return FW_ERROR;
}



/**
  * @brief      Execute ATCMD command
  * @param[in]  *filename: pointer to a name of file
	* @param[in]  fsize: size of file
	* @param[in]  * cmd: pointer to a ATCMD structure.
  * @param[in]  CAN_exec: is callback function.
  * @retval     0: execute success.
	*             -1: not success
  */		
static int32_t FW_atcmd_exec_command(char *filename, int fsize, ATCMD * cmd, char state)
{
	int i = 0;
	int cnt = fsize / sizeof(ATST);
	
	for (i = 0; i < cnt; i++)
	{		
		ATST at;	
    if(Read != NULL) {
			Read(filename,i * sizeof(ATST), &at, sizeof(ATST)); 
		}
		if (strcmp((char *)cmd->command, (char *)at.command) != 0)
			continue;
		//copy can to cmd		
		memcpy(cmd->data, at.data, 8);		
		cmd->stdid = at.stdid;
		if( at.bustype == BUS_CAN)
		{
			if (black_used_len > 0)
			{
				if (FW_AtBlackBlockPass(&at, state) != ATCAN_TBOX_PREVENT)
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

static int32_t FW_atcan_exec_command(int buf_size, ATCAN *cmd,char state)
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
			if (FW_AtBlackBlockPass(&at, state) != ATCAN_TBOX_PREVENT)
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

/**
  * @brief  Enables While
  * @param  None
  * @retval None
  */	
static void FW_EnableWhite(char * whichfile, int fsize)
{
	
	if (fsize > MAX_WHITE_LEN)
		return ;
  if(Read == NULL) 
		return;
	Read(whichfile, 0, white_buf, fsize);		
	white_used_len = fsize;
}

/**
  * @brief  Disables While
  * @param  None
  * @retval None
  */	
static void FW_DisableWhite(void)
{
	white_used_len = 0;	
}

/**
  * @brief  Enables Black
  * @param  None
  * @retval None
  */	
static void FW_EnableBlack(char * whichfile, int fsize)
{
	
	if (fsize > MAX_BLACK_LEN)
		return;
	if(Read == NULL) 
		return;
	Read(whichfile, 0, black_buf, fsize);
	black_used_len = fsize;			
}

/**
  * @brief  Disables Black
  * @param  None
  * @retval None
  */	
static void FW_DisableBlack(void)
{
	black_used_len = 0;
}

/**
  * @brief  Get length of the WhitePass
  * @param  None
  * @retval size of the WhitePass file
  */	
static int32_t FW_GetWhitePassLen(void)
{
	return white_used_len;
}

/**
  * @brief  Disables Black
  * @param  None
  * @retval None
  */	
static int32_t FW_GetBlackBlockLen(void)
{
	return black_used_len;
}

/**
  * @brief     Checked isWhiteCanPass
  * @param[in] CanRxMsg: Can Rx message structure.
  * @retval    1: success
  *            0: not success
  *
  */	
//static int32_t FW_IsWhiteCanPass(CanRxMsg *rx)
//{
//	ATST at = {0};
//	long state = get_state();
//	at.stdid = rx->StdId;
//	at.dataLen = 8;
//	memcpy(at.data, rx->Data, 8);
//	return FW_AtWhiteCanPass(&at, state);
//}

/**
  * @brief     Checked isBlackBlockPass
  * @param[in] CanRxMsg: Can Rx message structure.
  * @retval    1: success
  *            0: not success
  */	
//static int32_t FW_IsBlackBlockPass(CanRxMsg *rx)
//{
//	ATST at = {0};
//	long state = get_state();
//	at.stdid = rx->StdId;
//	at.dataLen = 8;
//	memcpy(at.data, rx->Data, 8);
//	return FW_AtBlackBlockPass(&at, state);
//}

	
/* Create Firewall function */
VT_FW_API VT_FW_Framework = {
	FW_GetVersion, 
  FW_EnableBlack,  
  FW_EnableWhite,
  FW_DisableBlack,  
	FW_DisableWhite,
	FW_GetWhitePassLen,	
	FW_GetBlackBlockLen,
	FW_AtWhiteCanPass,
	FW_AtBlackBlockPass,
	FW_atcan_exec_command,
  FW_atcmd_exec_command,
	FW_RegisterReadFileCB,
	FW_RegisterCanExecCB	
};

/**
  * @}
  */

/******************* END OF FILE************************************/
