[bits 32]



section .text  ; 代码段

global in_8, in_16, out_8 ; 将符号公开导入, 外部可以使用

in_8:
    ; 栈帧保存
    push ebp
    mov ebp, esp


    xor eax, eax
    mov edx, [ebp + 8]  ; +0 是本身ebp的值, +4 是call in_8所在代码的下一行, +8 是第一个参数port

    in al, dx  ; 将 port 的数据 读取1个字节 (8个比特位 al) 到al, 根据调用约定, eax作为返回值

    jmp $+2
    jmp $+2
    jmp $+2

    ; 恢复栈帧
    leave
    ret


in_16:
    ; 栈帧保存
    push ebp
    mov ebp, esp


    xor eax, eax
    mov edx, [ebp + 8]

    in ax, dx

    jmp $+2
    jmp $+2
    jmp $+2

    ; 恢复栈帧
    leave
    ret



out_8:
    ; 栈帧保存
    push ebp
    mov ebp, esp


    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12]  ; value
    
    out dx, al  ; 将 value 的8比特写入到 port

    jmp $+2
    jmp $+2
    jmp $+2

    ; 恢复栈帧
    leave
    ret

out_16:
    ; 栈帧保存
    push ebp
    mov ebp, esp


    mov edx, [ebp + 8]  ; port
    mov eax, [ebp + 12]  ; value
    
    out dx, ax  ; 将 value 的 16比特写入到 port

    jmp $+2
    jmp $+2
    jmp $+2

    ; 恢复栈帧
    leave
    ret
