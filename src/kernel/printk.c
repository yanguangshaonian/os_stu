
#include <onix/console.h>
#include <onix/stdarg.h>
#include <onix/vprintf.h>

static u8* buf[1024];

u32 printk(const u8 *fmt, ...){
    
    va_list args;
    u32 i;

    va_start(args, fmt);
    
    i = vsprintf(buf, fmt, args);
    va_end(args);
    
    asm volatile("cli\n");
    console_write(buf, i);
    asm volatile("sti\n");
    return i;
}