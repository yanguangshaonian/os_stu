#ifndef ONIX_TASK_H
#define ONIX_TASK_H

#include <onix/types.h>

#define PAGE_SIZE 0x1000  // 每页的内存大小 4k

typedef u32 target_t();  // 函数的入口地址


typedef struct task_t{
    u8* stack;  // 内核线程的栈顶
} task_t;

// ABI 需要保存的寄存器(字段顺序 不要变!)
typedef struct task_frame_t{
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    void (*eip)(void);  // 一个函数指针
} task_frame_t;

// 初始化任务的函数
void task_init();

#endif