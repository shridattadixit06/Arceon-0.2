# Tools
ASM      = nasm
CC       = gcc
LD       = ld
OBJCOPY  = objcopy
QEMU     = qemu-system-i386

# Compiler and linker flags
CFLAGS  = -m32 -ffreestanding -fno-pie -fno-pic -fno-asynchronous-unwind-tables -Ikernel/include
LDFLAGS = -m elf_i386 -T linker.ld

# Output files
BOOT_BIN      = boot.bin
ENTRY_OBJ     = entry.o
ISR_OBJ       = isr.o
KERNEL_OBJ    = kernel.o
IDT_OBJ       = idt.o
EXCEPTION_OBJ = exception.o
PIC_OBJ       = pic.o
IRQ_OBJ       = irq.o
TIMER_OBJ     = timer.o
KEYBOARD_OBJ  = keyboard.o

KERNEL_ELF = kernel.elf
KERNEL_BIN = kernel.bin
IMAGE      = arceon.img

# All kernel objects
KERNEL_OBJECTS = $(ENTRY_OBJ) $(ISR_OBJ) $(KERNEL_OBJ) $(IDT_OBJ) $(EXCEPTION_OBJ) $(PIC_OBJ) $(IRQ_OBJ) $(TIMER_OBJ) $(KEYBOARD_OBJ)

# Default target
all: $(IMAGE)

# Assemble kernel entry
$(ENTRY_OBJ): boot/entry.asm
	$(ASM) -f elf32 $< -o $@

# Assemble interrupt stubs
$(ISR_OBJ): kernel/isr.asm
	$(ASM) -f elf32 $< -o $@

# Compile kernel
$(KERNEL_OBJ): kernel/kernel.c kernel/include/idt.h kernel/include/pic.h kernel/include/timer.h kernel/include/keyboard.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IDT implementation
$(IDT_OBJ): kernel/src/idt.c kernel/include/idt.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile exception handler
$(EXCEPTION_OBJ): kernel/src/exception.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile PIC implementation
$(PIC_OBJ): kernel/src/pic.c kernel/include/pic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IRQ implementation
$(IRQ_OBJ): kernel/src/irq.c kernel/include/irq.h kernel/include/pic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile PIT timer
$(TIMER_OBJ): kernel/src/timer.c kernel/include/timer.h kernel/include/irq.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile keyboard driver
$(KEYBOARD_OBJ): kernel/src/keyboard.c kernel/include/keyboard.h kernel/include/irq.h kernel/include/pic.h
	$(CC) $(CFLAGS) -c $< -o $@

# Link kernel
$(KERNEL_ELF): $(KERNEL_OBJECTS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(KERNEL_OBJECTS)

# Convert ELF to raw binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# Assemble bootloader using actual kernel size
$(BOOT_BIN): boot/boot.asm $(KERNEL_BIN) tools-build.sh
	./tools-build.sh

# Create disk image with enough room for the kernel
$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(IMAGE)
	truncate -s $$((512 + ($$(stat -c %s $(KERNEL_BIN)) + 511) / 512 * 512)) $(IMAGE)

# Run in QEMU
run: $(IMAGE)
	$(QEMU) -drive format=raw,file=$(IMAGE)

# Remove build artifacts
clean:
	rm -f $(BOOT_BIN) $(ENTRY_OBJ) $(ISR_OBJ) $(KERNEL_OBJ) $(IDT_OBJ) $(EXCEPTION_OBJ) $(PIC_OBJ) $(IRQ_OBJ) $(TIMER_OBJ) $(KEYBOARD_OBJ) $(KERNEL_ELF) $(KERNEL_BIN) $(IMAGE)

# Rebuild everything
rebuild: clean all

# Test CPU exceptions
test-exception:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -DTEST_EXCEPTION=$(EXCEPTION)" run

.PHONY: all run clean rebuild test-exception