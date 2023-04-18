#ifndef ONIX_MEMORY_H
#define ONIX_MEMORY_H

#include <onix/types.h>
#include <onix/onix.h>
#include <onix/printk.h>
#include <onix/assert.h>


#define PAGE_SIZE 0x1000     // 一页的大小 4K
#define MEMORY_BASE 0x100000 // 1M，可用内存开始的位置

#define ZONE_VALID 1  // 可用内存区域
#define ZONE_RESERVED 2  // 不可用内存区域
#define IDX(addr) (((usize)addr) >> 12)  // 获得 addr 的页索引, 每页是4k大小, 2的12次方 是4k, 地址直接除以4096就得到了地址所在页的索引


void memory_init(u32 magic, u32* addr,  u8* mem_struct_buffer_addr);

typedef struct mem_struct
{
    u64 base; // 内存基地址
    u64 size; // 内存长度
    u32 type; // 类型
} _packed mem_struct;

#endif