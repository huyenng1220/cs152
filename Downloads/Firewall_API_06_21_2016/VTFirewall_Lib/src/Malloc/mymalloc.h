#ifndef __MY_MALLOC_H__
#define __MY_MALLOC_H__
//////////////////////////////////////////////////////////////////////////////////  
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK 开发板
//内存管理 代码     
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2011/7/5 
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 正点原子 2009-2019
//All rights reserved
//********************************************************************************
//没有更新信息
//////////////////////////////////////////////////////////////////////////////////   
#ifdef __cplusplus
extern "C"
{
#endif

#ifndef NULL
#define NULL 0
#endif

#define MEM_BLOCK_SIZE   32          //内存块大小为32字节
#define MAX_MEM_SIZE   20*1024       //最大管理内存 10K
#define MEM_ALLOC_TABLE_SIZE MAX_MEM_SIZE/MEM_BLOCK_SIZE //内存表大小

//内存管理控制器
struct _m_mallco_dev
{
 void (*init)(void);     //初始化
 unsigned char (*perused)(void);         //内存使用率
 unsigned char  membase[MAX_MEM_SIZE];   //内存池
 unsigned short memmap[MEM_ALLOC_TABLE_SIZE];  //内存管理状态表
 unsigned char  memrdy;        //内存管理是否就绪
};
extern struct _m_mallco_dev mallco_dev;  //在mallco.c里面定义

void mymemset(void *s,unsigned char c,unsigned long count);  //设置内存
void mymemcpy(void *des,void *src,unsigned long n);//复制内存 

void mem_init(void);      //内存管理初始化函数(外/内部调用)
unsigned long mem_malloc(unsigned long size);     //内存分配(内部调用)
unsigned char mem_free(unsigned long offset);     //内存释放(内部调用)
unsigned char mem_perused(void);      //获得内存使用率(外/内部调用) 
////////////////////////////////////////////////////////////////////////////////
//用户调用函数
void myfree(void *ptr);       //内存释放(外部调用)
void *mymalloc(unsigned long size);     //内存分配(外部调用)
void *myrealloc(void *ptr,unsigned long size);  //重新分配内存(外部调用)


#ifdef __cplusplus
}
#endif

#endif
