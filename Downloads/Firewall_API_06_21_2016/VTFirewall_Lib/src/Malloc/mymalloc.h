#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__
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
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL
#define NULL 0
#endif

#define MEM_BLOCK_SIZE   32          //�ڴ���СΪ32�ֽ�
#define MAX_MEM_SIZE   20*1024       //�������ڴ� 10K
#define MEM_ALLOC_TABLE_SIZE MAX_MEM_SIZE/MEM_BLOCK_SIZE //�ڴ���С

//�ڴ���������
struct _m_mallco_dev
{
 void (*init)(void);     //��ʼ��
 unsigned char (*perused)(void);         //�ڴ�ʹ����
 unsigned char  membase[MAX_MEM_SIZE];   //�ڴ��
 unsigned short memmap[MEM_ALLOC_TABLE_SIZE];  //�ڴ����״̬��
 unsigned char  memrdy;        //�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;  //��mallco.c���涨��

void mymemset(void *s,unsigned char c,unsigned long count);  //�����ڴ�
void mymemcpy(void *des,void *src,unsigned long n);//�����ڴ� 

void mem_init(void);      //�ڴ�����ʼ������(��/�ڲ�����)
unsigned long mem_malloc(unsigned long size);     //�ڴ����(�ڲ�����)
unsigned char mem_free(unsigned long offset);     //�ڴ��ͷ�(�ڲ�����)
unsigned char mem_perused(void);      //����ڴ�ʹ����(��/�ڲ�����) 
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(void *ptr);       //�ڴ��ͷ�(�ⲿ����)
void *mymalloc(unsigned long size);     //�ڴ����(�ⲿ����)
void *myrealloc(void *ptr,unsigned long size);  //���·����ڴ�(�ⲿ����)


#ifdef __cplusplus
}
#endif

#endif
