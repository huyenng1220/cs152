/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include "string.h"
#include "ffconf.h"
#include "../FLASH_MANAGE/W25Q32BV.h"

/*-----------------------------------------------------------------------*/
/* Correspondence between physical drive number and physical drive.      */
//extern SD_CardInfo SDCardInfo;
#define BlockSize            256 /* Block Size in Bytes [这里的block含义是“扇区”，SD卡的每个扇区为512字节]*/
#define BufferWordsSize      (BlockSize >> 2)	//128，含义参考MAIN中的解释，是128个32位的缓冲区，存放512个字节

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive [该函数是需要改写的]                                                   */
/* 注意：从函数的代码上来看，所谓的drv这个参数，是用一个数字来代表文件硬件的类型，从其switch语句看
   drv = ATA = 0：表示是IDE硬盘的一种接口
   drv = MMC = 1: 表示的是MMC卡的接口，也就是SD卡
   drv = USB = 2: 表示的是USB存储设备的接口
   从这里看来，我们应该要选择的是MMC接口,输入参数必须为1*/
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
  	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status  [该函数需要改写]                                                  */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s) [该函数需要改写]                                       */

DRESULT disk_read (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
	SPI_Flash_ReadSafe((uint8_t*)buff, sector*_MAX_SS, count*_MAX_SS);
	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s) [该函数需要改写]                                                      */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
	SPI_Flash_WriteSafe((uint8_t*)buff, sector*_MAX_SS, count*_MAX_SS);
  return RES_OK;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions [该函数需要改写]                                              */

DRESULT disk_ioctl (
	BYTE drv,		/* Physical drive nmuber (0..) */
	BYTE ctrl,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{

	if (GET_SECTOR_COUNT == ctrl)
	{
		*(int*)buff = 2*1024*1024/512;
	}
	return RES_OK;
}

