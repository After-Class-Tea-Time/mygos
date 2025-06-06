
// task.h

#ifndef _TASK_H
#define _TASK_H

#include "typedefs.h"


// 任务上下文
struct task_context
{
    //    x0;  // [00] ignore x0
    reg_t ra;  // [01] 返回地址
    reg_t sp;  // [02] 栈指针
    reg_t gp;  // [03] 全局指针
    reg_t tp;  // [04] 线程指针
    reg_t t0;  // [05] 临时寄存器 00
    reg_t t1;  // [06] 临时寄存器 01
    reg_t t2;  // [07] 临时寄存器 02
    reg_t s0;  // [08] 保存函数调用时的s0寄存器
    reg_t s1;  // [09] 保存函数调用时的s1寄存器
    reg_t a0;  // [10] 第一个参数寄存器
    reg_t a1;  // [11] 第二个参数寄存器
    reg_t a2;  // [12] 第三个参数寄存器
    reg_t a3;  // [13] 第四个参数寄存器
    reg_t a4;  // [14] 第五个参数寄存器
    reg_t a5;  // [15] 第六个参数寄存器
    reg_t a6;  // [16] 第七个参数寄存器
    reg_t a7;  // [17] 第八个参数寄存器
    reg_t s2;  // [18] 保存函数调用时的s2寄存器
    reg_t s3;  // [19] 保存函数调用时的s3寄存器
    reg_t s4;  // [20] 保存函数调用时的s4寄存器
    reg_t s5;  // [21] 保存函数调用时的s5寄存器
    reg_t s6;  // [22] 保存函数调用时的s6寄存器
    reg_t s7;  // [23] 保存函数调用时的s7寄存器
    reg_t s8;  // [24] 保存函数调用时的s8寄存器
    reg_t s9;  // [25] 保存函数调用时的s9寄存器
    reg_t s10; // [26] 保存函数调用时的s10寄存器
    reg_t s11; // [27] 保存函数调用时的s11寄存器
    reg_t t3;  // [28] 临时寄存器 03
    reg_t t4;  // [29] 临时寄存器 04
    reg_t t5;  // [30] 临时寄存器 05
    reg_t t6;  // [31] 临时寄存器 06

    reg_t pc;  // 保存下一个调度周期要运行的程序计数器（PC）

    uint8_t flags;
};


typedef struct task_context* task_context_ptr;


#endif // _TASK_H
