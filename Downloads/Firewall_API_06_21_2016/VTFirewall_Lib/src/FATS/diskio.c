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
#define BlockSize            256 /* Block Size in Bytes [�����block�����ǡ���������SD����ÿ������Ϊ512�ֽ�]*/
#define BufferWordsSize      (BlockSize >> 2)	//128������ο�MAIN�еĽ��ͣ���128��32λ�Ļ����������512���ֽ�

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive [�ú�������Ҫ��д��]                                                   */
/* ע�⣺�Ӻ����Ĵ�������������ν��drv�������������һ�������������ļ�Ӳ�������ͣ�����switch��俴
   drv = ATA = 0����ʾ��IDEӲ�̵�һ�ֽӿ�
   drv = MMC = 1: ��ʾ����MMC���Ľӿڣ�Ҳ����SD��
   drv = USB = 2: ��ʾ����USB�洢�豸�Ľӿ�
   �����￴��������Ӧ��Ҫѡ�����MMC�ӿ�,�����������Ϊ1*/
DSTATUS disk_initialize (
	BYTE drv				/* Physical drive nmuber (0..) */
)
{	
  	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Return Disk Status  [�ú�����Ҫ��д]                                                  */

DSTATUS disk_status (
	BYTE drv		/* Physical drive nmuber (0..) */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s) [�ú�����Ҫ��д]                                       */

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
/* Write Sector(s) [�ú�����Ҫ��д]                                                      */

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
/* Miscellaneous Functions [�ú�����Ҫ��д]                                              */

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

