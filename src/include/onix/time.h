#ifndef ONIX_TIME_H
#define ONIX_TIME_H
#include <onix/types.h>
#include <onix/debug.h>

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

#define MINUTE 60          // 每分钟的秒数
#define HOUR (60 * MINUTE) // 每小时的秒数
#define DAY (24 * HOUR)    // 每天的秒数
#define YEAR (365 * DAY)   // 每年的秒数，以 365 天算


typedef struct tm{
    u32 tm_sec;   // 秒数 [0，59]
    u32 tm_min;   // 分钟数 [0，59]
    u32 tm_hour;  // 小时数 [0，59]
    u32 tm_mday;  // 1 个月的天数 [0，31]
    u32 tm_mon;   // 1 年中月份 [0，11]
    u32 tm_year;  // 从 1900 年开始的年数
    u32 tm_wday;  // 1 星期中的某天 [0，6] (星期天 =0)
    u32 tm_yday;  // 1 年中的某天 [0，365]
    u32 tm_isdst; // 夏令时标志
} tm;

void time_read_bcd(tm *time);
void time_read(tm *time);

// 将struct tm 结构体类型表示的时间转换为从 1970年1月1日00:00:00 +0000(UTC) 到该时间点的秒数
usize mktime(tm *time);

void time_init();


#endif