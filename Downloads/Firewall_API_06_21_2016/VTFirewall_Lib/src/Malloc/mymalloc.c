#include "mymalloc.h"   
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

//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
 mem_init, //内存初始化
 mem_perused,//内存使用率
 0,   //内存池
 0,   //内存管理状态表
 0,     //内存管理未就绪
};

//内存管理初始化  
void mem_init(void)  
{  
    memset(mallco_dev.membase, 0, sizeof(mallco_dev.membase));//内存池素有数据清零  
    mallco_dev.memrdy=1;//内存管理初始化OK  
}  
//获取内存使用率
//返回值:使用率(0~100)
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
//内存分配(内部调用)
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
unsigned long mem_malloc(unsigned long size)  
{  
    signed long offset=0;  
    unsigned short nmemb; //需要的内存块数  
    unsigned short cmemb=0;//连续空内存块数
    unsigned long i;  
    if(!mallco_dev.memrdy)
		mallco_dev.init();//未初始化,先执行初始化 
    if(size==0)
		return 0XFFFFFFFF;//不需要分配

    nmemb=size/MEM_BLOCK_SIZE;   //获取需要分配的连续内存块数
    if(size%MEM_BLOCK_SIZE)
		nmemb++;  
    for(offset=MEM_ALLOC_TABLE_SIZE-1;offset>=0;offset--)//搜索整个内存控制区  
    {     
		if(!mallco_dev.memmap[offset])
			cmemb++; //连续空内存块数增加
		else
		 	cmemb=0;       //连续内存块清零
		if(cmemb==nmemb)      //找到了连续nmemb个空内存块
		{
			for(i=0;i<nmemb;i++)      //标注内存块非空 
			{  
			    mallco_dev.memmap[offset+i] = nmemb;  
			}  
			return (offset*MEM_BLOCK_SIZE);//返回偏移地址  
		}
    }  
    return 0XFFFFFFFF;//未找到符合分配条件的内存块  
}  
//释放内存(内部调用) 
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  
unsigned char mem_free(unsigned long offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//未初始化,先执行初始化
 	{
  		mallco_dev.init();    
        return 1;//未初始化  
    }  
    if(offset<MAX_MEM_SIZE)//偏移在内存池内. 
    {  
        unsigned long index=offset/MEM_BLOCK_SIZE;//偏移所在内存块号码  
        unsigned long nmemb=mallco_dev.memmap[index];   //内存块数量
        for(i=0;i<nmemb;i++)     //内存块清零
        {  
            mallco_dev.memmap[index+i]=0;  
        }  
        return 0;  
    }
	else
	   return 2;//偏移超区了.  
}  
//释放内存(外部调用) 
//ptr:内存首地址 
void myfree(void *ptr)  
{  
	
	unsigned long offset;  
	//return free(ptr);
    if(ptr==NULL)return;//地址为0.  
    offset=(unsigned long)ptr-(unsigned long)&mallco_dev.membase;  
    mem_free(offset);//释放内存     
}  
//分配内存(外部调用)
//size:内存大小(字节)
//返回值:分配到的内存首地址.
void *mymalloc(unsigned long size)  
{  
	
    unsigned long offset;  
	//return malloc(size);
    offset=mem_malloc(size);  
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((unsigned long)&mallco_dev.membase+offset);  
}  
//重新分配内存(外部调用)
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
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
			memcpy((void*)((unsigned long)&mallco_dev.membase+offset),ptr,size);//拷贝旧内存内容到新内存   
			myfree(ptr);               //释放旧内存
		}
        return (void*)((unsigned long)&mallco_dev.membase+offset);          //返回新内存首地址
    }  
}