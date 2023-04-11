#ifndef ONIX_TYPES_H
#define ONIX_TYPES_H

typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;
typedef i32 isize;
typedef u32 usize;


#define bool _Bool
#define true 1
#define false 0


#define EOF -1
#define EOS '\0'

#define nullptr ((void*) 0)

#define _packed __attribute__((packed))  // 用于定义特殊的结构体, 使用该属性可以使得变量或者结构体成员使用最小的对齐方式
                                         // 即对变量是一字节对齐, 对域(field)是位对齐

// 用于省略函数的栈帧
#define _ofp __attribute__((optimize("omit-frame-pointer")))





#endif
