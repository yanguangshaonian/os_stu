#include <onix/onix.h>




void kernel_init(){
    // 初始化控制台
    console_init();

    // 初始化全局描述符
    gdt_init();
    
    // 初始化中断
    interrupt_init();

    // 初始化任务
    // task_init();

    clock_init();

    asm volatile("sti\n");
    hang();

}


