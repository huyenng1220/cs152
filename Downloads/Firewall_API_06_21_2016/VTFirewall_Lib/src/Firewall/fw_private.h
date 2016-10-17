/**
  ******************************************************************************
  * @file    Project/Firewall/fw_private.h
  * @author
  * @version
  * @date    08-June-2016
  * @brief   Library FW file.
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FW_PRIVATE_H
#define __FW_PRIVATE_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
/* Includes ------------------------------------------------------------------*/
#include "fw_user_if.h"
#include "jsonfile.h"
	 

	 
/** @defgroup OBD_Private_Exported_Functions
  * @{
  */
int AtWhiteCanPass( ATST *at, char state );
int AtBlackBlockPass( ATST *at, char state );
int atcan_exec_command(char *filename, int buf_size, ATCAN *cmd);
int atcmd_exec_command(char *filename, int fsize, ATCMD *cmd, CAN_exec_t CAN_exec);
 
/**
  * @}
  */
	 
	 
#ifdef __cplusplus
}
#endif

#endif /* __FW_PRIVATE_H */
