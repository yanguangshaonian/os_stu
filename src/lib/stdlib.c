#include <onix/stdlib.h>

void delay(usize count){
    for (usize i = 0; i < count; i++);
}

void hang(){
    while (true){
        
    }
}


u8 bcd_to_bin(u8 value){
    return (value & 0xf) + (value >> 4) * 10;
}
u8 bin_to_bcd(u8 value){
    return (value / 10) * 0x10 + (value % 10);
}