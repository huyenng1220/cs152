#ifndef __GEN_DATA_TEST_H
#define __GEN_DATA_TEST_H

#include "file.h"


#define MAX_ITEM_BLACK_LIST 3
#define MAX_ITEM_WHITE_LIST 3


int Gen_BLACK_LIST_Data_To_Test(void);
int Gen_WHITE_LIST_Data_To_Test(void);
void Get_File_Size(void);
extern int black_file_size;
extern int white_file_size;

#endif
