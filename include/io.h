
// io.h

#ifndef _IO_H
#define _IO_H


#include "typedefs.h"


// Reference:
//[1]: TECHNICAL DATA ON 16550, http://byterunner.com/16550.html


/*/
 * The UART control registers are memory-mapped at address UART0.
 * This macro returns the address of one of the registers.
 *
 * UART控制寄存器的地址被分配在 UART0 处，
 * 这个宏据此计算某个寄存器的地址
 *
 * 通过 (volatile uint8_t*) 强制类型转换，把这个地址转换为指向 8 位无符号整型的易失性指针。
 * volatile 关键字告诉编译器，这个内存地址对应的值可能随时发生变化（比如被硬件修改），
 * 因此每次访问都必须直接从内存读取或写入，不能做优化缓存。
/*/
#define UART_REG(reg) ((volatile uint8_t*)(UART0 + reg))


/*/
 * UART control registers map. see [1] "PROGRAMMING TABLE"
 * note some are reused by multiple functions
 * 0 (write mode): THR/DLL
 * 1 (write mode): IER/DLM
 *
 * UART 寄存器映射（详见 [1] Programming Table）
 * 部分寄存器根据读/写模式用途不同而复用相同偏移地址。
/*/
#define RHR 0 // Receive Holding Register (read mode)       // 接收保持寄存器（读）
#define THR 0 // Transmit Holding Register (write mode)     // 发送保持寄存器（写）
#define DLL 0 // LSB of Divisor Latch (write mode)          // 除数锁存器低字节（写）
#define IER 1 // Interrupt Enable Register (write mode)     // 中断使能寄存器（写）
#define DLM 1 // MSB of Divisor Latch (write mode)          // 除数锁存器高字节（写）
#define FCR 2 // FIFO Control Register (write mode)         // FIFO 控制寄存器（写）
#define ISR 2 // Interrupt Status Register (read mode)      // 中断状态寄存器（读）
#define LCR 3 // Line Control Register                      // 行控制寄存器
#define MCR 4 // Modem Control Register                     // 调制解调控制寄存器
#define LSR 5 // Line Status Register                       // 行状态寄存器
#define MSR 6 // Modem Status Register                      // 调制解调状态寄存器
#define SPR 7 // ScratchPad Register                        // 暂存寄存器


/*/
 * POWER UP DEFAULTS
 * IER = 0: TX/RX holding register interrupts are both disabled
 * ISR = 1: no interrupt penting
 * LCR = 0
 * MCR = 0
 * LSR = 60 HEX
 * MSR = BITS 0-3 = 0, BITS 4-7 = inputs
 * FCR = 0
 * TX = High
 * OP1 = High
 * OP2 = High
 * RTS = High
 * DTR = High
 * RXRDY = High
 * TXRDY = Low
 * INT = Low
 *
 * 电源启动时默认值（Power-up Defaults）
 * 仅供参考
 * - IER = 0：发送/接收中断禁用
 * - ISR = 1：无中断挂起
 * - LSR = 0x60：发射器为空、可接受数据
 * - FCR = 0：FIFO 禁用
 * - 所有输出均为高电平（TX, RTS, DTR, OP1, OP2）
 * - INT 中断信号线为低电平
 *
/*/


/*/
 * LINE STATUS REGISTER (LSR)
 * LSR BIT 0:
 * 0 = no data in receive holding register or FIFO.
 * 1 = data has been receive and saved in the receive holding register or FIFO.
 * ......
 * LSR BIT 5:
 * 0 = transmit holding register is full. 16550 will not accept any data for transmission.
 * 1 = transmitter hold register (or FIFO) is empty. CPU can load the next character.
 * ......
/*/
#define LSR_RX_READY (1 << 0)                           // 接收数据准备好（有数据可读）
#define LSR_TX_IDLE (1 << 5)                            // 发送缓冲区空（可写）

#define uart_read_reg(reg) (*(UART_REG(reg)))           // 读UART寄存器
#define uart_write_reg(reg, v) (*(UART_REG(reg)) = (v)) // 写UART寄存器


void     plic_init();                     // 初始化 PLIC（外部中断控制器），设置 UART0 的中断优先级和使能
void     uart_init();                     // 初始化 UART，配置波特率和数据格式
uint32_t plic_claim();                    // 查询 PLIC，获取当前需要处理的中断源 ID（如果有）
void     plic_complete(uint32_t plic_id); // 完成中断处理，通知 PLIC 已经处理完指定的中断源 ID
char     uart_getc();                     // 接收一个字符（阻塞，直到接收缓冲区有数据）


#endif // _IO_H
