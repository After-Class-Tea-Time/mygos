
// timer.h

#ifndef _TIMER_H
#define _TIMER_H

#include "typedefs.h"


// 计时器
struct timer
{
    void (*func)(void);    // 回调函数
    void*    arg;          // 回调函数参数
    uint32_t timeout_tick; // 超时时间（以 tick 为单位）
};


// 时间间隔约等于 1 秒
#define TIMER_INTERVAL CLINT_TIMEBASE_FREQ

#define MAX_TIMER 10


void _timer_load(uint32_t interval); // 加载下一个定时器中断的时间间隔（以计时周期为单位）
void _timer_checkout();              // 检查定时器列表，调用超时的计时器回调函数，并删除它们


#endif // _TIMER_H
