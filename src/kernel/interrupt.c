#include <onix/interrupt.h>
#include <onix/onix.h>
#include <onix/global.h>  // // 全局描述符表 指针

// 全局中断向量表 一共IDT_SIZE个中断符号
gate_t idt[IDT_SIZE];
pointer_t idt_ptr;

// 这个会被handle.asm导入, 里面放的是我们定义的中断处理函数
u8* handler_table[IDT_SIZE];

// 从handle.asm 导入一共 ENTRY_SIZE个 存放了统一的中断描述符处理函数的地址
// 为了让我们从c语言加载 idt表, 中断时 会先进入这个指针所在函数内, 然后继续跳入到 handler_table中
extern handler_entry_table[ENTRY_SIZE];

// 异常 中断处理函数默认函数
void exception_handle(u8 handle_num){
    char *msg;
    if (handle_num < 22) {
        msg = messages[handle_num];
    } else{
        msg = messages[15];
    }
    printk("Exception as [0x%02X] %s\n", handle_num, msg);
    hang();
}

// 通知中断控制器，中断处理结束
void send_eoi(u8 handle_num)
{
    if (handle_num >= 0x20 && handle_num < 0x28)
    {
        out_8(PIC_M_CTRL, PIC_EOI);
    }
    if (handle_num >= 0x28 && handle_num < 0x30)
    {
        out_8(PIC_M_CTRL, PIC_EOI);
        out_8(PIC_S_CTRL, PIC_EOI);
    }
}

u32 counter = 0;
// 外中断 中断处理函数默认函数
void default_handler(u8 handle_num)
{
    send_eoi(handle_num);
    counter += 1;
    DEBUGK("[%x] default interrupt called... %d", handle_num, counter);
}


void idt_init(){
    for (usize i = 0; i < ENTRY_SIZE; i++){
        // 得到中断处理函数
        void* handle = handler_entry_table[i];

        idt[i].offset0 = (usize)handle;
        idt[i].offset1 = ((usize)handle) >> 16;
        idt[i].selector = CODE_SELECT;  // 代码段
        idt[i].reserved = 0;            // 保留不用
        idt[i].type = 0b1110;           // 任务门, 中断门, 陷阱门,   这里使用中断门, 他比陷阱门多了一个eflag, 任务门最复杂性能也不咋地
        idt[i].segment = 0;             // 系统级别的中断
        idt[i].DPL = 0;                 // 内核态权限才能调用
        idt[i].present = 1;             // 在内存中是有效的
    }

    // 加载idt表
    idt_ptr.base = (usize)idt;
    idt_ptr.limit = sizeof(idt)-1;
    asm volatile("lidt idt_ptr\n");

    // 修改 handler_table, 使其调用int中断执行handle.asm统一处理函数之后, 执行我们自己的这里的c处理程序
    // 异常处理函数设置
    for (usize i = 0; i < 0x20; i++){
        handler_table[i] = exception_handle;
    }

    // 外中断处理函数设置
    for (usize i = 0x20; i < ENTRY_SIZE; i++){
        handler_table[i] = default_handler;
    }
}

// 初始化中断控制器
void pic_init(){
    out_8(PIC_M_CTRL, 0b00010001); // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    out_8(PIC_M_DATA, 0x20);       // ICW2: 起始中断向量号 0x20
    out_8(PIC_M_DATA, 0b00000100); // ICW3: IR2接从片.
    out_8(PIC_M_DATA, 0b00000001); // ICW4: 8086模式, 正常EOI

    out_8(PIC_S_CTRL, 0b00010001); // ICW1: 边沿触发, 级联 8259, 需要ICW4.
    out_8(PIC_S_DATA, 0x28);       // ICW2: 起始中断向量号 0x28
    out_8(PIC_S_DATA, 2);          // ICW3: 设置从片连接到主片的 IR2 引脚
    out_8(PIC_S_DATA, 0b00000001); // ICW4: 8086模式, 正常EOI

    out_8(PIC_M_DATA, 0b11111110); // 只打开主片的第0个中断也就是 的主片的0x20时钟中断
    out_8(PIC_S_DATA, 0b11111111); // 关闭从片所有pic中断
}


// 初始化保护模式的中断向量表
void interrupt_init(){
    pic_init();
    idt_init();
}


