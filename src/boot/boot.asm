; 声明这段代码的位置运行时会在0x7c00, 直接取址会加上0x7c00
; xchg bx, bx  ; bochs 的魔数, 代码执行到这里会停下

[org 0x7c00]

start:
    init:
        ; 设置屏幕模式微文本模式, 清除屏幕
        mov ax, 3
        int 0x10

        ; 初始化段寄存器
        mov ax, 0
        mov bx, ax
        mov cx, ax
        mov dx, ax

        mov ds, ax
        mov ss, ax
        mov es, ax

        mov si, ax
        mov di, ax

        ; 修改栈顶为0x7c00, 使其向下增长
        mov sp, 0x7c00

    ; 读取loader.bin
    mov edi, 0x1000  ; 读取到目标内存地址(32位地址空间)
    mov ebx, 2  ; 从第 n 个扇区开始读(32位 扇区最大有 2的27次方个)
    mov cl, 4  ; 读 1个 扇区(8位 每次最多读取256个扇区)
    call read_disk

    ; 校验上方读入的数据
    cmp word es:[0x1000], 0xa0a0
    jnz error

    ; mov edi, 0x1000  ; 把內存中什么地方的数据写出来
    ; mov ebx, 1  ; 从第 n 个扇区开始写(32位 扇区最大有 2的27次方个)
    ; mov cl, 1  ; 写 1个 扇区(8位 每次最多读取256个扇区)
    ; call write_disk

    mov si, booting
    call print


    jmp 0x1002


    ; 阻塞, 一直跳转到当前行
    jmp $
    ret


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



write_disk:
    push edx
    push eax

    ; ecx寄存器校验, 使其最大值为256
    and ecx,0b1111_1111

    ; 设置写入扇区的数量 0x1f2
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

    ; 模式为 LBA模式
    or al, 0b1110_0000  ; al: 1110_0000
    out dx, al     

    ; 从硬盘设置写  0x1f7
    inc dx
    mov al, 0x30    ; al: 0x20
    
    out dx, al


    ; 写入到指定为止
    call .writes
    jmp .end


    .writes:
        ; 等待硬盘空闲
        call .waits
        
        push cx
        ; 写入每个扇区的512字节, 每次写2字节读256次
        mov cx, 256
        mov dx, 0x1f0

        .writew:
            mov ax, es:[edi]
            out dx, ax
            jmp $+2
            jmp $+2
            jmp $+2
            mov es:[edi], ax
            add edi, 2
            loop .writew
        pop cx
        ; 如果写了多个扇区 继续循环
        loop .writes
        ret

    ; 循环检查磁盘状态
    .waits:
        mov dx, 0x1f7
        in al,dx
        jmp $+2
        jmp $+2
        jmp $+2

        ; 如果 第三位是1, 说明准备好了
        and al, 0b1000_0000  ; 写入,不需要校验第三位了, 秩序要校验第七位
        cmp al, 0b0000_0000
        jnz .waits
        ret

    .end:
        pop eax
        pop edx
        ret



; 一个print函数, 调用时 把字符串的地址 mov到si寄存器即可
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


error:
    mov si, .error_msg
    call print
    hlt  ; 让 CPU 停止
    jmp $
    ret
    .error_msg db "Booting Error!!!", 10, 13, 0




booting:
    db "Booting Start ...", 10, 13, 0  ; 结尾 \n \r 0


padding:
    ; 填充数据, 引导扇区必须为512字节,  最后两个字节是魔数, 除了代码之外必须用0填充
    times 510 - ($ - $$) db 0  ; 最后俩字节是0xaa55, 所以一共需要填充 510 - (当前行位置 - 开始的的位置) = 510 - 代码段的大小


; 魔数
dw 0xaa55