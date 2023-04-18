#include <onix/time.h>




// 每个月开始时的已经过去天数
static u32 month[13] = {
    0, // 这里占位，没有 0 月，从 1 月开始
    0,
    (31),
    (31 + 29),
    (31 + 29 + 31),
    (31 + 29 + 31 + 30),
    (31 + 29 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31),
    (31 + 29 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30)};

// 开机启动的时间点
u32 startup_time;

u32 century;

u32 elapsed_leap_years(u32 year)
{
    u32 result = 0;
    result += (year - 1) / 4;
    result -= (year - 1) / 100;
    result += (year + 299) / 400;
    result -= (1970 - 1900) / 4;
    return result;
}

bool is_leap_year(u32 year)
{
    return ((year % 4 == 0) && (year % 100 != 0)) || ((year + 1900) % 400 == 0);
}

void localtime(u32 stamp, tm *time)
{
    time->tm_sec = stamp % 60;

    u32 remain = stamp / 60;

    time->tm_min = remain % 60;
    remain /= 60;

    time->tm_hour = remain % 24;
    u32 days = remain / 24;

    time->tm_wday = (days + 4) % 7; // 1970-01-01 是星期四

    // 这里产生误差显然需要 365 个闰年，不管了
    u32 years = days / 365 + 70;
    time->tm_year = years;
    u32 offset = 1;
    if (is_leap_year(years))
        offset = 0;

    days -= elapsed_leap_years(years);
    time->tm_yday = days % (366 - offset);

    u32 mon = 1;
    for (; mon < 13; mon++)
    {
        if ((month[mon] - offset) > time->tm_yday)
            break;
    }

    time->tm_mon = mon - 1;
    time->tm_mday = time->tm_yday - month[time->tm_mon] + offset + 1;
}

// 这里生成的时间可能和 UTC 时间有出入
// 与系统具体时区相关，不过也不要紧，顶多差几个小时
// 这段代码的作用是将 struct tm 结构体类型表示的时间转换为从 1970 年 1 月 1 日 00:00:00 +0000 (UTC) 到该时间点的秒数
u32 mktime(tm *time)
{
    u32 res;
    u32 year; // 1970 年开始的年数
    // 下面从 1900 年开始的年数计算
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    // 这些年经过的秒数时间
    res = YEAR * year;

    // 已经过去的闰年，每个加 1 天
    res += DAY * ((year + 1) / 4);

    // 已经过完的月份的时间
    res += month[time->tm_mon] * DAY;

    // 如果 2 月已经过了，并且当前不是闰年，那么减去一天
    if (time->tm_mon > 2 && ((year + 2) % 4))
        res -= DAY;

    // 这个月已经过去的天
    res += DAY * (time->tm_mday - 1);

    // 今天过去的小时
    res += HOUR * time->tm_hour;

    // 这个小时过去的分钟
    res += MINUTE * time->tm_min;

    // 这个分钟过去的秒
    res += time->tm_sec;

    return res;
}

u32 get_yday(tm *time)
{
    u32 res = month[time->tm_mon]; // 已经过去的月的天数
    res += time->tm_mday;          // 这个月过去的天数

    u32 year;
    if (time->tm_year >= 70)
        year = time->tm_year - 70;
    else
        year = time->tm_year - 70 + 100;

    // 如果不是闰年，并且 2 月已经过去了，则减去一天
    // 注：1972 年是闰年，这样算不太精确，忽略了 100 年的平年
    if ((year + 2) % 4 && time->tm_mon > 2)
    {
        res -= 1;
    }

    return res;
}

// 从cmos芯片中读取到时间
u8 cmos_read(u8 addr)
 {
     out_8(CMOS_ADDR, CMOS_NMI | addr);
     return in_8(CMOS_DATA);
 };

void time_read_bcd(tm *time)
{
    // CMOS 的访问速度很慢。为了减小时间误差，在读取了下面循环中所有数值后，
    // 若此时 CMOS 中秒值发生了变化，那么就重新读取所有值。
    // 这样内核就能把与 CMOS 的时间误差控制在 1 秒之内。
    do
    {
        time->tm_sec = cmos_read(CMOS_SECOND);
        time->tm_min = cmos_read(CMOS_MINUTE);
        time->tm_hour = cmos_read(CMOS_HOUR);
        time->tm_wday = cmos_read(CMOS_WEEKDAY);
        time->tm_mday = cmos_read(CMOS_DAY);
        time->tm_mon = cmos_read(CMOS_MONTH);
        time->tm_year = cmos_read(CMOS_YEAR);
        century = cmos_read(CMOS_CENTURY);
    } while (time->tm_sec != cmos_read(CMOS_SECOND));
}

void time_read(tm *time)
{
    time_read_bcd(time);
    time->tm_sec = bcd_to_bin(time->tm_sec);
    time->tm_min = bcd_to_bin(time->tm_min);
    time->tm_hour = bcd_to_bin(time->tm_hour);
    time->tm_wday = bcd_to_bin(time->tm_wday);
    time->tm_mday = bcd_to_bin(time->tm_mday);
    time->tm_mon = bcd_to_bin(time->tm_mon);
    time->tm_year = bcd_to_bin(time->tm_year);
    time->tm_yday = get_yday(time);
    time->tm_isdst = -1;
    century = bcd_to_bin(century);
}

void time_init()
{
    tm time;
    // 读取时间
    time_read(&time);
    DEBUGK("startup time: %d%d-%02d-%02d %02d:%02d:%02d\n",
         century,
         time.tm_year,
         time.tm_mon,
         time.tm_mday,
         time.tm_hour,
         time.tm_min,
         time.tm_sec);

    // 计算得到时间戳
    startup_time = mktime(&time);
    DEBUGK("时间戳 time: %d", startup_time);

}