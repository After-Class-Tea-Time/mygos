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


extern void uart_init();                // 初始化串口
extern int  uart_puts(const char* str); // 发送字符串

char LINE[]    = "===================================================\n";
char CONTEXT[] = "中文测试！\n";

__attribute__((section(".boot.text"))) void
kernel_init()
{
    uart_init();
    uart_puts(LINE);

    uart_puts("Hello, MyGOS!\n");
    uart_puts(CONTEXT);

    uart_puts("\nloop...\n");
    uart_puts(LINE);
    while(1); // stop here!
}
