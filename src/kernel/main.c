
#include <onix/onix.h>




void kernel_init(){
    // 初始化控制台
    console_init();

    // 初始化全局描述符
    gdt_init();

    // 初始化任务
    // task_init();

    // 初始化中断
    interrupt_init();

    asm volatile(
        "sti\n"               // 打开cpu的中断中断
        "movl %eax, %eax\n"
    );

    // u32 counter = 0;
    // while(true){
    //     counter += 1;
    //     DEBUGK("looping in kernel init %d", counter);
    //     delay(10000000);
    // }

}


