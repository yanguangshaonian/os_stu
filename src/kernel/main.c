
#include <onix/onix.h>




void kernel_init(){
    // 初始化控制台
    console_init();

    // 初始化全局描述符
    gdt_init();
    
    // 初始化中断
    interrupt_init();

    // int cnt = 0;
    // while (true){
    //     cnt += 1;
    //     printk("%d", 123);
    // }

    // 初始化任务
    task_init();

}


