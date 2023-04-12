#include <onix/task.h>
#include <onix/debug.h>
#include <onix/printk.h>

extern void task_switch(task_t* next);

task_t* task_a_stack = (task_t*) (PAGE_SIZE * 1);  // A线程栈开始的位置
task_t* task_b_stack = (task_t*) (PAGE_SIZE * 2);  // B线程栈开始的位置



static void task_create(task_t* task_struct, usize target_handle){
    usize stack = (usize)task_struct + PAGE_SIZE;  // 得到线程栈的栈底
    stack -= sizeof(task_frame_t);  // 线程本身的信息留出来空间
    task_frame_t* frame = (task_frame_t*) stack;  //  上面留出来的空间, 创建frame

    frame->edi = 0x11111111;
    frame->esi = 0x22222222;
    frame->ebx = 0x33333333;
    frame->ebp = 0x44444444;
    frame->eip = (u8*) target_handle;  // ip指向指定的 函数

    task_struct->stack = (u8*)stack;  // 把减去了 frame大小的栈的栈底, 赋值给 task结构 的首地址, 后面会被 schdule 间接取址
    int a = 123;
}

task_t* running_task(){
    asm volatile(
        "movl %esp, %eax\n"   // 当前 栈顶
        "andl $0xfffff000, %eax\n"  // 得到栈底; 得到当前线程开始的位置, 即task_create中创建的 frame, 
                                  // 由于我们每个任务 占用 1个页, 1个页的大小是0x1000, 所以我们每个线程开始的地方
                                  // 刚好是 0x1000的整倍数, 直接把低3位置位0 即可得到每个任务开始的地方也就是保存了task_frame_t和后续栈的地方
    );

}

void _ofp schdule(){
    task_t* current = running_task();
    // 这里实验的, 如果当前是任务a 就切换到任务b, 如果是任务b 就切换到任务a
    task_t* next;
    if (current == task_a_stack) {
        next = task_b_stack;
    } else {
        next = task_a_stack;
    }
    task_switch(next);  // 调用汇编实现的切换函数
}


void _ofp thread_a(){
    asm volatile("sti\n");
    int c = 0;
    while(true){
        printk("A~");
    }
}
void _ofp thread_b(){
    // 因为是在中断内进行的调度, 而进入中断 if标志位又被置为0, 所以由中断函数进入到当前任务中时, 需要把任务的eflag置为1(打开中断)
    // , 这样中断在调用完中断处理函数之后, 会pop保存的寄存器状态, 我们这个函数一进来就把 if标志位置 设置为1了, 也就跟着恢复了
    asm volatile("sti\n");
    int c = 0;
    while(true){
        printk("B~");
    }
}


void task_init(){
    test();

}


void test(){
    task_create(task_a_stack, thread_a);
    task_create(task_b_stack, thread_b);
    schdule();
}

/*
线程栈的内存分布



|        |        |              |
| ------ | ------ | ------------ |
| eip    | 0x1fff | Function_ptr |
| ebp    |        | 0x44444444   |
| ebx    |        | 0x33333333   |
| esi    |        | 0x22222222   |
| edi    |        | 0x11111111   |
| 栈底    | 0x1f00 |              |
|        |        | ...          |
|        |        | ...          |
| 栈顶    | 0x1100 |              |
|        |        |              |
| ...    |        |              |
| 保存栈顶 | 0x1000 | 0x1100       |

*/


