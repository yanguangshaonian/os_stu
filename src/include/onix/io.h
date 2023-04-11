#ifndef ONIX_IO_H
#define ONIX_IO_H

#include <onix/types.h>

// 这个文件只是定义了头文件, 真正实现的是 io.asm 

extern u8 in_8(u16 port);  // 从端口读出一个字节
extern u16 in_16(u16 port);  // 从端口读出一个字


extern void out_8(u16 port, u8 value);  // 写入到 端口内 一个字节
extern void out_16(u16 port, u8 value);  // 写入到 端口内 一个字


#endif


// #define CRT_ADDR_PORT 0x3d4  // 位置寄存器 port
// #define CRT_DATA_PORT 0x3d5  // 数据寄存器 port

// #define CRT_CURSOR_H_VALUE 0xe  // 光标高位 value
// #define CRT_CURSOR_L_VALUE 0xf  // 光标低位 value
    // {
    //     // 读取光标位置的 高8位
    //     out_8(CRT_ADDR_PORT, CRT_CURSOR_H_VALUE);
    //     u8 pos_h = in_8(CRT_DATA_PORT);

    //     // 读取光标位置的 低8位
    //     out_8(CRT_ADDR_PORT, CRT_CURSOR_L_VALUE);
    //     u8 pos_l = in_8(CRT_DATA_PORT);
        
    //     // 获得光标位置
    //     u16 pos = (pos_h << 8) | pos_l;
    // }


    // {
    //     // 设置光标位置到666处
    //     u16 pos = 444;

    //     // 设置高地址
    //     out_8(CRT_ADDR_PORT, CRT_CURSOR_H_VALUE);
    //     u8 pos_h = pos >> 8;
    //     out_8(CRT_DATA_PORT, pos_h);

    //     // 设置低地址
    //     u8 pos_l = pos & 0xff;
    //     out_8(CRT_ADDR_PORT, CRT_CURSOR_L_VALUE);
    //     out_8(CRT_DATA_PORT, pos_l);
    // }