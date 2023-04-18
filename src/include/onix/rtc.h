#ifndef ONIX_RTC_H
#define ONIX_RTC_H
#include <onix/types.h>
#include <onix/debug.h>
#include <onix/assert.h>
#include <onix/time.h>
#include <onix/interrupt.h>
#include <onix/printk.h>


#define CMOS_ADDR 0x70 // CMOS 地址寄存器
#define CMOS_DATA 0x71 // CMOS 数据寄存器


// 下面是 CMOS 信息的寄存器索引
#define CMOS_SECOND 0x00  // (0 ~ 59)
#define CMOS_MINUTE 0x02  // (0 ~ 59)
#define CMOS_HOUR 0x04    // (0 ~ 23)
#define CMOS_WEEKDAY 0x06 // (1 ~ 7) 星期天 = 1，星期六 = 7
#define CMOS_DAY 0x07     // (1 ~ 31)
#define CMOS_MONTH 0x08   // (1 ~ 12)
#define CMOS_YEAR 0x09    // (0 ~ 99)
#define CMOS_CENTURY 0x32 // 可能不存在
#define CMOS_NMI 0x80

#define CMOS_A 0x0a
#define CMOS_B 0x0b
#define CMOS_C 0x0c
#define CMOS_D 0x0d




u8 cmos_read(u8 addr);
void cmos_write(u8 addr, u8 value);

void rtc_init();

void set_alarm(u32 secs);


#endif