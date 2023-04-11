#include <onix/debug.h>
#include <onix/stdarg.h>
#include <onix/vprintf.h>
#include <onix/printk.h>

static char buf[1024];

void debugk(char *file, int line, const char *fmt, ...){
    // 先格式化自定义的一些文本信息
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);

    printk("[%s] [%d] %s \n", file, line, buf);
}