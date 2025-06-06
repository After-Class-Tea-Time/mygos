
// memory.h

#ifndef _MEMORY_H
#define _MEMORY_H

#include "typedefs.h"


// 这些全局变量在 mem.S 中定义，表示程序的各个内存段的起始和结束地址
extern ptr_t TEXT_START;   // 程序代码段起始地址
extern ptr_t TEXT_END;     // 程序代码段结束地址
extern ptr_t DATA_START;   // 数据段起始地址
extern ptr_t DATA_END;     // 数据段结束地址
extern ptr_t RODATA_START; // 只读数据段起始地址
extern ptr_t RODATA_END;   // 只读数据段结束地址
extern ptr_t BSS_START;    // BSS 段起始地址
extern ptr_t BSS_END;      // BSS 段结束地址
extern ptr_t HEAP_START;   // 堆起始地址
extern ptr_t HEAP_SIZE;    // 堆大小


// 页面
struct Page
{
    uint8_t flags;
};

typedef struct Page* page_ptr; // 页面指针类型


#define PAGE_SIZE 4096               // 页面大小（字节）
#define PAGE_ORDER 12                // 页面大小的对数
#define PAGE_TAKEN (uint8_t)(1 << 0) // 页面是否使用
#define PAGE_LAST (uint8_t)(1 << 1)  // 是否是一个连续页面的最后一页


#endif // _MEMORY_H
