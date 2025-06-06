
# 源文件定义
# 汇编文件（.S）和 C 文件（.c）分别存储在不同的变量中

SRCS_ASM += ./src/start.S
# SRCS_ASM += ./src/memory/memory.S
# SRCS_ASM += ./src/trap/entry.S


SRCS_C += ./src/kernel.c

SRCS_C += ./src/io/uart.c
# SRCS_C += ./src/io/printf.c
# SRCS_C += ./src/io/plic.c
# SRCS_C += ./src/io/trap_handler.c

# SRCS_C += ./src/tasks/count.c
# SRCS_C += ./src/tasks/tasks.c
# SRCS_C += ./src/tasks/timer.c

# SRCS_C += ./src/memory/page.c

# SRCS_C += ./src/user.c

# SRCS_C += ./src/task/sched.c
# SRCS_C += ./src/task/task.c
# SRCS_C += ./src/task/trap_handler.c

# SRCS_C += ./src/trap/trap.c
# SRCS_C += ./src/trap/trap_handler.c

# SRCS_C += ./src/timer/timer.c
# SRCS_C += ./src/timer/trap_handler.c

# SRCS_C += ./src/lock.c


INCLUDE_DIRS += ./include/


# 支持“是否启用系统调用”这一功能的可配置编译
# 之后可以继续追加宏定义
# DEFS += -DCONFIG_SYSCALL


# 分别定义交叉编译器、二进制拷贝工具、反汇编工具
CROSS_COMPILE = riscv64-unknown-elf-
CC            = ${CROSS_COMPILE}gcc
OBJCOPY       = ${CROSS_COMPILE}objcopy
OBJDUMP       = ${CROSS_COMPILE}objdump
# 编译器选项
# | 参数           | 作用                                                |
# | -------------- | --------------------------------------------------- |
# | `-nostdlib`    | 不链接 C 标准库（裸机开发必须）                     |
# | `-fno-builtin` | 禁用 GCC 内建函数优化（例如 `memcpy`, `printf` 等） |
# | `-g`           | 生成调试信息                                        |
# | `-Wall`        | 打开所有警告信息（推荐）                            |
# | `-march=rv32g` | 指定 RISC-V 架构为 RV32G（通用指令集）              |
# | `-mabi=ilp32e` | 指定 ABI 为 ILP32E（整数、长整型和指针为 32 位）    |
# | `-I./include`  | 添加头文件搜索路径（包含目录）                      |
CFLAGS += -nostdlib
CFLAGS += -fno-builtin
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -march=rv32g
CFLAGS += -mabi=ilp32e
CFLAGS += $(addprefix -I, ${INCLUDE_DIRS})


# QEMU 运行参数
# | 参数            | 说明                                 |
# | --------------- | ------------------------------------ |
# | `-nographic`    | 不启用图形界面，使用终端             |
# | `-smp 1`        | 单核                                 |
# | `-machine virt` | 使用 `virt` 机器模型（虚拟硬件平台） |
# | `-bios none`    | 不使用 BIOS ，直接加载内核 ELF 文件   |
QEMU    = qemu-system-riscv32
QFLAGS += -nographic
QFLAGS += -smp 1
QFLAGS += -machine virt
QFLAGS += -bios none


# 多架构 GDB，用于调试非本地架构（如 RISC-V）
GDB = gdb-multiarch
GDBINIT = ./gdbinit

# 是否使用链接脚本（默认是 true）
USE_LINKER_SCRIPT ?= true
LDFILE = ./os.ld

# 定义创建目录和删除文件的命令（用于兼容性和可维护性）
MKDIR = mkdir -p
RM = rm -rf

# 所有编译生成的文件都会放到 build/ 目录下
OUTPUT_PATH = ./build/
ELF = ${OUTPUT_PATH}/os.elf
BIN = ${OUTPUT_PATH}/os.bin

# 最终目标文件（汇编 + C 文件）合并为一个变量 OBJS，稍后用于链接整个程序
OBJS_ASM := $(addprefix ${OUTPUT_PATH}/, $(patsubst %.S, %.o, ${SRCS_ASM}))
OBJS_C   := $(addprefix $(OUTPUT_PATH)/, $(patsubst %.c, %.o, ${SRCS_C}))
OBJS = ${OBJS_ASM} ${OBJS_C}


# 链接脚本
# ?= 是 Make 的条件赋值语法，表示如果 USE_LINKER_SCRIPT 没有被外部环境或命令行定义，则默认为 true
# 默认使用链接脚本进行链接
ifeq (${USE_LINKER_SCRIPT}, true)
    # 表示链接时使用这个生成的 linker script 来安排内存布局（非常关键，比如 .text, .data, .bss 等段放在哪些地址）
    LDFLAGS = -T ${OUTPUT_PATH}/os.ld.generated
else
    # 直接强行把代码段 .text 放到地址 0x8000_0000 起始
    LDFLAGS = -Ttext=0x80000000
endif


# Makefile 的默认目标
# 当只输入 make 时，等同于 make all
# all 依赖于两个其他目标：${OUTPUT_PATH} 和 ${ELF}
# 这意味着，在构建 all 目标之前，make 会先确保 ${OUTPUT_PATH} 和 ${ELF} 这两个目标已经被正确生成
# 如果它们还没有生成，make 会自动查找并执行相应的规则来创建它们。
.DEFAULT_GOAL := all
all: ${OUTPUT_PATH} ${ELF}

# 创建输出目录
# 确保输出目录存在，如果不存在则创建
# @ 表示不在终端打印命令本身，只执行
${OUTPUT_PATH}:
	@${MKDIR} ${OUTPUT_PATH}


# 构建最终可执行文件（ELF）
# build/os.elf 依赖于所有 .o 目标文件（C 和汇编），变量 ${OBJS} 包含这些目标文件路径
${ELF}: ${OBJS}

    # 如果启用链接脚本（默认是 true）
    # 用 GCC 先预处理 ./os.ld，生成实际使用的 build/os.ld.generated
    # -E：只运行预处理器，不编译。
    # -P：不输出 #line 行号信息。
    # -x c：把 .ld 文件当作 C 源代码进行预处理，这样可以使用 #define 等预处理语法。
    # ${DEFS} 是一些宏定义（如 -DCONFIG_SYSCALL），会参与预处理。
ifeq (${USE_LINKER_SCRIPT}, true)
	${CC} -E -P -x c ${DEFS} ${CFLAGS} ${LDFILE} > ${OUTPUT_PATH}/os.ld.generated
endif

    # 调用链接器（通过 GCC 调用）来生成 ELF 可执行文件
    # 把 ELF 格式的可执行文件转为裸二进制 os.bin，用于加载到内存运行
	${CC} ${CFLAGS} ${LDFLAGS} -o ${ELF} ${OBJS}
	${OBJCOPY} -O binary ${ELF} ${BIN}

# 编译 C 文件的规则
# 只要有一个 .o 文件依赖于同名 .c 文件，就用这条规则编译
# 匹配规则：把每个 .c 文件编译为对应的 .o 文件。
# mkdir -p $(dir $@)：确保 .o 文件输出的目录存在。
# $<：表示第一个依赖（也就是 .c 文件）
# $@：表示目标文件（即 .o 文件）
${OUTPUT_PATH}/%.o : %.c
	@mkdir -p $(dir $@)
	${CC} ${DEFS} ${CFLAGS} -c -o $@ $<

# 编译汇编文件的规则
# 同样逻辑，用于把 .S 汇编文件编译为 .o 文件
${OUTPUT_PATH}/%.o : %.S
	@mkdir -p $(dir $@)
	${CC} ${DEFS} ${CFLAGS} -c -o $@ $<


run: all
	@${QEMU} -M ? | grep virt >/dev/null || exit
	@echo "Press Ctrl-A and then X to exit QEMU"
	@echo "------------------------------------"
	@${QEMU} ${QFLAGS} -kernel ${ELF}


.PHONY : debug
debug: all
	@echo "Press Ctrl-C and then input 'quit' to exit GDB and QEMU"
	@echo "-------------------------------------------------------"
	@${QEMU} ${QFLAGS} -kernel ${ELF} -s -S &
	@${GDB} ${ELF} -q -x ${GDBINIT}


.PHONY : code
code: all
	@${OBJDUMP} -S ${ELF} | less > ./ignore/code.txt


.PHONY : clean
clean:
	@${RM} ${OUTPUT_PATH}

