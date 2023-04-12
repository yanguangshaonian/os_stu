#include <onix/interrupt.h>
#include <onix/clock.h>
#include <onix/assert.h>
#include <onix/io.h>


// 时钟中断的处理函数, 等下就替换掉默认的外中断处理函数
void clock_handler(u8 handle_num){
    assert(handle_num == 0x20);  // 只允许时钟中断调用这个
    send_eoi(handle_num);  // 通知中断控制器, cpu正在处理这个中断了
}

void pit_init(){
    // 配置计数器 0 时钟
    out_8(PIT_CTRL_REG, 0b00110100);  // 第0号计数器, 计数器先读写低字节, 再读写高字节, 模式2, 使用二进制编码的计数器
    out_8(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);  // 先设置低字节
    out_8(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);  // 再设置高字节
}

void clock_init(){
    pit_init();
    // 设置中断处理函数
    set_interrupt_handler(IRQ_CLOCK, clock_handler);
    // 打开时钟中断
    set_interrupt_mask(IRQ_CLOCK, true);
}

