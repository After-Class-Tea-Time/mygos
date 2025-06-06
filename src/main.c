//
// Created by katherinesolar on 25-5-23.
//
// #include "kmemory.h"
// #include "page.h"
// #include "device_tree_parser.h"

// int __attribute__((section(".boot.text"))) kernel_init(void *dtd) {
// 	parse_device_tree(dtd);
// 	page_count=memory.size / PAGE_SIZE;
// 	// 初始化物理页帧分配器
// 	phys_mem_init();

// 	// 创建内核页表
// 	uint64_t* kernel_pagetable = (uint64_t *)page_alloc();
// 	zero(kernel_pagetable, PAGE_SIZE);

// 	// 建立恒等映射
// 	map_kernel_identity(kernel_pagetable);

// 	// 启用分页
// 	enable_paging(kernel_pagetable);
// }


#include "os.h"


extern void uart_init(); // 初始化串口

char LINE[]    = "===================================================\n";
char CONTEXT[] = "中文测试！\n";


// 打印内存信息
void
print_memory_info()
{
    uart_puts("Memory Info:\n");

    print("TEXT:   0x%08x - 0x%08x (%d bytes)\n", TEXT_START, TEXT_END, (int)(TEXT_END - TEXT_START));
    print("DATA:   0x%08x - 0x%08x (%d bytes)\n", DATA_START, DATA_END, (int)(DATA_END - DATA_START));
    print("RODATA: 0x%08x - 0x%08x (%d bytes)\n", RODATA_START, RODATA_END, (int)(RODATA_END - RODATA_START));
    print("BSS:    0x%08x - 0x%08x (%d bytes)\n", BSS_START, BSS_END, (int)(BSS_END - BSS_START));
    print("HEAP:   0x%08x - 0x%08x (%d bytes)\n", HEAP_START, HEAP_START + HEAP_SIZE, (int)HEAP_SIZE);
    print("Total Memory: %d bytes\n", (int)(HEAP_SIZE + (BSS_END - TEXT_START)));

    uart_puts(LINE);
}


void
END()
{
    print("Press Ctrl-A and then X to exit QEMU\n");
    while(1); // stop here!
}


__attribute__((section(".boot.text"))) void
kernel_init()
{
    uart_init();
    uart_puts(LINE);

    uart_puts("Hello, MyGOS!\n");
    uart_puts(CONTEXT);

    uart_puts(LINE);
    print_memory_info();

    uart_puts("\nloop...\n");
    uart_puts(LINE);

    END();
}
