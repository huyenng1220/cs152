#ifndef __FILE_H
#define __FILE_H

#include "fw_user_if.h"
#include <string.h>
#include "eeprom.h"

#define START_BLACK_ADDRESS 0x0000
#define BLACK_FILE_SIZE_ADDRESS   0x01FE

#define START_WHITE_ADDRESS 0x0200
#define WHITE_FILE_SIZE_ADDRESS   0x03FE

void EEL_Init(void);
int32_t Read  (char* whichfile, int pos, void * buf, int len);
int32_t Write  (char* whichfile, int pos, void * buf, int len);

#endif
