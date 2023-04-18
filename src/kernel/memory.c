
#include <onix/memory.h>

static usize memory_base = 0; // 可用内存基地址，应该等于 1M
static usize memory_size = 0; // 可用内存大小
static usize total_pages = 0; // 所有内存页数
static usize free_pages = 0;  // 空闲内存页数

// 找到一块最大的物理页, 且是0x10000为初始位置的
void memory_init(u32 magic, u32* mem_struct_count_addr, u8* mem_struct_buffer_addr){
    u32 mem_struct_count = *mem_struct_count_addr;

    if(ONIX_MAGIC != magic) {
        panic("memory magic error");
    }

    for (usize i = 0; i < mem_struct_count; i++){
        mem_struct* ms = (mem_struct*)(mem_struct_buffer_addr + sizeof(mem_struct) * i);
        printk("Memory base 0x%p size 0x%p type %d\n", (u32)ms->base, (u32)ms->size, (u32)ms->type);

        // 找到最大的一块内存
        if (ms->type == ZONE_VALID && ms->size > memory_size) {
            memory_base = (u32)ms->base;
            memory_size = (u32)ms->size;
        }
    }

    assert(memory_base == MEMORY_BASE); // 内存开始的位置为 1M
    assert((memory_size & 0xfff) == 0); // 要求按页对齐

    free_pages =  IDX(memory_size);
    total_pages = free_pages + IDX(memory_base);
    

    printk("mem_struct_count %d\n", mem_struct_count);
    printk("Memory base 0x%p\n", (u32)memory_base);
    printk("Memory size 0x%p\n", (u32)memory_size);
    printk("free_pages %d\n", free_pages);
    printk("total_pages %d\n", total_pages);
    

}