BUILD_PATH:=../build
SRC:= ./

# 内核的执行位置
KERNEL_ENTRY_POINT := 0x10000

# 内核编译 .c文件需要传递给gcc的变量
CFLAG := -m32  # 32位程序
CFLAG += -fno-builtin  # 不需要gcc内置函数(比如memcpy, 需要我们自己写)
CFLAG += -nostdinc  # 不需要标准头文件
CFLAG += -fno-pic  # 不需要位置无关的代码
CFLAG += -fno-pie  # 不需要位置无关的可执行程序
CFLAG += -nostdlib  # 不需要标准库
CFLAG += -fno-stack-protector  # 不需要栈保护
CFLAG += -fno-common
CFLAG := $(strip ${CFLAG})

DEBUG := -g  # 需要debug
INCLUDE := -I$(SRC)/include  # 头文件引入目录


# bootloader的编译
$(BUILD_PATH)/boot/%.bin:$(SRC)/boot/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f bin $< -o $@

# kernel还有其他asm的编译, 把start.asm 编译成elf格式的 start.o
$(BUILD_PATH)/%.o:$(SRC)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -f elf32 $(DEBUG) $< -o $@

# kernel.c还有其他 .c的编译
$(BUILD_PATH)/%.o:$(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	gcc $(CFLAG) $(DEBUG) $(INCLUDE) -c $< -o $@

# kernel的编译, 需要依赖上面的 start.o
# 使用链接器将所有依赖的($^)的文件链接成一个静态链接的文件
# 且重新编排指定程序的入口地址(_start符号)为 KERNEL_ENTRY_POINT 定义的地址，并将生成的可执行文件输出为 $@ 定义的文件名
$(BUILD_PATH)/kernel.bin:$(BUILD_PATH)/kernel/start.o \
		$(BUILD_PATH)/kernel/main.o\
		$(BUILD_PATH)/kernel/io.o\
		$(BUILD_PATH)/lib/string.o\
		$(BUILD_PATH)/lib/console.o\
		$(BUILD_PATH)/lib/vprintf.o\
		$(BUILD_PATH)/kernel/printk.o\
		$(BUILD_PATH)/lib/assert.o\
		$(BUILD_PATH)/kernel/debug.o\
		$(BUILD_PATH)/kernel/global.o\
		$(BUILD_PATH)/kernel/task.o\
		$(BUILD_PATH)/kernel/schdule.o\
		$(BUILD_PATH)/kernel/handle.o\
		$(BUILD_PATH)/kernel/interrupt.o\
		$(BUILD_PATH)/lib/stdlib.o\
		$(BUILD_PATH)/kernel/clock.o\
		$(BUILD_PATH)/kernel/time.o\
		$(BUILD_PATH)/kernel/rtc.o\
		$(BUILD_PATH)/kernel/memory.o\

	$(shell mkdir -p $(dir $@))
	ld -m elf_i386 -static $^ -o $@ -Ttext $(KERNEL_ENTRY_POINT)

# elf文件从磁盘进入内存还需要特殊处理, 使用这种方式, 直接读入内存
# objcopy -O binary 输出文件中只包含了输入文件的纯二进制数据，没有任何 ELF 文件格式的头部信息
# 如数据段, 代码段等
$(BUILD_PATH)/system.bin: $(BUILD_PATH)/kernel.bin
	objcopy -O binary $< $@

# 导出kernel的符号表
$(BUILD_PATH)/system.map: $(BUILD_PATH)/kernel.bin
	nm $< | sort > $@


# 创建镜像文件
$(BUILD_PATH)/master.img: $(BUILD_PATH)/boot/boot.bin \
	$(BUILD_PATH)/boot/loader.bin \
	$(BUILD_PATH)/system.bin \
	$(BUILD_PATH)/system.map
	
	yes | bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $@
	dd if=$(BUILD_PATH)/boot/boot.bin of=$@ bs=512 count=1 conv=notrunc
	dd if=$(BUILD_PATH)/boot/loader.bin of=$@ bs=512 seek=2 count=4 conv=notrunc
	dd if=$(BUILD_PATH)/system.bin of=$@ bs=512 seek=10 count=200 conv=notrunc

# 清理
.PHONY:clean
clean:
	@rm -rf $(BUILD_PATH)

test:  $(BUILD_PATH)/master.img

# 启动系统
.PHONY:bochs
bochs: $(BUILD_PATH)/master.img
	@rm -rf $(BUILD_PATH)/*lock
	bochs -q -f ../bochs/bochsrc

# 启动系统
.PHONY:bochsg
bochsg: $(BUILD_PATH)/master.img
	@rm -rf $(BUILD_PATH)/*lock
	bochs-gdb -q -f ../bochs/bochsrc.gdb


QEMU:= 	qemu-system-i386 \
	-m 64M \
	-boot c \
	-drive file=$(BUILD_PATH)/master.img,if=ide,index=0,media=disk,format=raw \
	-audiodev pa,id=hda \
	-machine pcspk-audiodev=hda \
	-rtc base=localtime \


.PHONY: qemug
qemug: $(BUILD_PATH)/master.img
	$(QEMU) -s -S \

.PHONY: qemu
qemu: $(BUILD_PATH)/master.img
	$(QEMU)