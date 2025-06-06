# My Great Operating System

### 简称 MyGOS

---

### 项目目录

```plaintext
.
├── src/
│   ├── start.S
│   ├── mem.S
│   ├── entry.S
│   ├── kernel.c
│   ├── io/
│   ├── tasks/
│   └── ...
├── include/
│   └── *.h
├── build/
│   └── (中间文件与输出文件)
├── os.ld         # 链接脚本
├── gdbinit       # 调试脚本
└── Makefile

```

### platform.h

## MemoryMap

see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[]

| 地址范围     | 设备        | 描述                                |
| ------------ | ----------- | ----------------------------------- |
| `0x00001000` | boot ROM    | QEMU 提供的启动 ROM                 |
| `0x02000000` | CLINT       | Core Local Interrupt Controller     |
| `0x0C000000` | PLIC        | Platform Level Interrupt Controller |
| `0x10000000` | UART0       | 串口 UART（输出调试信息）           |
| `0x10001000` | virtio disk | 虚拟磁盘                            |
| `0x80000000` | DRAM base   | 内核加载的起始地址                  |

```plaintext

ram (从 0x80000000 开始)
│
├─ .text       → 存放程序代码
│
├─ .rodata     → 只读数据，如字符串常量等
│
├─ .data       → 已初始化的全局/静态变量
│
├─ .bss        → 未初始化的全局/静态变量（在运行时清零）
│
├─ heap        → 堆，从 _bss_end 到内存结尾 (_memory_end)
│
└──────────────► 内存结束地址：0x80000000 + 128MB = 0x88000000

```

---

### 缩写速查

| 缩写  | 英文全称                                    | 中文全称             | 功能描述                                                            |
| ----- | ------------------------------------------- | -------------------- | ------------------------------------------------------------------- |
| CLINT | Core Local Interruptor                      | 核心本地中断控制器   | 管理每个 hart（核心）本地的软件和定时器中断。                       |
| PLIC  | Platform-Level Interrupt Controller         | 平台级中断控制器     | 接收外部设备的中断并分发给各个 hart。                               |
| UART  | Universal Asynchronous Receiver-Transmitter | 通用异步收发器       | 用于串口通信，提供终端输入输出接口。                                |
| IRQ   | Interrupt Request                           | 中断请求             | 外设向处理器请求服务的信号。                                        |
| WARL  | Write-Any Read-Legal                        | 可写任意、读合法     | 某些 CSR 类型，写入任意值但读取会被硬件过滤为合法值。               |
| QEMU  | Quick Emulator                              | 快速模拟器           | 支持模拟多种 CPU 架构的虚拟机，常用于操作系统实验开发。             |
| HART  | Hardware Thread                             | 硬件线程（处理器核） | 表示一个逻辑 CPU 核心，在 RISC-V 中 hart 是并行执行单元的基本单位。 |
| GPR   | General Purpose Register                    | 通用寄存器           | 用于存储临时数据和计算结果的寄存器。                                |
| CSR   | Control and Status Register                 | 控制与状态寄存器     | 用于控制和监视 CPU 状态的特殊寄存器。                               |
| MMIO  | Memory-Mapped I/O                           | 内存映射 I/O         | 将外设寄存器映射到内存地址空间，允许通过内存访问外设。              |
| ABI   | Application Binary Interface                | 应用程序二进制接口   | 定义程序与操作系统之间的接口规范，包括系统调用约定等。              |
| BIOS  | Basic Input/Output System                   | 基本输入输出系统     | 启动计算机时加载操作系统的固件，通常在虚拟机中不使用。              |
| BSS   | Block Started by Symbol                     | 符号开始的块         | 用于存储未初始化的全局变量和静态变量，运行时会被清零。              |

---

### 中断相关 CSR 位缩写

| 缩写 | 英文全称                             | 中文全称               | 功能描述                                               | 位置 |
| ---- | ------------------------------------ | ---------------------- | ------------------------------------------------------ | ---- |
| MSIP | Machine Software Interrupt Pending   | 机器软件中断挂起位     | 指示软件中断是否挂起，由 CLINT 控制并映射至 `mip` 中。 |
| MSIE | Machine Software Interrupt Enable    | 软件中断使能           | `mie` 中的一位，控制是否响应软件中断。                 |
| MTIE | Machine Timer Interrupt Enable       | 定时器中断使能         | `mie` 中的一位，控制是否响应定时器中断。               |
| MEIE | Machine External Interrupt Enable    | 外部中断使能           | `mie` 中的一位，控制是否响应外部中断（如 UART 中断）。 |
| MIE  | Machine Interrupt Enable             | 机器中断总使能         | `mstatus` 中的一位，启用机器模式下的全局中断响应能力。 |
| SIE  | Supervisor Interrupt Enable          | 监督模式中断总使能     | `mstatus` 中的一位，启用 S 模式中断。                  |
| UIE  | User Interrupt Enable                | 用户模式中断总使能     | `mstatus` 中的一位，启用 U 模式中断。                  |
| MPIE | Machine Previous Interrupt Enable    | 上次中断使能状态（M）  | 中断时自动保存先前的中断使能状态。                     |
| SPIE | Supervisor Previous Interrupt Enable | 上次中断使能状态（S）  | 同上，适用于 S 模式。                                  |
| UPIE | User Previous Interrupt Enable       | 上次中断使能状态（U）  | 同上，适用于 U 模式。                                  |
| MPP  | Machine Previous Privilege           | 上次特权级（M 模式中） | 表示陷入中断前的特权级。                               |
| SPP  | Supervisor Previous Privilege        | 上次特权级（S 模式中） | 表示陷入中断前的特权级（仅两级特权时使用）。           |

---

### RISC-V 通用寄存器（GPR）速查表

| 编号 | 缩写  | ABI 名称                  | 中文名称              | 功能描述                                                      |
| ---- | ----- | ------------------------- | --------------------- | ------------------------------------------------------------- |
| x0   | zero  | zero                      | 常数 0                | 只读寄存器，始终为零，写入无效。                              |
| x1   | ra    | return address            | 返回地址寄存器        | 用于函数调用返回地址（`jal` 等跳转指令保存处）。              |
| x2   | sp    | stack pointer             | 栈指针                | 指向当前栈顶，函数调用、局部变量分配等操作依赖此寄存器。      |
| x3   | gp    | global pointer            | 全局指针              | 一般由编译器使用，指向全局数据段基址（RV GCC ABI）。          |
| x4   | tp    | thread pointer            | 线程指针              | 多线程环境下指向当前线程控制块，或 hart ID。                  |
| x5   | t0    | temporary 0               | 临时寄存器 0          | 用作中间运算，不需在函数调用间保留值。                        |
| x6   | t1    | temporary 1               | 临时寄存器 1          | 同上。                                                        |
| x7   | t2    | temporary 2               | 临时寄存器 2          | 同上。                                                        |
| x8   | s0/fp | saved 0/frame pointer     | 保存寄存器 0 / 帧指针 | 函数调用中保存值；当作帧指针（frame pointer）时用于栈帧管理。 |
| x9   | s1    | saved 1                   | 保存寄存器 1          | 函数调用中必须保留的值（callee-saved）。                      |
| x10  | a0    | argument 0 / return value | 参数 0 / 返回值       | 用于函数参数或返回值（前 2 个参数/返回值寄存器）。            |
| x11  | a1    | argument 1 / return value | 参数 1 / 返回值       | 同上。                                                        |
| x12  | a2    | argument 2                | 参数 2                | 函数参数传递使用（总共最多支持 8 个参数）。                   |
| x13  | a3    | argument 3                | 参数 3                | 同上。                                                        |
| x14  | a4    | argument 4                | 参数 4                | 同上。                                                        |
| x15  | a5    | argument 5                | 参数 5                | 同上。                                                        |
| x16  | a6    | argument 6                | 参数 6                | 同上。                                                        |
| x17  | a7    | argument 7                | 参数 7                | 同上。                                                        |
| x18  | s2    | saved 2                   | 保存寄存器 2          | Callee-saved 寄存器，函数调用必须保存和恢复。                 |
| x19  | s3    | saved 3                   | 保存寄存器 3          | 同上。                                                        |
| x20  | s4    | saved 4                   | 保存寄存器 4          | 同上。                                                        |
| x21  | s5    | saved 5                   | 保存寄存器 5          | 同上。                                                        |
| x22  | s6    | saved 6                   | 保存寄存器 6          | 同上。                                                        |
| x23  | s7    | saved 7                   | 保存寄存器 7          | 同上。                                                        |
| x24  | s8    | saved 8                   | 保存寄存器 8          | 同上。                                                        |
| x25  | s9    | saved 9                   | 保存寄存器 9          | 同上。                                                        |
| x26  | s10   | saved 10                  | 保存寄存器 10         | 同上。                                                        |
| x27  | s11   | saved 11                  | 保存寄存器 11         | 同上。                                                        |
| x28  | t3    | temporary 3               | 临时寄存器 3          | 用于中间结果，caller-saved。                                  |
| x29  | t4    | temporary 4               | 临时寄存器 4          | 同上。                                                        |
| x30  | t5    | temporary 5               | 临时寄存器 5          | 同上。                                                        |
| x31  | t6    | temporary 6               | 临时寄存器 6          | 同上。                                                        |

---

### RISC-V 特殊寄存器缩写全称表

| 缩写     | 类型 | 英文全称                          | 中文全称             | 功能描述                                                 |
| -------- | ---- | --------------------------------- | -------------------- | -------------------------------------------------------- |
| mhartid  | CSR  | Machine Hardware Thread ID        | 硬件线程编号         | 当前 hart（核心）的 ID，用于多核系统区分处理器。         |
| mepc     | CSR  | Machine Exception Program Counter | 异常返回地址寄存器   | 保存异常/中断发生前的 PC 值。                            |
| mscratch | CSR  | Machine Scratch Register          | 机器暂存寄存器       | 由操作系统使用，在中断处理期间保存临时数据（如上下文）。 |
| mtvec    | CSR  | Machine Trap-Vector Base Address  | 陷阱向量表地址寄存器 | 设置 trap（中断/异常）处理函数入口地址。                 |
| mstatus  | CSR  | Machine Status Register           | 机器状态寄存器       | 包含中断使能、特权级等重要状态位。                       |
| mie      | CSR  | Machine Interrupt Enable          | 中断使能寄存器       | 各类中断源的使能控制位。                                 |
| mcause   | CSR  | Machine Cause Register            | 中断/异常原因寄存器  | 指明 trap 原因，是中断还是异常及具体类型。               |
| mip      | CSR  | Machine Interrupt Pending         | 中断挂起寄存器       | 显示哪些中断正在等待处理中。                             |
| mtimecmp | MMIO | Machine Timer Compare Register    | 定时器比较寄存器     | 当 `mtime >= mtimecmp` 时触发定时器中断，由 CLINT 提供。 |
| mtime    | MMIO | Machine Timer Register            | 机器时间寄存器       | 记录自系统启动以来的计时器周期数，由 CLINT 提供。        |

---

### RISC-V 架构手册中定义的标准编码（常见的 M 模式 Machine Mode 原因）：

| `mcause` 值（十进制） | 中断/异常 | 宏定义（常见写法）             | 原因说明                         |
| --------------------- | --------- | ------------------------------ | -------------------------------- |
| 0                     | 异常      | `InstructionAddressMisaligned` | 指令地址未对齐                   |
| 1                     | 异常      | `InstructionAccessFault`       | 指令访问故障（如取指页面不存在） |
| 2                     | 异常      | `IllegalInstruction`           | 非法指令                         |
| 3                     | 异常      | `Breakpoint`                   | 断点（调试）                     |
| 4                     | 异常      | `LoadAddressMisaligned`        | 读地址未对齐                     |
| 5                     | 异常      | `LoadAccessFault`              | 加载访问故障                     |
| 6                     | 异常      | `StoreAddressMisaligned`       | 写地址未对齐                     |
| 7                     | 异常      | `StoreAccessFault`             | 写访问故障                       |
| 8                     | 异常      | `EnvironmentCallFromUMode`     | U 模式发出的系统调用（ecall）    |
| 9                     | 异常      | `EnvironmentCallFromSMode`     | S 模式发出的系统调用（ecall）    |
| 11                    | 异常      | `EnvironmentCallFromMMode`     | M 模式发出的系统调用（ecall）    |
| 12                    | 异常      | `InstructionPageFault`         | 指令页错误（Sv32）               |
| 13                    | 异常      | `LoadPageFault`                | 加载页错误                       |
| 15                    | 异常      | `StorePageFault`               | 存储页错误                       |
| 0x80000000 + 3        | 中断      | `MachineSoftwareInterrupt`     | 软件中断（由 MSIP 触发）         |
| 0x80000000 + 7        | 中断      | `MachineTimerInterrupt`        | 定时器中断（mtimecmp）           |
| 0x80000000 + 11       | 中断      | `MachineExternalInterrupt`     | 外部中断（PLIC）                 |

---

## 内存空间 MemoryMap

| 地址范围     | 设备        | 描述                                |
| ------------ | ----------- | ----------------------------------- |
| `0x00001000` | boot ROM    | QEMU 提供的启动 ROM                 |
| `0x02000000` | CLINT       | Core Local Interrupt Controller     |
| `0x0C000000` | PLIC        | Platform Level Interrupt Controller |
| `0x10000000` | UART0       | 串口 UART（输出调试信息）           |
| `0x10001000` | virtio disk | 虚拟磁盘                            |
| `0x80000000` | DRAM base   | 内核加载的起始地址                  |

see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[]

---

## 关于多任务

1. 协作式多任务 (Cooperative Multitasking)：

   协作式环境下，下一个任务被调度的前提是当前任务
   主动放弃处理器。

2. 抢占式多任务 (Preemptive Multitasking)：

   抢占式环境下，操作系统完全决定任务调度方案，操
   作系统可以剥夺当前任务对处理器的使用，将处理
   器提供给其它任务。
