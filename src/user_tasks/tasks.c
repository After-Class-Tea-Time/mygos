
// tasks.c

#include "mygos.h"
#include "utils.h"


void
user_task0()
{
    static int sum = 0;

    print_task_start(task_get_current_context_id(), (reg_t)user_task0, "Task 0");
    while(1)
    {
        printf("Task 0: sum = %d\n", sum);
        if(sum > 25)
        {
            uart_puts("Task 0: sum exceeded 25, reset to 0\n");
            sum = 0;
        }
        else sum++;

        task_delay(DELAY);
        // task_yield();
    }
}

void
user_task1()
{
    static int product = 1;

    print_task_start(task_get_current_context_id(), (reg_t)user_task1, "Task 1");
    while(1)
    {
        printf("Task 1: product = %d\n", product);
        if(product > 1000)
        {
            uart_puts("Task 1: product exceeded 1000, reset to 1\n");
            product = 1;
        }
        else product *= 2;

        task_delay(DELAY);
        // task_yield();
    }
}

#define USE_LOCK
void
user_task_lock()
{
    for(int i = 0; i < 5; i++)
    {
#ifdef USE_LOCK
        spin_lock();
#endif
        printf("Task Lock: 第%d次循环: \n", i);
        uart_puts("Task Lock: Begin ... \n");
        for(int i = 0; i < 15; i++)
        {
            uart_puts("Task Lock: Running... \n");
            task_delay(DELAY);
        }
        uart_puts("Task Lock: End ... \n");
#ifdef USE_LOCK
        spin_unlock();
#endif
    }
}


void
page_test()
{
    print_task_start(task_get_current_context_id(), (reg_t)page_test, "Page Test");

    void* p1 = page_alloc(2);
    printf("p1 = %p\n", p1);
    page_free(p1);

    void* p2 = page_alloc(7);
    printf("p2 = %p\n", p2);

    void* p3 = page_alloc(4);
    printf("p3 = %p\n", p3);
    page_free(p3);

    page_free(p2);

    printf("\n");

    print_task_end(task_get_current_context_id(), "Page Test");
}
