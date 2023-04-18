#ifndef ONIX_H
#define ONIX_H
#include <onix/types.h>
#include <onix/io.h>
#include <onix/string.h>
#include <onix/console.h>
#include <onix/assert.h>
#include <onix/printk.h>
#include <onix/debug.h>
#include <onix/global.h>
#include <onix/task.h>>
#include <onix/interrupt.h>>
#include <onix/stdlib.h>>
#include <onix/clock.h>>
#include <onix/time.h>>
#include <onix/rtc.h>>
#include <onix/memory.h>>


void kernel_init();

#define ONIX_MAGIC 0x20220205  // 内核魔数, 用于校验错误

#endif