[org 0x1000]

; 校验用的, 这里会被读到 0x1000处
dw 0xa0a0

mov si, loading_log
call print
call detect_memory
call prepare_protect_mode

jmp $



detect_memory:
    ; 设置检测内存的buffer位置
    mov ax, 0
    mov es, ax
    mov edi, mem_struct_buffer

    ; 固定签名
    mov edx, 0x534d4150

    ; 置为0, 每次系统调用会修改这个寄存器
    xor ebx, ebx

    ; 保存的目的地
    mov di, mem_struct_buffer

    .next:
        ; 子功能号
        mov eax, 0xe820
        ; ards 结构的大小 (字节)
        mov cx, 20
        ; 调用中断
        int 0x15
        ; 如果cf置位, 表示出错了
        jc error

        ; 计算下一个内存结构体保存的首地址
        add di, cx

        inc word [mem_struct_count]

        ; 不为0 说明检查未完成
        cmp ebx, 0
        jnz .next


    mov si, detect_memory_log
    call print
    ret

    ; ; 循环结构体内的值(我们只读取低32位相关的信息, 高32位的暂时不需要)
    ; mov cx, [mem_struct_count]
    ; ; 初始偏移量
    ; mov si, 0
    ; .show
    ;     mov eax, [mem_struct_buffer + si]  ; 基地址 低32位
    ;     mov ebx, [mem_struct_buffer + si + 8]  ; 内存长度的低32位
    ;     mov edx, [mem_struct_buffer + si + 16]  ; 本段内存类型  1: 可以使用, 2: 内存使用或者被保留中, 其他: 未定义

    ;     add si, 20
    ;     ; xchg bx, bx  ; bochs 的魔数, 代码执行到这里会停下
    ;     loop .show


print:
    push ax
    mov ah, 0x0e
    .show:
        mov al,[si]
        cmp al, 0
        jz .end
        int 0x10
        inc si
        jmp .show
    .end:
        pop ax
        ret

prepare_protect_mode:

    cli  ; 关闭中断

    ; ; 打开 A20线
    ; mov al, 0xdd
    ; out 0x64, al

    ; 打开 A20 线
    in al,  0x92
    or al, 0b10
    out 0x92, al

    ; 进入保护模式
    mov eax, cr0
    or eax, 0b1
    mov cr0, eax

    ; 加载 gdt
    lgdt [gdt_ptr]

    ; 长跳转, 刷新缓存, 跳进保护模式
    jmp dword code_selecter:protect_mode

    ret

[bits 32]
protect_mode:
    ; 这里已经进入了保护模式
    
    ; 初始化段寄存器(将代码段之外的寄存器都设置为 数据段)
    mov ax, data_selecter
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    
    ; 修改自己设置的栈顶
    mov esp, 0x10000

    ; 进入保护模式之后, 实模式的print就不能用了
    mov byte [0xb8000], 'P'
    
    ; 加载kernel, 把从10扇区 读取200个扇区的数据, 读到0x10000
    mov edi, 0x10000  ; 
    mov ebx, 10
    mov cl, 200 
    call read_disk
    
    mov eax, 0x20220205  ; 内核魔数
    mov ebx, mem_struct_count  ; 内存结构体 数量统计
    mov ecx, mem_struct_buffer  ; 内存结构体 的起始位置
    jmp dword code_selecter:0x10000
    ud2  ; 执行到这里直接出错(不可能执行到这里)
    jmp $

read_disk:
    push edx
    push eax

    ; ecx寄存器校验, 使其最大值为256
    and ecx,0b1111_1111

    ; 设置读取扇区的数量 0x1f2
    mov dx, 0x1f2
    mov al, cl  ; cl:1,  al:1
    out dx, al  ; out 0x1f2, 1

    mov eax, ebx ; eax: 0

    ; 起始扇区的 0~7位 设置 0x1f3
    inc dx
    out dx, al  ; al: 0

    ; 起始扇区的 8~15位 设置 0x1f4
    inc dx
    shr eax, 8
    out dx, al ; al:0

    ; 起始扇区的 16~23位 设置 0x1f5
    inc dx
    shr eax, 8
    out dx, al  ; al:0

    ; 起始扇区的 24~27位 设置 0x1f6
    inc dx
    shr eax, 8  ; al: 0
    
    ; 高4位设为0, 低四位保持不变
    and al, 0b0000_1111   ; al: 0000_0000

    ; 读取模式为 LBA模式
    or al, 0b1110_0000  ; al: 1110_0000
    out dx, al     

    ; 从硬盘读  0x1f7
    inc dx
    mov al, 0x20    ; al: 0x20
    
    out dx, al


    ; 等待读取完毕
    call .waits
    ; 读取到指定为止
    call .reads
    jmp .end


    .reads:
        push cx
        ; 读取每个扇区的512字节, 每次读2字节读256次
        mov cx, 256
        mov dx, 0x1f0

        .readw:
            in ax, dx
            jmp $+2
            jmp $+2
            jmp $+2
            mov es:[edi], ax
            add edi, 2
            loop .readw
        pop cx
        ; 如果读取了多个扇区 继续循环
        loop .reads
        ret

    ; 循环检查磁盘状态
    .waits:
        mov dx, 0x1f7
        in al,dx
        jmp $+2
        jmp $+2
        jmp $+2

        ; 如果 第三位是1, 说明准备好了
        and al, 0b1000_1000
        cmp al, 0b0000_1000
        jnz .waits
        ret

    .end:
        pop eax
        pop edx
        ret

error:
    mov si, .error_msg
    call print
    hlt  ; 让 CPU 停止
    jmp $
    ret
    .error_msg db "Loader Error!!!", 10, 13, 0

mem_struct_count:  ; 一共多少内存结构体
    dd 0

; 用来存放检测内存结果的结构体
mem_struct_buffer:
    times 20*10 db 0


; 定义gdt
base equ 0  ; 段地址
limit equ 0xfffff  ; 段界限数量, 和粒度搭配  如果粒度是4k, 那么 0xfffff*4096 最大的大小

; 段选择子
code_selecter equ 1 << 3  ; 选择子 前13位 代表索引, 后面3位暂时不需要
data_selecter equ 2 << 3  ; 

; gdt表的指针
gdt_ptr:
    dw gdt_end - gdt_start - 1  ; 16位表示gdt的总大小, 每个段描述符8字节, 2**16/8=8192刚好可以表示8292; (2的13次方个刚好是段选择子的最大索引)
    dd gdt_start                ; gdt表起始位置


; gdt 表的详情
gdt_start:
    gdt_base:
        times 8 db 0  ; 第0个段描述符 不能被选择也不可使用和访问, 否则会cpu发出异常
    gdt_code:
        dw limit                          ; 段界限 dw 0~15
        dw base                           ; 段基址 dw 0~15
        db base >> 16                     ; 段基址16~23
        db 0b_1_00_1_1010                  ; P_DPL_S_TYPE(代码段, 非依从, 可读, 没有被cpu访问过)
        db 0b_1_1_0_0_0000 | limit >> 16   ; G_D/B_L_AVL | 段界限16~19
        db base >> 24                     ; 段基址24~31
    gdt_data:
        dw limit                          ; 段界限 dw 0~15
        dw base                           ; 段基址 dw 0~15
        db base >> 16                     ; 段基址16~23
        db 0b_1_00_1_0010                  ; P_DPL_S_TYPE(0010 表示"数据段,数据可写, 数据向上拓展")
        db 0b_1_1_0_0_0000 | limit >> 16   ; G_D/B_L_AVL | 段界限16~19
        db base >> 24                     ; 段基址24~31
    gdt_padding:
        times 2<<15-($-gdt_start) db 0
gdt_end:

loading_log:
    db 'Loader Start', 13, 10, 0

detect_memory_log:
    db 'detect_memory end', 13, 10, 0


