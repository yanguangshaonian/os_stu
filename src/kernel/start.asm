[bits 32]

; 导入外部符号
extern kernel_init

; 导出符号
global _start


_start:
    call kernel_init
    ; int 0x0d
    ; mov bx, 0
    ; div bx

    jmp $


    