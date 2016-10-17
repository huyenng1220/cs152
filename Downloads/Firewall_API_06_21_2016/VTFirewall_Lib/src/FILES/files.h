#ifndef __FILES__H__
#define __FILES__H__
#include "../jsonfile/jsonfile.h"
#include "../fats/ff.h"

#ifdef __cplusplus
extern "C"
{
#endif

extern FIL  f_config;
extern FIL  f_old;
extern FIL  f_patch;
extern FIL  f_new;
extern FIL  f_at;
extern FIL  f_black;


void InitFiles();
FIL*  getFile(char * file);
int ReCreateFile(char * whichfile);
int WriteFile1(char* whichfile,int pos,  void * buf, int len);
int ReadFile1(char* whichfile, int pos, void * buf, int len);
int WriteFile2(char* whichfile,  void * buf, int len);
int ReadFile2(char* whichfile,  void * buf, int len);
int Gets(char* whichfile);
int Seek(char * whichfile, int pos);
int FlushFile(char *whichfile);
int MoveFile1(char *whichfileSrc, char *whichfileDst);

#ifdef __cplusplus
}
#endif

#endif
