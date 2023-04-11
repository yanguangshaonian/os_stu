#include <onix/console.h>

static u8 char_attr = 7;                 // 字符默认样式
static u16 space = 0x0720;               // 带有样式的空格
struct CURSOR_REL {u8 x; u8 y} ;         // 光标距离 当前屏幕的 x和y 单位为字符


// 得到当前显示器这一屏的开始的内存位置
static usize get_current_screen_mem_status(){
    // 首先 获得当前显示器 距离 0xb800 多少个字符
    // 高八位
    out_8(CRT_ADDR_PORT, CRT_MEM_H_VALUE);
    u8 h = in_8(CRT_DATA_PORT);

    // 低8位
    out_8(CRT_ADDR_PORT, CRT_MEM_L_VALUE);
    u8 l = in_8(CRT_DATA_PORT);

    u16 char_pos = (h << 8) | l;
    
    // 累加 得到当前显示器的内存地址
    usize mem_pos = char_pos << 1;  // 一个字符 2字节
    return CRT_MEM_START + mem_pos;
}

// 设置显示器当前屏需要展示的字符的内存位置 为新的一屏
static set_current_screen_mem_status(usize screen){

    // 得到距离 crt开始的内存的距离
    usize screen_rel = screen-CRT_MEM_START;

    // 高八位字符设置
    out_8(CRT_ADDR_PORT, CRT_MEM_H_VALUE);
    out_8(CRT_DATA_PORT, (screen_rel >> 1) >> 8);

    // 低8位
    out_8(CRT_ADDR_PORT, CRT_MEM_L_VALUE);
    out_8(CRT_DATA_PORT, (screen_rel >> 1));
}

// 得到当前光标在内存中的位置
static usize get_current_cursor_mem_status(){
    // 首先 获得当前当前光标 距离 0xb800 多少个字符
    // 高八位
    out_8(CRT_ADDR_PORT, CRT_CURSOR_H_VALUE);
    u8 h = in_8(CRT_DATA_PORT);

    // 低8位
    out_8(CRT_ADDR_PORT, CRT_CURSOR_L_VALUE);
    u8 l = in_8(CRT_DATA_PORT);

    u16 char_pos = (h << 8) | l;
    
    // 累加 得到当前光标的内存地址
    usize mem_pos = char_pos << 1;  // 一个字符 2字节
    return CRT_MEM_START + mem_pos;
}

// 设置光标的新的内存位置
static set_current_cursor_mem_status(usize cursor){
    // 得到距离 crt开始的内存的距离
    usize cursor_rel = cursor-CRT_MEM_START;

    // 高八位字符设置
    out_8(CRT_ADDR_PORT, CRT_CURSOR_H_VALUE);
    out_8(CRT_DATA_PORT, (cursor_rel >> 1) >> 8);

    // 低8位
    out_8(CRT_ADDR_PORT, CRT_CURSOR_L_VALUE);
    out_8(CRT_DATA_PORT, (cursor_rel >> 1));
}

// 得当当前距离屏幕的相对坐标
static struct CURSOR_REL get_current_cursor_x_and_y(){
    // 获得当前 光标内存位置
    usize cursor_mem_abs = get_current_cursor_mem_status();
    // 获得当前显示器内存位置
    usize screen_mem_abs = get_current_screen_mem_status();

    // 当前光标距离当前屏幕首地址几个字符
    usize char_count = (cursor_mem_abs - screen_mem_abs) >> 1;

    u8 x = char_count % WIDTH;
    u8 y = char_count / WIDTH;
    struct CURSOR_REL tmp = {x,y};
    return tmp;
}

// 向上滚动 count行
static scroll_up(usize* cursor_mem_abs_ptr){
    usize count_bytes = (1 * 2 * 80 * 1);

    // 当前屏幕的内存位置
    usize screen_mem_abs = get_current_screen_mem_status();
    
    // 新的内存屏幕的位置
    usize new_screen_mem_abs_start = screen_mem_abs + count_bytes;

    // 显存检测, 如果写入超过显存
    if (*cursor_mem_abs_ptr >= CRT_MEM_END) {

        // 把当前屏幕数据copy到 起始 位置
        memcpy((u8*)CRT_MEM_START, (u8*)screen_mem_abs, (1 * 2 * WIDTH * HEIGHT));

        // 清空现在当前屏幕, 内存之下所有的数据(!有问题, 用下面的循环)
        // memset((u8*)CRT_MEM_START+(1 * 2 * WIDTH * HEIGHT), 0, CRT_MEM_END-(CRT_MEM_START+(1 * 2 * WIDTH * HEIGHT)));

        // 结束位置
        u8* e = (u8*)CRT_MEM_START +  (1 * 2 * WIDTH * HEIGHT);
        while (true){
            if (e >= CRT_MEM_END) {
                break;
            }
            *e = space;
            e += 2;
        }
        
        
        // 设置 new_screen_mem_abs_start 为 新地址
        new_screen_mem_abs_start = CRT_MEM_START + count_bytes;

        // 光标也挪过去(cursor_mem_abs_ptr 是指针类型指针)
        (*cursor_mem_abs_ptr) -= (screen_mem_abs - CRT_MEM_START);
    }
    
    // 设置屏幕
    set_current_screen_mem_status(new_screen_mem_abs_start);
}


console_write(u8* buf, usize count){
    // 当前光标内存位置
    u8* cursor_mem_abs_ptr = (u8*)get_current_cursor_mem_status();

    // 当前屏幕的内存位置
    u8* screen_mem_abs_ptr = (u8*)get_current_screen_mem_status();

    // 当前光标相对于屏幕坐标系
    struct CURSOR_REL cursor_rel_xy = get_current_cursor_x_and_y();

    for (usize i = 0; i < count; i++){
        
        u8 ch = buf[i];
        if (ch == '~'){
            int f = 123;
        }

        switch (ch){
            case EOS:
                break;
            case BEL:
                break;
            case BS:
                // 如果光标内存位置==屏幕内存位置, 说明在开头
                if (cursor_mem_abs_ptr == screen_mem_abs_ptr) {
                    break;
                }

                // 前一个字符的 内存位置
                cursor_mem_abs_ptr -= 2;
                // 修改为空字符
                *cursor_mem_abs_ptr = space;
                break;
            case HT:
                break;
            case LF:
                // 光标换行
                cursor_mem_abs_ptr += (WIDTH << 1);
                cursor_rel_xy.y += 1;
                // 换行是否需要滚动
                if(cursor_rel_xy.y >= HEIGHT) {
                    scroll_up(&cursor_mem_abs_ptr);
                    cursor_rel_xy.y -= 1;
                }
                goto in_cr;
                break;
            case VT:
                break;
            case FF:
                goto in_cr;
                break;
            case CR:
                // 使光标回到开始的位置
                in_cr:
                    cursor_mem_abs_ptr -= (cursor_rel_xy.x << 1);
                    cursor_rel_xy.x = 0;
                    break;
            case DEL:
                // 直接替换当前字符为空白
                *cursor_mem_abs_ptr = space;
                break;
            default:
                cursor_rel_xy.x += 1;
                
                // 如果需要换行
                if(cursor_rel_xy.x >= WIDTH) {
                    cursor_rel_xy.x =0;
                    cursor_rel_xy.y += 1;

                    // 即将在下一行写入字符, 要保证下一行在 最大高度HEIGHT 之内
                    // cursor_rel_xy 内保存的是idx, 需要+1得到当前屏幕的行数 和HEIGHT 比较如果相等, 说明当前屏幕已经写满了
                    if(cursor_rel_xy.y >= HEIGHT) {
                        scroll_up(&cursor_mem_abs_ptr);
                        cursor_rel_xy.y -= 1;
                    }
                }

                // 替换字符和样式
                *cursor_mem_abs_ptr = ch;

                cursor_mem_abs_ptr += 1;
                *cursor_mem_abs_ptr = char_attr;
                cursor_mem_abs_ptr += 1;
                
                break;
        }
    }

    // // 如果最后一个字符显示完毕后刚好是屏幕的最后一个字符
    // if(cursor_rel_xy.x >= WIDTH) {
    //     cursor_rel_xy.x =0;
    //     cursor_rel_xy.y += 1;

    //     // 即将在下一行写入字符, 要保证下一行在 最大高度HEIGHT 之内
    //     // cursor_rel_xy 内保存的是idx, 需要+1得到当前屏幕的行数 和HEIGHT 比较如果相等, 说明当前屏幕已经写满了
    //     if(cursor_rel_xy.y >= HEIGHT) {
    //         scroll_up(&cursor_mem_abs_ptr);
    //         cursor_rel_xy.y -= 1;
    //     }
    // }

    // 重新设置光标位置
    set_current_cursor_mem_status((usize)cursor_mem_abs_ptr);
}

console_clear(){
    // 整个显存内存区域改成 有样式的空格
    usize crt_mem_start = CRT_MEM_START;
    u8* crt_mem_start_ptr = (u8*)crt_mem_start;
    for (usize i = 0; i < CRT_MEM_SIZE; i++){
        *crt_mem_start_ptr = space;
        crt_mem_start_ptr += 2;
    }

    // 光标重置到当前屏幕的第一个字符
    set_current_cursor_mem_status(get_current_screen_mem_status());
}

console_init(){
    // test_console();
    console_clear();
}


test_console(){
    struct CURSOR_REL cursor_rel_xy = get_current_cursor_x_and_y();
    int a = 0;

    // 当前位置
    usize screen_mem_abs = get_current_screen_mem_status();

    // 设置从第二行开始显示 (2*80)表示一行的字节数量
    usize screen = CRT_MEM_START + (2*80)*1;
    set_current_screen_mem_status(screen);
    // 当前位置
    screen_mem_abs = get_current_screen_mem_status();

    // 当前光标位置
    usize cursor_mem_abs = get_current_cursor_mem_status();
    // 设置新的光位置视在 第一行的 下标为3的字符, 当然, 这是不可见的, 因为 上面第一行已经不显示了
    cursor_mem_abs =  CRT_MEM_START + (1 * 2) * 3;
    set_current_cursor_mem_status(cursor_mem_abs);

    // 设置光标在第二行 下标为4的 字符处(这里屏幕中的第一行, 就是内存中的第二行)
    cursor_mem_abs = CRT_MEM_START + (1 * 2) * 80 + (1 * 2) * 4;
    set_current_cursor_mem_status(cursor_mem_abs);

    // 得到当前光标的相对位置
    struct CURSOR_REL cur = get_current_cursor_x_and_y();



    console_clear();
    char* message = "123456781234567812345672812345678123456781234567812345678123456781234567812345678\n";
    for (usize i = 0; i < 49; i++){
        console_write(message, strlen(message));
    }
    char* message1= "123456\n";
    console_write(message1, strlen(message1));
    console_write(message1, strlen(message1));
    console_write(message1, strlen(message1));

    cursor_rel_xy = get_current_cursor_x_and_y();
    a = 0;

    console_write(message1, strlen(message1));

    cursor_rel_xy = get_current_cursor_x_and_y();
    a = 0;

    console_write(message1, strlen(message1));
    console_write(message1, strlen(message1));
    console_write(message1, strlen(message1));
    console_write(message1, strlen(message1));
    console_write(message1, strlen(message1));

    char* message2= "111~\n";
    console_write(message2, strlen(message2));



    // screen_mem_abs = get_current_screen_mem_status();
    // cursor_rel_xy = get_current_cursor_x_and_y();
    

    // usize tmp = (6 * 2) + (24 * 80 * 2);
    // set_current_cursor_mem_status(screen_mem_abs + tmp);

    // char* message2= "1234567\n";
    // console_write(message2, strlen(message2));
    // console_write(message2, strlen(message2));
    // console_write(message2, strlen(message2));
    // console_write(message2, strlen(message2));
    // console_write(message2, strlen(message2));
    // console_write(message2, strlen(message2));
}