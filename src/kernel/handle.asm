[bits 32]
; 这里调用关系复杂, 解释一下, INTERRUPT_HANDLER这个宏生成和handler_entry_table表里对应的函数, handler_entry_table表里存放呃是宏生成的每个函数的位置的指针
; 然后interrupt.c语言初始化中断向量表的时候, 把handler_entry_table里面函数的指针, 加载到idt里面, 
; 在异常或者中断发生的时候, 有的中断会push进去一个eflag 有的不会, 这里 通过宏整理判断 没有push的都会push进去一点东西, 保证参数数量 统一一点
; 然后跳入interrupt_entry中执行, 根据中断号, 去调用interrupt.c定义的 handler_table, 然后iret

; 导入c语言 整理过的中断向量表
; c语言整理的其实也是下面的 handler_entry_table
extern handler_table

;  中断发生时栈的布局为:  低地址(栈顶) %1(中断向量号) gp错误码(如果有) ip cs eflags 低高地址(栈底)
section .text

; ## gp错误码
; !!注意, 只有硬中断才会压入gp错误码,  而我们手动调用int 不会触发, (ps: int 一个我们没有定义中断处理函数的中断, 会调用0x0d,由我们软中断->硬中断)
; 这个宏主要是为 没有gp错误码的异常, 随便压入一个参数到栈中为了保持中断时的一致,后面容易进行弹栈操作, 然后push到终端处理函数的入口
; gp错误码的格式 0b0000000000000_01_0,   第1位是判断有内部还是由外部触发, 第2~3位表示是 IDT(01或者11) 还是GDT(00)还是LDT(10)  4~16位表示选择子索引

; int 0x80 的异常码 就是 0x00000402 换成2进制就是 10000000_01_0,  其中二进制的10000000 换算16进制就是0x80, 
; 由于0x80 我们没有中断处理函数, 手动软中断出发之后, 又会立即硬中断所以会又触发0x0d一般性保护异常, 把异常压入栈
; 在80286中 gp错误码是16个bit, 现在为了兼容 扩充到了32位

; ## eflag ; sti打开中断这个指令会把 if位 置为1,
; 如果使用sti指令 后压入栈的 eflags就是 0x212, 如果关闭中断则压入栈的eflags就变成了0x012, 0x212或者0x012 这俩数, 他们的bit位的第九位存在差别
; 此时压入栈的是0x212, 而eflag寄存器则是0x12, 因为我们初始化中断描述符的时候, 设置的是中断门 所以第九位被置为了0保存在了eflag寄存器中
; 所以, 在中断的时候 eflag中的 if位又被设置成了0(关闭中断状态), 如果我们想在中断处理函数内调度用户任务, 需要打开中断, 让用户任务执行的时候可以触发中断

; 时钟中断时 0x20 这个也没有错误码
%macro INTERRUPT_HANDLER 2
interrupt_handler_%1:
    xchg bx, bx  ; 这时候观察栈, 如果有状态码
%ifn %2
    push 0x20222202  ; 这个是给没有错误码的调用添加一个自己定义的魔数
%endif
    push %1; 压入宏的中断向量，跳转到中断入口
    jmp interrupt_entry
%endmacro


interrupt_entry:
    mov eax, [esp]
    ; 调用中断处理函数 handler_table 中存储了中断处理函数的指针, 并把 上方push %1 中断号传入中断处理函数的指针(所以这里 *4)
    call [handler_table + eax * 4]
    ; 弹栈, 弹出 %1, 弹出0x20222202, 后面调用iret

    add esp, 8
    iret

; 下面的功能, 主要是用 用宏生成代码
; 异常处理
INTERRUPT_HANDLER 0x00, 0; divide by zero
INTERRUPT_HANDLER 0x01, 0; debug
INTERRUPT_HANDLER 0x02, 0; non maskable interrupt
INTERRUPT_HANDLER 0x03, 0; breakpoint

INTERRUPT_HANDLER 0x04, 0; overflow
INTERRUPT_HANDLER 0x05, 0; bound range exceeded
INTERRUPT_HANDLER 0x06, 0; invalid opcode
INTERRUPT_HANDLER 0x07, 0; device not avilable

INTERRUPT_HANDLER 0x08, 1; double fault
INTERRUPT_HANDLER 0x09, 0; coprocessor segment overrun
INTERRUPT_HANDLER 0x0a, 1; invalid TSS
INTERRUPT_HANDLER 0x0b, 1; segment not present

INTERRUPT_HANDLER 0x0c, 1; stack segment fault
INTERRUPT_HANDLER 0x0d, 1; general protection fault
INTERRUPT_HANDLER 0x0e, 1; page fault
INTERRUPT_HANDLER 0x0f, 0; reserved

INTERRUPT_HANDLER 0x10, 0; x87 floating point exception
INTERRUPT_HANDLER 0x11, 1; alignment check
INTERRUPT_HANDLER 0x12, 0; machine check
INTERRUPT_HANDLER 0x13, 0; SIMD Floating - Point Exception

INTERRUPT_HANDLER 0x14, 0; Virtualization Exception
INTERRUPT_HANDLER 0x15, 1; Control Protection Exception
INTERRUPT_HANDLER 0x16, 0; reserved
INTERRUPT_HANDLER 0x17, 0; reserved

INTERRUPT_HANDLER 0x18, 0; reserved
INTERRUPT_HANDLER 0x19, 0; reserved
INTERRUPT_HANDLER 0x1a, 0; reserved
INTERRUPT_HANDLER 0x1b, 0; reserved

INTERRUPT_HANDLER 0x1c, 0; reserved
INTERRUPT_HANDLER 0x1d, 0; reserved
INTERRUPT_HANDLER 0x1e, 0; reserved
INTERRUPT_HANDLER 0x1f, 0; reserved

; 外中断
INTERRUPT_HANDLER 0x20, 0; clock 时钟中断
INTERRUPT_HANDLER 0x21, 0; keyboard 键盘中断
INTERRUPT_HANDLER 0x22, 0
INTERRUPT_HANDLER 0x23, 0; com2 串口2
INTERRUPT_HANDLER 0x24, 0; com1 串口1
INTERRUPT_HANDLER 0x25, 0
INTERRUPT_HANDLER 0x26, 0
INTERRUPT_HANDLER 0x27, 0
INTERRUPT_HANDLER 0x28, 0; rtc 实时时钟
INTERRUPT_HANDLER 0x29, 0
INTERRUPT_HANDLER 0x2a, 0
INTERRUPT_HANDLER 0x2b, 0
INTERRUPT_HANDLER 0x2c, 0
INTERRUPT_HANDLER 0x2d, 0
INTERRUPT_HANDLER 0x2e, 0; harddisk1 硬盘主通道
INTERRUPT_HANDLER 0x2f, 0; harddisk2 硬盘从通道


; 下面的数组记录了每个中断入口函数的指针, 每个4个字节 dd
; 这里每个指针就是 INTERRUPT_HANDLER 宏生成的 函数的入口地址
; 后面还需要再导出handler_entry_table 到c语言
section .data
global handler_entry_table
handler_entry_table:
    dd interrupt_handler_0x00
    dd interrupt_handler_0x01
    dd interrupt_handler_0x02
    dd interrupt_handler_0x03
    dd interrupt_handler_0x04
    dd interrupt_handler_0x05
    dd interrupt_handler_0x06
    dd interrupt_handler_0x07
    dd interrupt_handler_0x08
    dd interrupt_handler_0x09
    dd interrupt_handler_0x0a
    dd interrupt_handler_0x0b
    dd interrupt_handler_0x0c
    dd interrupt_handler_0x0d
    dd interrupt_handler_0x0e
    dd interrupt_handler_0x0f
    dd interrupt_handler_0x10
    dd interrupt_handler_0x11
    dd interrupt_handler_0x12
    dd interrupt_handler_0x13
    dd interrupt_handler_0x14
    dd interrupt_handler_0x15
    dd interrupt_handler_0x16
    dd interrupt_handler_0x17
    dd interrupt_handler_0x18
    dd interrupt_handler_0x19
    dd interrupt_handler_0x1a
    dd interrupt_handler_0x1b
    dd interrupt_handler_0x1c
    dd interrupt_handler_0x1d
    dd interrupt_handler_0x1e
    dd interrupt_handler_0x1f

    ; 外中断
    dd interrupt_handler_0x20
    dd interrupt_handler_0x21
    dd interrupt_handler_0x22
    dd interrupt_handler_0x23
    dd interrupt_handler_0x24
    dd interrupt_handler_0x25
    dd interrupt_handler_0x26
    dd interrupt_handler_0x27
    dd interrupt_handler_0x28
    dd interrupt_handler_0x29
    dd interrupt_handler_0x2a
    dd interrupt_handler_0x2b
    dd interrupt_handler_0x2c
    dd interrupt_handler_0x2d
    dd interrupt_handler_0x2e
    dd interrupt_handler_0x2f


; [bits 32]
; ; 中断处理函数入口

; section .text

; extern printk ; 导入一个外部符号进来

; global interrupt_handler  ; 导出一个符号

; interrupt_handler:

;     ; 传入参数
;     push message
;     call printk

;     ; 恢复上面的 push message之前的栈
;     add esp, 4

;     iret


; section .data
; message:
;     db "default interrupt", 10, 13, 0