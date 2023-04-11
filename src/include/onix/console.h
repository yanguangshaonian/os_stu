#ifndef ONIX_CONSOLE_H
#define ONIX_CONSOLE_H

#include <onix/types.h>
#include <onix/io.h>
#include <onix/string.h>



#define CRT_ADDR_PORT 0x3d4  // crt位置寄存器 port
#define CRT_DATA_PORT 0x3d5  // crt数据寄存器 port

#define CRT_CURSOR_H_VALUE 0xe  // 当前光标所在字符距离0xb800 高位 value
#define CRT_CURSOR_L_VALUE 0xf  // 当前光标所在字符距离0xb800 低位 value

#define CRT_MEM_H_VALUE 0xC // 当前屏幕字符起始位置距离0xb800 - 高位
#define CRT_MEM_L_VALUE 0xD // 当前屏幕显示字符起始位置距离0xb800 - 低位

#define CRT_MEM_START 0xB8000                // 显卡内存起始位置
#define CRT_MEM_SIZE 0x4000                  // 显卡内存大小
#define CRT_MEM_END (CRT_MEM_START + CRT_MEM_SIZE)   // 显卡内存结束位置

#define WIDTH 80                      // 屏幕文本列数
#define HEIGHT 25                     // 屏幕文本行数

#define ROW_SIZE (WIDTH * 2)          // 每行字节数
#define SCR_SIZE (ROW_SIZE * HEIGHT)  // 一个屏幕容纳的字节数


// 特殊字符
#define NUL 0x00
#define ENQ 0x05
#define ESC 0x1B // ESC
#define BEL 0x07 // \a
#define BS 0x08  // \b  退格键
#define HT 0x09  // \t
#define LF 0x0A  // \n  换行
#define VT 0x0B  // \v
#define FF 0x0C  // \f
#define CR 0x0D  // \r  回到开头的位置
#define DEL 0x7F  // 删除当前位置的字符, 但是不退格


console_init();
console_clear();
console_write(u8* buf, usize count);

#endif