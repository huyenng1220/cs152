/**
  ******************************************************************************
  * @file    Project/Firewall/fw_user_if.h
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
#ifndef __FW_USER_IF_H
#define __FW_USER_IF_H

#ifdef __cplusplus
 extern "C" {
#endif
	 
/* Includes ------------------------------------------------------------------*/
#include <stddef.h>
#include <stdint.h>
	 
#ifndef NULL
#define NULL 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef FW_ERROR
#define FW_ERROR -1
#endif	

#ifndef FW_SUCCESS
#define FW_SUCCESS 0
#endif

	 
#define BLACK_FILE "d4"
#define WHITE_FILE "d2"

#define MAX_WHITE_LEN (510)
#define MAX_BLACK_LEN (510)
	 
typedef struct _ATST
{	
#define BUS_CAN 0x00
#define BUS_KLINE14230 0x01
#define BUS_KLINE9141 0x02
	unsigned char bustype;	
	unsigned char dataLen;	
	unsigned short stdid;
	unsigned short filter;
	unsigned long baud;
	unsigned char command[16];
#define AT_STATE_STILL 0x01
#define AT_STATE_POWERON 0x02
#define AT_STATE_RUNNING 0x03
	unsigned char data[12];
}ATST;

typedef struct _ATCMD
{
	unsigned long atExeced;
	unsigned char command[16];
	unsigned short stdid;
	unsigned char  data[8];
}ATCMD;

typedef struct _ATCAN
{
#define ATCAN_TBOX_PREVENT 0x02
#define ATCAN_TBOX_EXECED 0x04
	unsigned long atExeced;
	unsigned short stdid;
	unsigned short filterid;
	unsigned char  data[8];
}ATCAN;

struct WhitePassHead{
	unsigned short procHead;
	unsigned char commandType;
	unsigned short commandLen;
	unsigned char busType;
};

struct WhitePass
{
	unsigned char whiteLen;
	unsigned char procHead1;
	unsigned char canType;
	unsigned short canId;
	unsigned short filterId;
	unsigned char hasOffset;
	unsigned char offset;
	unsigned char compareLen;
	unsigned char proc[1];
};

struct WhitePassTail{
	unsigned char chedk;	
	unsigned short procEnd;
};



typedef  void   (*CAN_exec_t)(unsigned short stdid, unsigned short filter, char *data, char * retData);	
typedef int32_t (*FW_Read_t)  (char* whichfile, int pos, void * buf, int len);     //< Pointer to Read Data for White List or Black List.
//typedef int32_t (*FW_Write_t) (char* whichfile, int pos, void * buf, int len);   //< Pointer to Write Data for White List or Black List.

/**
\brief Firewall Version
*/
typedef struct _FIREWALL_VERSION {
  uint16_t api;                         ///< API version  
  char *name;	
} FIREWALL_VERSION;

typedef struct _VT_FW_API {
  FIREWALL_VERSION         (*GetVersion)            (void);                                            //< Pointer to \ref FIREWALL_GetVersion           : Get firewall version.
  void                     (*EnableBlack)           (char * whichfile, int fsize);                     //< Pointer to \ref FW_Initialize                 : Initialize FW interface.
  void                     (*EnableWhite)           (char * whichfile, int fsize);                     //< Pointer to \ref FW_Uninitialize               : De-initialize Firewall interface.
  void                     (*DisableBlack)          (void);                                            //< Pointer to \ref FW_GetWhiteListLen            : Get length of White list.
  void                     (*DisableWhite)          (void);                                            //< Pointer to \ref FW_IsWhitePass                : Check ID existing in White List or Black List.
  int32_t                  (*GetWhitePassLen)       (void);     
  int32_t                  (*GetBlackBlockLen)      (void);
	int32_t                  (*AtWhiteCanPass)        (ATST * at, char state);
	int32_t                  (*AtBlackBlockPass)      (ATST * at, char state);
	int32_t                  (*ExecATCANCommand)      (int buf_size, ATCAN *cmd,char state);
	int32_t                  (*ExecATCMDCommand)      (char *filename, int fsize, ATCMD * cmd,char state);
	void                     (*RegisterReadFileCB)    (FW_Read_t read);
	void                     (*RegisterCanExecCB)     (CAN_exec_t can_exec);	
} const VT_FW_API;

extern   VT_FW_API VT_FW_Framework;
/**
  * @}
  */
	 
#ifdef __cplusplus
}
#endif

#endif /* __FW_USER_IF_H */
