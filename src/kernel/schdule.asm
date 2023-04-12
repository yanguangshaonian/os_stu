[bits 32]

global task_switch

task_switch:
    ; 保存栈帧
    push ebp
    mov ebp, esp

    ; 调用ABI约定, 保存寄存器
    push ebx
    push esi
    push edi

    mov eax, esp  ;先得到当前调用栈
    and eax, 0xfffff000  ; 当前任务的最开始的地方, 也就是线程栈的开始这里保存了4字节的当前线程的栈顶指针

    ; 保存当前调用栈的的 esp 栈顶 到 task_t->stack 也就是0x1000 或者0x2000处的地方
    mov [eax], esp

    ; 得到传进来的参数 next,  ebp保存的是current在进入函数之后最开始的栈顶, ebp+0保存的本来的ebp, ebp+4保存的call task_switch之后下一行地址
    ; ebp +8 得到是传进来的参数 即 next线程栈的最开始的地址(把栈顶,0x1000或者0x2000保存的4字节栈顶 传入到 esp中)
    mov eax, [ebp + 8] 

    ; next的 的栈顶指针
    mov esp, [eax]

    ; 在任务第一次进入到这里的时候会把我们设置的寄存器的默认值 pop 到指定寄存器中
    pop edi
    pop esi
    pop ebx
    pop ebp

    ; 此时 栈顶是 call task_switch 的下一行代码的位置, ret即可 
    ret
