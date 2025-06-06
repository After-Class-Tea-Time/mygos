
// start.S

#include "platform.h"


.global _start              // 系统入口
.text
_start:
    // park harts with id != 0
    csrr    t0, mhartid     // read current hart id
    mv      tp, t0          // keep CPU's hartid in its tp for later usage.
    bnez    t0, park        // if we're not on the hart 0 we park the hart

    // 将 BSS 段清零。
    la      a0, _bss_start
    la      a1, _bss_end
    bgeu    a0, a1, 2f      // 如果 a0 >= a1，说明 BSS 段已经清零完成
1:
    sw      zero, (a0)      // 把 a0 指向的 4 字节内容清零。
    addi    a0, a0, 4       // 指向下一个 4 字节的内存地址。
    bltu    a0, a1, 1b      // 如果 a0 小于 a1，则跳转回标签 1，继续清零下一个 4 字节的内存地址。

2:
    // 设置栈空间，栈 **从高地址向低地址增长** ，所以把 sp 指向栈空间的末尾
    slli    t0, t0, 10              // shift left the hart id by 1024
    la      sp, stacks + STACK_SIZE // 将初始堆栈指针设置为第一个堆栈空间的末尾
    add     sp, sp, t0              // 根据 hart id 偏移到对应的 hart 的堆栈空间

    /*/
     * 在 start_kernel 结束时，schedule() 将调用 MRET 切换到第一个任务，因此在这里准备 mstatus 。
     * 注意：最好不要假设 mstatus 的初始值为零。
     * 把 mstatus.MPP 设置为 3，以便在 MRET 后仍然以机器模式运行。
     * 把 mstatus.MPIE 设置为 1，以便 MRET 将启用中断。
    /*/
	li	t0, 3 << 11 | 1 << 7
	csrs	mstatus, t0

    j       start_kernel            // hart 0 jump to c

park:
    wfi
    j       park

// In the standard RISC-V calling convention, the stack pointer sp is always 16-byte aligned.
// 在标准 RISC-V 调用约定中，堆栈指针 sp 始终是 16 字节对齐的。
.balign 16
stacks:
.skip CPU_STACK_SIZE * MAXNUM_CPU       // allocate space for all the harts stacks // 为所有 hart 分配堆栈空间

.end                                // End of file
