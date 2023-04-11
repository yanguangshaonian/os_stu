#include <onix/string.h>

strcpy(u8* dest, const u8* src){
    while (true){
        *dest = *src;
        if (*src == EOS){
            break;
        }
        dest += 1;
        src += 1;
    }
}

strncpy(u8* dest, const u8* src, usize count){
    for (usize i = 0; i < count; i++){
        if (*src == EOS){
            break;
        }
        *dest = *src;
        dest += 1;
        src += 1;
    }
    *dest = EOS;
}

strcat(u8* dest, const u8* src){
    // 找到dest 的结尾
    while (true) {
        if (*dest == EOS) {
            break;
        }
        dest += 1;
    }

    // 从结尾开始拼接
    while (true){
        *dest = *src;
        if (*src == EOS){
            break;
        }
        dest += 1;
        src += 1;
    }
}

usize strlen(const u8* src){
    u8* start = src;
    while (*src != EOS){
         src += 1;
    }
    return src - start;
}

i8 strcmp(const u8* lhs, const u8* rhs){
    while (*lhs == *rhs && *lhs != EOS && *rhs != EOS){
        lhs += 1;
        rhs += 1;
    }

    // 比较不相同的一位
    if (*lhs < *rhs) {
        return -1;
    } else if (*lhs == *rhs){
        return 0;
    }
    return 1;
}

u8* strchr_l(const u8* src, u8 ch) {
    while (true){
        if (*src == EOS) {
            return nullptr;
        } else if (*src == ch){
            return src;
        }
        src += 1;
    }
}

u8* strchr_r(const u8* src, u8 ch) {
    u8* last_ptr = nullptr;
    while (true){
        if (*src == EOS) {
            return last_ptr;
        } else if (*src == ch){
            last_ptr = src;
        }
        src += 1;
    }
}

i8 memcmp(const u8* lhs, const u8* rhs, usize count){
    if (count == 0){
        return 0;
    }
    
    for (usize i = 0; i < count; i++){
        if (*lhs != *rhs) {
            break;
        }
        lhs += 1;
        rhs += 1;
    }
    if (*lhs < *rhs) {
        return -1;
    } else if (*lhs == *rhs){
        return 0;
    }
    return 1;
}

memset(u8* src, u8 ch, usize count){
    for (usize i = 0; i < count; i++){
        *src = ch;
        src += 1;
    }
}

memcpy(u8* dest, const u8* src, usize count){
    for (usize i = 0; i < count; i++){
        *dest = *src;
        dest += 1;
        src += 1;
    }
}

u8* memchr(const u8* src, u8 ch, usize count){
    for (usize i = 0; i < count; i++){
        if (*src == ch){
            return src;
        }
        src += 1;
    }
    return nullptr;
}



void test_string(){
    u8 message[] = "hello~";
    u8 buffer[1024];  
      
    u8* video = (u8*)0xb8000;
    for (usize i = 0; i < sizeof(message); i++){
        video[i * 2] = message[i];
    }

    int res;
    res = strcmp(message, buffer);  // 1
    strcpy(buffer, message);
    res = strcmp(message, buffer);  // 0
    strcat(buffer, message);        // 拼接一下
    res = strcmp(message, buffer);  // -1
    res = strlen(buffer);           // 12
    res = strlen(message);          // 6
    res = sizeof(message);          // 7  sizeof 把 0 也统计了


    usize l_pos = strchr_l(message, 'l') - message;  // 2
    l_pos = strchr_r(message, 'l') - message;        // 3 这是最后一个l 他在指针开始下标为 3 的位置

    memset(buffer, 0, sizeof(buffer));       // 重置为0

    res = memcmp(message, buffer, sizeof(message));  // 比较内存的大小, buff重置了之后, 这里结果应该是 1

    memcpy(buffer, message, sizeof(message)); 
    res = memcmp(message, buffer, sizeof(message));  // copy完 再比较一下  应该是 0

}