#include "malloc.h"	   

//内存池(64字节对齐)
__align(64) uint8_t mem1base[MEM_MAX_SIZE] __attribute__((at(0x20010000)));

//内存管理表
uint32_t mem1mapbase[MEM_ALLOC_TABLE_SIZE] __attribute__((at(0x20010000 + MEM_MAX_SIZE)));

//内存管理参数	   
const uint32_t memtblsize   = MEM_ALLOC_TABLE_SIZE;	    //内存表大小
const uint32_t memblksize   = MEM_BLOCK_SIZE;           //内存分块大小
const uint32_t memsize      = MEM_MAX_SIZE;             //内存总大小

//内存管理控制器
struct _m_mallco_dev mallco_dev=
{
	my_mem_init,			//内存初始化
	my_mem_perused,			//内存使用率
	mem1base,               //内存池
	mem1mapbase,            //内存管理状态表
	0                      //内存管理未就绪
};

//复制内存
//*des:目的地址
//*src:源地址
//n:需要复制的内存长度(字节为单位)
void mymemcpy(void *des, void *src, uint32_t n)  
{  
    uint8_t *xdes=des;
	uint8_t *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
} 

//设置内存
//*s:内存首地址
//c :要设置的值
//count:需要设置的内存大小(字节为单位)
void mymemset(void *s, uint8_t c, uint32_t count)  
{  
    uint8_t *xs = s;  
    while(count--)*xs++=c;  
}

//内存管理初始化
void my_mem_init()  
{  
    mymemset(mallco_dev.memmap, 0, memtblsize*4);	//内存状态表数据清零  
 	mallco_dev.memrdy = 1;                          //内存管理初始化OK  
} 

//获取内存使用率
//返回值:使用率(扩大了10倍,0~1000,代表0.0%~100.0%)
uint16_t my_mem_perused()  
{  
    uint32_t used=0;  
    uint32_t i;  
    for(i=0; i<memtblsize; i++) {  
        if(mallco_dev.memmap[i])used++; 
    } 
    return (used*1000)/(memtblsize);  
} 

//内存分配(内部调用)
//size:要分配的内存大小(字节)
//返回值:0XFFFFFFFF,代表错误;其他,内存偏移地址 
uint32_t my_mem_malloc(uint32_t size)  
{
    signed long offset=0;
    uint32_t nmemb;	//需要的内存块数
	uint32_t cmemb=0;//连续空内存块数
    uint32_t i;
    if(!mallco_dev.memrdy)mallco_dev.init();//未初始化,先执行初始化 
    if(size==0)return 0XFFFFFFFF;//不需要分配
    nmemb=size/memblksize;  	//获取需要分配的连续内存块数
    if(size%memblksize)nmemb++;  
    for(offset=memtblsize-1;offset>=0;offset--)//搜索整个内存控制区  
    {     
		if(!mallco_dev.memmap[offset])cmemb++;//连续空内存块数增加
		else cmemb=0;								//连续内存块清零
		if(cmemb==nmemb)							//找到了连续nmemb个空内存块
		{
            for(i=0;i<nmemb;i++)  					//标注内存块非空 
            {  
                mallco_dev.memmap[offset+i]=nmemb;  
            }  
            return (offset*memblksize);//返回偏移地址  
		}
    }  
    return 0XFFFFFFFF;//未找到符合分配条件的内存块  
} 

//释放内存(内部调用)
//offset:内存地址偏移
//返回值:0,释放成功;1,释放失败;  
uint8_t my_mem_free(uint32_t offset)  
{  
    int i;  
    if(!mallco_dev.memrdy)//未初始化,先执行初始化
	{
		mallco_dev.init();    
        return 1;//未初始化  
    }  
    if(offset<memsize)//偏移在内存池内. 
    {  
        int index=offset/memblksize;			//偏移所在内存块号码  
        int nmemb=mallco_dev.memmap[index];	//内存块数量
        for(i=0;i<nmemb;i++)  						//内存块清零
        {  
            mallco_dev.memmap[index+i]=0;  
        }  
        return 0;  
    }else return 2;//偏移超区了.  
} 

//释放内存(外部调用)
//ptr:内存首地址 
void myfree(void *ptr)  
{  
	uint32_t offset;   
	if(ptr==NULL)return;//地址为0.  
 	offset=(uint32_t)ptr-(uint32_t)mallco_dev.membase;     
    my_mem_free(offset);	//释放内存      
} 

//分配内存(外部调用)
//size:内存大小(字节)
//返回值:分配到的内存首地址.
void *mymalloc(uint32_t size)  
{  
    uint32_t offset;   
	offset=my_mem_malloc(size);  	   	 	   
    if(offset==0XFFFFFFFF)return NULL;  
    else return (void*)((uint32_t)mallco_dev.membase+offset);  
}

//重新分配内存(外部调用)
//*ptr:旧内存首地址
//size:要分配的内存大小(字节)
//返回值:新分配到的内存首地址.
void *myrealloc(void *ptr,uint32_t size)  
{  
    uint32_t offset;    
    offset=my_mem_malloc(size);   	
    if(offset==0XFFFFFFFF)return NULL;     
    else  
    {  									   
	    mymemcpy((void*)((uint32_t)mallco_dev.membase+offset),ptr,size);	//拷贝旧内存内容到新内存   
        myfree(ptr);  											  		//释放旧内存
        return (void*)((uint32_t)mallco_dev.membase+offset);  				//返回新内存首地址
    }  
}

