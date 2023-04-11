#ifndef ONIX_STRING_H
#define ONIX_STRING_H

#include <onix/types.h>

// 字符串 src copy 到 dest
strcpy(u8* dest, const u8* src);
// 字符串 src copy count 个字符 到 dest
strncpy(u8* dest, const u8* src, usize count);
// 字符串 dest += src
strcat(u8* dest, const u8* src);
// 统计字符串长度
usize strlen(const u8* str);
// 比较字符的大小, 返回值 -1/0/1   ("acd", "abd") 返回 -1, 因为 acd < abd
i8 strcmp(const u8* lhs, const u8* rhs);
// 左边第一个 指定字符串中找都指定字符所在的指针地址, 如果没有找到, 返回 nullptr
u8* strchr_l(const u8* str, u8 ch);
// 右边第一个 指定字符串中找都指定字符所在的指针地址, 如果没有找到, 返回 nullptr
u8* strchr_r(const u8* str, u8 ch);
// u8* strsep(const u8* str);
// u8* strrsep(const u8* str);

// 比较指定字节内存
i8 memcmp(const u8* lhs, const u8* rhs, usize count);
// 将指定区域赋值为 ch
memset(u8* src, u8 ch, usize count);
// 从src copy指定字节的数据 到dest
memcpy(u8* dest, const u8* src, usize count);
// 从内存总找到, 第一个 ch字符 从左到右
u8* memchr(const u8* src, u8 ch, usize count);



#endif



