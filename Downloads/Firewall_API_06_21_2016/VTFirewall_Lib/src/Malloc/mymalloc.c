#include "mymalloc.h"   
//////////////////////////////////////////////////////////////////////////////////  
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK ������
//�ڴ���� ����     
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2011/7/5 
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************
//û�и�����Ϣ
//////////////////////////////////////////////////////////////////////////////////   

//�ڴ���������
struct _m_mallco_dev mallco_dev=
{
 mem_init, //�ڴ��ʼ��
 mem_perused,//�ڴ�ʹ����
 0,   //�ڴ��
 0,   //�ڴ����״̬��
 0,     //�ڴ����δ����
};

//�ڴ�����ʼ��  
void mem_init(void)  
{  
    memset(mallco_dev.membase, 0, sizeof(mallco_dev.membase));//�ڴ��������������  
    mallco_dev.memrdy=1;//�ڴ�����ʼ��OK  
}  
//��ȡ�ڴ�ʹ����
//����ֵ:ʹ����(0~100)
unsigned char mem_perused(void)  
{  
    unsigned long used=0;  
    unsigned long i;  
    for(i=0;i<MEM_ALLOC_TABLE_SIZE;i++)  
    {  
        if(mallco_dev.memmap[i])used++; 
    }
	used = used*100;
	used /= MEM_ALLOC_TABLE_SIZE;  
    return used;
}  
//�ڴ����(�ڲ�����)
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:0XFFFFFFFF,�������;����,�ڴ�ƫ�Ƶ�ַ 
unsigned long mem_malloc(unsigned long size)  
{  
    signed long offset=0;  
    unsigned short nmemb; //��Ҫ���ڴ����  
    unsigned short cmemb=0;//�������ڴ����
    unsigned long i;  
    if(!mallco_dev.memrdy)
		mallco_dev.init();//δ��ʼ��,��ִ�г�ʼ�� 
    if(size==0)
		return 0XFFFFFFFF;//����Ҫ����

    nmemb=size/MEM_BLOCK_SIZE;   //��ȡ��Ҫ����������ڴ����
    if(size%MEM_BLOCK_SIZE)
		nmemb++;  
    for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)//���������ڴ������  
    {     
		if(!mallco_dev.memmap[offset])
			cmemb++; //�������ڴ��������
		else
		 	cmemb=0;       //�����ڴ������
		if(cmemb==nmemb)      //�ҵ�������nmemb�����ڴ��
		{
			for(i=0;i<nmemb;i++)      //��ע�ڴ��ǿ� 
			{  
			    mallco_dev.memmap[offset+i] = nmemb;  
			}  
			return (offset*MEM_BLOCK_SIZE);//����ƫ�Ƶ�ַ  
		}
    }  
    return 0XFFFFFFFF;//δ�ҵ����Ϸ����������ڴ��  
}  
//�ͷ��ڴ�(�ڲ�����) 
//offset:�ڴ��ַƫ��
//����ֵ:0,�ͷųɹ�;1,�ͷ�ʧ��;  
unsigned char mem_free(unsigned long offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//δ��ʼ��,��ִ�г�ʼ��
 	{
  		mallco_dev.init();    
        return 1;//δ��ʼ��  
    }  
    if(offset<MAX_MEM_SIZE)//ƫ�����ڴ����. 
    {  
        unsigned long index=offset/MEM_BLOCK_SIZE;//ƫ�������ڴ�����  
        unsigned long nmemb=mallco_dev.memmap[index];   //�ڴ������
        for(i=0;i<nmemb;i++)     //�ڴ������
        {  
            mallco_dev.memmap[index+i]=0;  
        }  
        return 0;  
    }
	else
	   return 2;//ƫ�Ƴ�����.  
}  
//�ͷ��ڴ�(�ⲿ����) 
//ptr:�ڴ��׵�ַ 
void myfree(void *ptr)  
{  
	
	unsigned long offset;  
	//return free(ptr);
    if(ptr==NULL)return;//��ַΪ0.  
    offset=(unsigned long)ptr-(unsigned long)&mallco_dev.membase;  
    mem_free(offset);//�ͷ��ڴ�     
}  
//�����ڴ�(�ⲿ����)
//size:�ڴ��С(�ֽ�)
//����ֵ:���䵽���ڴ��׵�ַ.
void *mymalloc(unsigned long size)  
{  
	
    unsigned long offset;  
	//return malloc(size);
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((unsigned long)&mallco_dev.membase+offset);  
}  
//���·����ڴ�(�ⲿ����)
//*ptr:���ڴ��׵�ַ
//size:Ҫ������ڴ��С(�ֽ�)
//����ֵ:�·��䵽���ڴ��׵�ַ.
void *myrealloc(void *ptr,unsigned long size)  
{  

	
    unsigned long offset;  
	//return realloc(ptr, size);
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  
		if (ptr != NULL)
		{
			memcpy((void*)((unsigned long)&mallco_dev.membase+offset),ptr,size);//�������ڴ����ݵ����ڴ�   
			myfree(ptr);               //�ͷž��ڴ�
		}
        return (void*)((unsigned long)&mallco_dev.membase+offset);          //�������ڴ��׵�ַ
    }  
}