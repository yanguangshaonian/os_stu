[bits 32]

; 导入外部符号
extern kernel_init
extern memory_init

; 导出符号
global _start


_start:
    push ecx  ; 内存结构体的buffer起始位置
    push ebx  ; 内存结构体的数量 u32 ptr 起始位置
    push eax  ; magic

    call memory_init  ; 初始化内存

    ; call kernel_init
    jmp $


    