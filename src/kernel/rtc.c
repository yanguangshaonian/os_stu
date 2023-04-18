#include <onix/rtc.h>

// 从cmos芯片中读取到时间
u8 cmos_read(u8 addr){
     out_8(CMOS_ADDR, CMOS_NMI | addr);
     return in_8(CMOS_DATA);
}


// 写 cmos 寄存器的值
void cmos_write(u8 addr, u8 value)
{
    out_8(CMOS_ADDR, CMOS_NMI | addr);
    out_8(CMOS_DATA, value);
}

int counter = 0;
static u32 next_time = 0xffffffff;
// 实时时钟中断处理函数
void rtc_handler(int handle_num)
{
    // 实时时钟中断向量号
    assert(handle_num == 0x28);

    // 向中断控制器发送中断处理完成的信号
    send_eoi(handle_num);

    // 读 CMOS 寄存器 C，允许 CMOS 继续产生中断
    cmos_read(CMOS_C);

    tm time;
    time_read(&time);
    u32 now_time = mktime(&time);
    if (next_time <= mktime(&time)) {
        DEBUGK("rtc handler %d... %d  %d", counter++, now_time, next_time);
        next_time = 0xffffffff;
    }
}


// 设置 secs 秒后发生实时时钟中断
void set_alarm(u32 secs)
{
    tm time;
    time_read(&time);

    u8 sec = secs % 60;
    secs /= 60;
    u8 min = secs % 60;
    secs /= 60;
    u32 hour = secs;

    time.tm_sec += sec;
    if (time.tm_sec >= 60)
    {
        time.tm_sec %= 60;
        time.tm_min += 1;
    }

    time.tm_min += min;
    if (time.tm_min >= 60)
    {
        time.tm_min %= 60;
        time.tm_hour += 1;
    }

    time.tm_hour += hour;
    if (time.tm_hour >= 24)
    {
        time.tm_hour %= 24;
    }

    next_time = mktime(&time);

    printk("set alarm...\n");
    cmos_write(CMOS_B, 0b01000010);  // bocsh 中无法打开闹钟中断, 未知bug, 这里使用周期中断 实现的伪闹钟
    cmos_read(CMOS_C); // 读 C 寄存器，以允许 CMOS 中断
}



void rtc_init()
{
    // cmos_write(CMOS_B, 0b01000010); // 打开周期中断(当达到寄存器 A 中 RS 所设定的时间基准时,允许产生中断, 24小时制)
    // // cmos_write(CMOS_B, 0b00100010); // 打开闹钟中断
    // cmos_read(CMOS_C); // 读 C 寄存器，以允许 CMOS 中断

    // // 设置中断频率
    // out_8(CMOS_A, (in_8(CMOS_A) & 0xf) | 0b1110);  // 250 ms 触发一次中断

    // 设置自定义中断处理函数
    set_interrupt_handler(IRQ_RTC, rtc_handler);
    
    // 打开从片上的实时时钟中断控制器
    set_interrupt_mask(IRQ_RTC, true);
    // 从片和主片和这个接口相连接
    set_interrupt_mask(IRQ_CASCADE, true);
}
