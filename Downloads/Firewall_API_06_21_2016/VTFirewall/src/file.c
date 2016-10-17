#include "file.h"


/* Virtual address defined by the user: 0xFFFF value is prohibited */
uint16_t VirtAddVarTab[NumbOfVar]= {0};

void EEL_Init(void)
{
	int i;
	
	/* Fill virtual address table */
	for(i=0; i < NumbOfVar; i++)
	{
		VirtAddVarTab[i]=i*2 + 0xD000;
	}
	
	 /* Unlock the Flash Program Erase controller */
	FLASH_Unlock();

	/* EEPROM Init */
	EE_Init();		
}

static int32_t EEProm_Write_HalfWord(uint16_t virtAddress, uint16_t buf)
{
  uint16_t address;
	uint16_t result;
	
	address = virtAddress/2; /* Each virtual address contain 2 bytes */
	if(address >= NumbOfVar)
		return -1;
	
	/* Write data */
	result = EE_WriteVariable(VirtAddVarTab[address], buf);
	if(result != FLASH_COMPLETE)
		return -1;
	
	return 0;
}

static int32_t EEProm_Read_HalfWord(uint16_t virtAddress, uint16_t *buf)
{
 	int result;	
	uint16_t address;
	
	address = virtAddress/2;
	if(address >= NumbOfVar)
		return -1;	
  	
	result = EE_ReadVariable(VirtAddVarTab[address], buf);	
	if(result != 0)
		return -1;

	return 0;	
}

int32_t Read  (char* whichfile, int pos, void * buf, int len)
{
	int result = -1;
	int i;
	uint16_t virtAddress;
	uint16_t *ptr = buf;
	
	if (strcmp(BLACK_FILE, whichfile) == 0)
	{ 
		virtAddress = (uint16_t)pos + START_BLACK_ADDRESS;			
		result = 0;
	}
	else
	{	
		if (strcmp(WHITE_FILE, whichfile) == 0)
		{ 
			virtAddress = (uint16_t)pos + START_WHITE_ADDRESS;  
			result = 0;
		}		
	}
	if(result == 0)
	{
		for(i = 0;i < len/sizeof(uint16_t); i++)
		{
			result = EEProm_Read_HalfWord((virtAddress + i*sizeof(uint16_t)), ptr++);	
			if(result == -1)
				break;
		}
		if(i == 0)
			return -1;
		else
			return i;
	}
	return result;
}

int32_t Write  (char* whichfile, int pos, void * buf, int len)
{
	int result = -1;
	int i;
	uint16_t virtAddress;
	uint16_t *ptr = buf;
	
	if (strcmp(BLACK_FILE, whichfile) == 0)
	{ 
		virtAddress = (uint16_t)pos + START_BLACK_ADDRESS;			
		result = 0;
	}
	else
	{	
		if (strcmp(WHITE_FILE, whichfile) == 0)
		{ 
			virtAddress = (uint16_t)pos + START_WHITE_ADDRESS;
			result = 0;
		}		
	}
	
	if(result == 0)
	{
		for(i = 0;i < len/sizeof(uint16_t); i++)
		{
			result = EEProm_Write_HalfWord((virtAddress + i*sizeof(uint16_t)), *ptr);
			ptr++;
			if(result == -1)
				break;
		}
		if(i == 0)
			return -1;
		else
			return i;
	}
	
	return result;
}
