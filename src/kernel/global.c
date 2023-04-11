#include <onix/debug.h>
#include <onix/string.h>
#include <onix/global.h>


descriptor_t gdt[GDT_SIZE];  // 内核全部的全局描述符表
pointer_t gdt_ptr;           // 内核全局描述符表指针

// 初始化全局描述符表 为我们 c语言 的数组
// 把loader中的全局描述符表 copy 到内核中
void gdt_init(){
    // 把loader内的的全局描述符表指针加载出来
    asm volatile("sgdt gdt_ptr");
    // 把全部的全局描述符表 copy出来
    memcpy(&gdt, (u8*)gdt_ptr.base, gdt_ptr.limit + 1);

    // 
    gdt_ptr.base = (usize)&gdt;
    gdt_ptr.limit = sizeof(gdt) - 1;

    // 把内核的gdt表加载进去
    asm volatile("lgdt gdt_ptr");
}