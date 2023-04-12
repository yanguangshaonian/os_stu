
#ifndef ONIX_CLOCK_H
#define ONIX_CLOCK_H


#define PIT_CHAN0_REG 0X40  // 设置计数器 0
#define PIT_CHAN2_REG 0X42  // 设置计数器 2  我们用不到
#define PIT_CTRL_REG 0X43  // 控制字, 端口号是0x43, 8位寄存器, 控制字寄存器也是模式控制寄存器, 用于指定计数器的工作方式, 读写格式, 以及数字制式


#define HITS 100  // 我们设置的 每秒钟想要发生中断的次数, 当然也不能太低或者太高, 因为要在计数器寄存器(16)位存放
#define OSCILLATOR 1193182  // 每秒钟时钟震荡次数
#define CLOCK_COUNTER (OSCILLATOR / HITS)  // 如果想每秒钟 发生 HITS 次中断, 需要给计数器设置为 总震荡次数/我们想要发生的次数 得到每次发生中断需要计算的计数器
#define HIT_GAP (1000 / HITS)  // 1秒有1000ms, 算出来每次 中断的间隔时间, 也就是 每个时间片 大概10ms


void clock_init();

#endif