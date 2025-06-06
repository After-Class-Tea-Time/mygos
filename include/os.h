
// os.h

#ifndef OS_H
#define OS_H

#include "memory.h"
#include "task.h"

#include "typedefs.h"


/* 任务调度器相关函数声明 */
void    task_yield();                                   // 任务主动让出 CPU，允许其他任务运行
int32_t task_create(void (*task)());                    // 创建一个新任务，传入任务函数指针
void    task_delete(task_context_ptr task_context_ptr); // 删除指定任务
void    task_delete_current();                          // 删除当前任务
void    task_delay(volatile int count);                 // 延迟执行，消耗 CPU 时间

uint32_t         task_get_count();                      // 获取当前活动任务数
uint32_t         task_get_current();                    // 获取当前任务编号
task_context_ptr task_get_current_context();            // 获取当前任务的上下文指针


/* IO 相关函数声明 */
void     uart_putc(char ch);        // 输出一个字符到串口（阻塞，等待发送缓冲区空）
void     uart_puts(char* s);        // 输出字符串到串口（逐字符发送）
uint32_t print(const char* s, ...); // 格式化输出到串口，类似于 printf
void     panic(char* s);            // 输出错误信息并进入死循环


/* 内存管理 */
void* page_alloc(int npages);
void  page_free(void* p);


/* 锁 */
void spin_lock(void);
void spin_unlock(void);


/* 软件定时器 */
timer_ptr timer_create(void (*callback)(void*), void* arg, uint32_t timeout);
void      timer_delete(timer_ptr timer);


#endif // OS_H
