
// memory.S

#define SIZE_PTR .word

.section .rodata

// 堆内存起始地址
.global HEAP_START
HEAP_START: SIZE_PTR _heap_start

// 堆内存大小
.global HEAP_SIZE
HEAP_SIZE: SIZE_PTR _heap_size

// .text 段起始地址
.global TEXT_START
TEXT_START: SIZE_PTR _text_start

// .text 段结束地址
.global TEXT_END
TEXT_END: SIZE_PTR _text_end

// .data 段起始地址
.global DATA_START
DATA_START: SIZE_PTR _data_start

// .data 段结束地址
.global DATA_END
DATA_END: SIZE_PTR _data_end

// .rodata 段起始地址
.global RODATA_START
RODATA_START: SIZE_PTR _rodata_start

// .rodata 段结束地址
.global RODATA_END
RODATA_END: SIZE_PTR _rodata_end

// .bss 段起始地址
.global BSS_START
BSS_START: SIZE_PTR _bss_start

// .bss 段结束地址
.global BSS_END
BSS_END: SIZE_PTR _bss_end
