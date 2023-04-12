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



// #include <onix/console.h>
// #include <onix/io.h>
// #include <onix/string.h>

// #define CRT_ADDR_REG 0x3D4 // CRT(6845)索引寄存器
// #define CRT_DATA_REG 0x3D5 // CRT(6845)数据寄存器

// #define CRT_START_ADDR_H 0xC // 显示内存起始位置 - 高位
// #define CRT_START_ADDR_L 0xD // 显示内存起始位置 - 低位
// #define CRT_CURSOR_H 0xE     // 光标位置 - 高位
// #define CRT_CURSOR_L 0xF     // 光标位置 - 低位

// #define MEM_BASE 0xB8000              // 显卡内存起始位置
// #define MEM_SIZE 0x4000               // 显卡内存大小
// #define MEM_END (MEM_BASE + MEM_SIZE) // 显卡内存结束位置
// #define WIDTH 80                      // 屏幕文本列数
// #define HEIGHT 25                     // 屏幕文本行数
// #define ROW_SIZE (WIDTH * 2)          // 每行字节数
// #define SCR_SIZE (ROW_SIZE * HEIGHT)  // 屏幕字节数

// #define ASCII_NUL 0x00
// #define ASCII_ENQ 0x05
// #define ASCII_BEL 0x07 // \a
// #define ASCII_BS 0x08  // \b
// #define ASCII_HT 0x09  // \t
// #define ASCII_LF 0x0A  // \n
// #define ASCII_VT 0x0B  // \v
// #define ASCII_FF 0x0C  // \f
// #define ASCII_CR 0x0D  // \r
// #define ASCII_DEL 0x7F

// static u32 screen; // 当前显示器开始的内存位置

// static u32 pos; // 记录当前光标的内存位置

// static u32 x, y; // 当前光标的坐标

// static u8 attr = 7;        // 字符样式
// static u16 erase = 0x0720; // 空格

// // 获得当前显示器的开始位置
// static void get_screen()
// {
//     out_8(CRT_ADDR_REG, CRT_START_ADDR_H); // 开始位置高地址
//     screen = in_8(CRT_DATA_REG) << 8;      // 开始位置高八位
//     out_8(CRT_ADDR_REG, CRT_START_ADDR_L);
//     screen |= in_8(CRT_DATA_REG);

//     screen <<= 1; // screen *= 2
//     screen += MEM_BASE;
// }

// // 设置当前显示器开始的位置
// static void set_screen()
// {
//     out_8(CRT_ADDR_REG, CRT_START_ADDR_H); // 开始位置高地址
//     out_8(CRT_DATA_REG, ((screen - MEM_BASE) >> 9) & 0xff);
//     out_8(CRT_ADDR_REG, CRT_START_ADDR_L);
//     out_8(CRT_DATA_REG, ((screen - MEM_BASE) >> 1) & 0xff);
// }

// // 获得当前光标位置
// static void get_cursor()
// {
//     out_8(CRT_ADDR_REG, CRT_CURSOR_H); // 高地址
//     pos = in_8(CRT_DATA_REG) << 8;     // 高八位
//     out_8(CRT_ADDR_REG, CRT_CURSOR_L);
//     pos |= in_8(CRT_DATA_REG);

//     get_screen();

//     pos <<= 1; // pos *= 2
//     pos += MEM_BASE;

//     u32 delta = (pos - screen) >> 1;
//     x = delta % WIDTH;
//     y = delta / WIDTH;
// }

// static void set_cursor()
// {
//     out_8(CRT_ADDR_REG, CRT_CURSOR_H); // 光标高地址
//     out_8(CRT_DATA_REG, ((pos - MEM_BASE) >> 9) & 0xff);
//     out_8(CRT_ADDR_REG, CRT_CURSOR_L);
//     out_8(CRT_DATA_REG, ((pos - MEM_BASE) >> 1) & 0xff);
// }

// console_clear()
// {
//     screen = MEM_BASE;
//     pos = MEM_BASE;
//     x = y = 0;
//     set_cursor();
//     set_screen();

//     u16 *ptr = (u16 *)MEM_BASE;
//     while (ptr < (u16 *)MEM_END)
//     {
//         *ptr++ = erase;
//     }
// }

// // 向上滚屏
// static void scroll_up()
// {
//     if (screen + SCR_SIZE + ROW_SIZE >= MEM_END)
//     {
//         memcpy((void *)MEM_BASE, (void *)screen, SCR_SIZE);
//         pos -= (screen - MEM_BASE);
//         screen = MEM_BASE;
//     }

//     u32 *ptr = (u32 *)(screen + SCR_SIZE);
//     for (usize i = 0; i < WIDTH; i++)
//     {
//         *ptr++ = erase;
//     }
//     screen += ROW_SIZE;
//     pos += ROW_SIZE;
//     set_screen();
// }

// static void command_lf()
// {
//     if (y + 1 < HEIGHT)
//     {
//         y++;
//         pos += ROW_SIZE;
//         return;
//     }
//     scroll_up();
// }

// static void command_cr()
// {
//     pos -= (x << 1);
//     x = 0;
// }

// static void command_bs()
// {
//     if (x)
//     {
//         x--;
//         pos -= 2;
//         *(u16 *)pos = erase;
//     }
// }

// static void command_del()
// {
//     *(u16 *)pos = erase;
// }

// console_write(u8 *buf, u32 count)
// {
//     char ch;
//     while (count--)
//     {
//         ch = *buf++;
//         switch (ch)
//         {
//         case ASCII_NUL:
//             break;
//         case ASCII_BEL:
//             // todo \a
//             break;
//         case ASCII_BS:
//             command_bs();
//             break;
//         case ASCII_HT:
//             break;
//         case ASCII_LF:
//             command_lf();
//             command_cr();
//             break;
//         case ASCII_VT:
//             break;
//         case ASCII_FF:
//             command_lf();
//             break;
//         case ASCII_CR:
//             command_cr();
//             break;
//         case ASCII_DEL:
//             command_del();
//             break;
//         default:
//             if (x >= WIDTH)
//             {
//                 x -= WIDTH;
//                 pos -= ROW_SIZE;
//                 command_lf();
//             }

//             *((char *)pos) = ch;
//             pos++;
//             *((char *)pos) = attr;
//             pos++;

//             x++;
//             break;
//         }
//     }
//     set_cursor();
// }

// console_init()
// {
//     console_clear();
// }