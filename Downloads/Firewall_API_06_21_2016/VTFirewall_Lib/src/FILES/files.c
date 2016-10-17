#include "files.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#define max(x,y)  ( (x)>(y)?(x):(y) )
#define min(x,y)  ( (x)>(y)?(y):(x) )


FIL  f_config ={0};
FIL  f_old ={0};
FIL  f_patch ={0};
FIL  f_new ={0};
FIL  f_at ={0};
FIL  f_black ={0};

extern struct FlashInfo flashInfo;
FATFS fatfs;
void InitFiles()
{

	
	FRESULT res = f_mount(0, &fatfs);
	/*res = f_open(&f_config, "0:/config.bat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE );

	if (res == FR_NO_FILESYSTEM)
		res = f_mkfs(0,0,2*1024*1024);
	else if (res == FR_OK)
		f_close(&f_config);
	 */

	res = f_mkfs(0,0,2*1024*1024);

	getFile(JSONFILE_CONFIG);
	getFile(JSONFILE_OLD);
	getFile(JSONFILE_PATCH);
	getFile(JSONFILE_NEW);
	getFile(JSONFILE_AT);
	getFile(JSONFILE_BLACK);
}


FIL*  getFile(char * file)
{
	if (strcmp(JSONFILE_CONFIG, file)== 0)
	{	
		if (f_config.fs == NULL)
		{
			UINT wlen;
			FRESULT res = f_open(&f_config, "0:/config.bat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
			if (res != FR_OK)
				return NULL;
			if (f_config.fsize == 0)
			{
				memset(&flashInfo, 0, sizeof(struct FlashInfo));
				flashInfo.version = 0x01;
				f_write(&f_config, &flashInfo, sizeof(struct FlashInfo), &wlen);  
			}
		}
		return &f_config;
	}
	else  if (strcmp(JSONFILE_OLD, file)== 0)
	{
			if (f_old.fs == NULL)
		{
			f_open(&f_old,"0:/old.bat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE );
		}
		return &f_old;
	}
	else  if (strcmp(JSONFILE_PATCH, file)== 0)
	{
		if (f_patch.fs == NULL)
		{
			f_open(&f_patch, "0:/patch.dat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		}
		return &f_patch;
	}
	else  if (strcmp(JSONFILE_NEW, file)== 0)
	{
		if (f_new.fs == NULL)
		{
			f_open(&f_new, "0:/new.dat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		}
		return &f_new;
	}
	else  if (strcmp(JSONFILE_AT, file)== 0)
	{
		if (f_at.fs == NULL)
		{
			f_open(&f_at, "0:/at.dat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		}
		return &f_at;
	}
	else  if (strcmp(JSONFILE_BLACK, file)== 0)
	{
		if (f_black.fs == NULL)
		{
			f_open(&f_black, "0:/black.dat", FA_OPEN_ALWAYS|FA_READ|FA_WRITE);
		}
		return &f_black;
	}
	return NULL;
}

int ReCreateFile(char * whichfile)
{
	FIL *f = getFile(whichfile);
	f_lseek(f, 0);
	if (f != NULL)
		f_truncate(f);
	return 0;
}


int WriteFile1(char* whichfile, int pos, void * buf, int len)
{
	UINT wlen = 0;
   FIL * f = getFile(whichfile);
   if (f == NULL)
   		return -1;
   f_lseek(f, pos);
   f_write(f, buf, len, &wlen);
   return wlen; 
}
int ReadFile1(char * whichfile, int pos, void * buf, int len)
{
   	UINT wlen = 0;
   FIL * f = getFile(whichfile);
   if (f == NULL)
   	return -1;
   f_lseek(f, pos);
   f_read(f, buf, len, &wlen);
   return wlen;
}
int WriteFile2(char* whichfile,void * buf, int len)
{
	UINT wlen = 0;
   FIL * f = getFile(whichfile);
   if (f == NULL)
   		return -1;
   f_write(f, buf, len, &wlen);
   return wlen; 
}
int ReadFile2(char * whichfile, void * buf, int len)
{
   	UINT wlen = 0;
   FIL * f = getFile(whichfile);
   if (f == NULL)
   	return -1;
   f_read(f, buf, len, &wlen);
   return wlen;
}

int Gets(char* whichfile)
{	
	char ch = -1;
	int wlen = 0;
	FIL * f = getFile(whichfile);
   	if (f == NULL)
   	return -1;
   	if ((f_read(f, &ch, 1, &wlen) == FR_OK) && wlen == 1)
   		return ch;
	else
		return -1;
}

int Seek(char * whichfile, int pos)
{
	FIL * f = getFile(whichfile);
   	if (f == NULL)
   	return -1;
 	f_lseek(f, pos);
	return 0;
}

char tmpBuf[512];
int MoveFile1(char *whichfileSrc, char *whichfileDst)
{
	FIL * src = getFile(whichfileSrc);
	FIL * dst = getFile(whichfileDst);
	int pos = 0;
	if (src == NULL || dst == NULL)
		return -1;
   	ReCreateFile(whichfileDst);
	while(pos < src->fsize)
	{
		int len = min(512, src->fsize - pos);
		ReadFile1(whichfileSrc, pos, tmpBuf, len);
		WriteFile1(whichfileDst, pos, tmpBuf, len);
		pos += len;		
	}
	return 0;
}

int FlushFile(char *whichfile)
{
	FIL* f = getFile(whichfile);
	if (f != NULL)
	{
		f_close(f);
		getFile(whichfile);
	}
	return 0;
}