
#include <onix/memory.h>

static usize memory_base = 0; // 可用内存基地址，这个在调用完memory_init之后应该是 0x100000
static usize memory_size = 0; // 可用内存大小
static usize total_pages = 0; // 所有内存页数
static usize free_pages = 0;  // 空闲内存页数

// 找到一块最大的物理页, 且是0x100000为初始位置的
void memory_init(u32 magic, u32* mem_struct_count_addr, u8* mem_struct_buffer_addr){
    printk("------ memory_init ------\n");
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

static u32 start_page = 0;   // 可分配物理内存起始位置, 应该在管理页的后面
static u8 *memory_map;       // 物理内存数组
static u32 memory_map_pages; // 物理内存数组占用的页数


// 初始化用来管理物理内存的 "管理者"数组
// 我们用 1位 表示1页是否被占用, 用来当做管理全部的页
// 这个在可用内存的最开头的位置
void memory_map_init(){
    printk("------ memory_map_init ------\n");
    // 初始化管理物理内存的数组, 我们用内存起始位置的开头, 来当做 "管理者"
    memory_map = (u8*)memory_base;

    // 计算管理者 占用的页数, (总页数 / 页大小   得到的就是管理 "总页数" 需要多少个 基础的"页" 保存每个页是否被占用)
    memory_map_pages = div_round_up(total_pages, PAGE_SIZE);
    printk("memory_map_pages: %d\n", memory_map_pages);

    // 这些页已经被"管理者" 占用了, 需要去掉
    free_pages -= memory_map_pages;

    // 初始化 "管理者"
    memset(memory_map, 0, PAGE_SIZE * memory_map_pages);

    // 我们可用的只有0x100000之后的物理内存 且0x100000之后的前n页, 被我们的"管理者"所占用
    // 这里设置 已经被占用的页的状态
    start_page = IDX(MEMORY_BASE) + memory_map_pages;
    for (usize i = 0; i < start_page; i++){
        memory_map[i] = 1;
    }
    printk("free_pages %d\n", free_pages);
    printk("total_pages %d\n", total_pages);
}


// 获得一页空闲的内存
static u32 get_page(){
    for (usize i = start_page; i < total_pages; i++){
        if (memory_map[i] == 0) {
            memory_map[i] = 1;
            // 不应该存在空闲页, 但是free_pages为0的情况
            if (free_pages == 0) {
                panic("free page size error");
            }
            // 得到页的初始位置
            usize page = ((usize)i) << 12;
            printk("get page is os: 0x%p\n", page);
            free_pages -= 1;
            return page;
        }
    }

    // 同样的 也没有空闲页了
    panic("Out of Memory");

}

// 释放一页空闲的内存
static void put_page(u32 addr){
    // 禁止非页的起始位置传入进来
    assert((addr & 0xfff) == 0);

    usize idx = IDX(addr);
    // 禁止idx小于start_page的页的起始地址传入进来, 禁止超过最大页
    assert(idx >= start_page && idx < total_pages);

    // 保证最少一个引用,这个也
    assert(memory_map[idx] >= 1);

    memory_map[idx] -= 1;
    
    // 如果减去一之后被引用的数量为0, 说明这页内存没人使用, 空闲页+1
    if (memory_map[idx] == 0) {
        free_pages += 1;
    }

    // 保证free_page > 0
    assert(free_pages > 0 && free_pages < total_pages);
    printk("put page is os: 0x%p\n", addr);
}

void memory_test(){
    usize size = 5;
    usize pages[size];
    for (usize i = 0; i < size; i++){
        pages[i] = get_page();
    }

    for (usize i = 0; i < size; i++){
        put_page(pages[i]);
    }
}