//
// Created by katherinesolar on 25-5-24.
//

#ifndef TYPEDEFS_H
#define TYPEDEFS_H
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef signed char        int8_t;
typedef signed short       int16_t;
typedef signed int         int32_t;
typedef signed long long   int64_t;
typedef unsigned long long uintptr_t;
/*/
 * Register Width
/*/
typedef uint32_t reg_t;
typedef uint32_t ptr_t;


typedef struct timer* timer_ptr; // 计时器指针类型
#define TRUE 1
#define FALSE 0
#define NULL ((void*)0)
#endif // TYPEDEFS_H
