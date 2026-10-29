# Tools
ASM      = nasm
CC       = gcc
LD       = ld
OBJCOPY  = objcopy
QEMU     = qemu-system-i386

# Compiler and linker flags
CFLAGS  = -m32 -ffreestanding -fno-pie -fno-pic -fno-asynchronous-unwind-tables
LDFLAGS = -m elf_i386 -T linker.ld

# Output files
BOOT_BIN    = boot.bin
ENTRY_OBJ   = entry.o
ISR_OBJ     = isr.o
KERNEL_OBJ  = kernel.o
IDT_OBJ     = idt.o
EXCEPTION_OBJ = exception.o
KERNEL_ELF  = kernel.elf
KERNEL_BIN  = kernel.bin
IMAGE       = arceon.img
PIC_OBJ = pic.o
IRQ_OBJ = irq.o

# Default target
all: $(IMAGE)

# Assemble kernel entry
$(ENTRY_OBJ): boot/entry.asm
	$(ASM) -f elf32 $< -o $@

# Assemble exception entry stubs
$(ISR_OBJ): kernel/isr.asm
	$(ASM) -f elf32 $< -o $@

# Compile kernel
$(KERNEL_OBJ): kernel/kernel.c kernel/idt.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile IDT implementation
$(IDT_OBJ): kernel/idt.c kernel/idt.h
	$(CC) $(CFLAGS) -c $< -o $@

# Compile exception handler
$(EXCEPTION_OBJ): kernel/exception.c
	$(CC) $(CFLAGS) -c $< -o $@

$(PIC_OBJ): kernel/pic.c kernel/pic.h
	$(CC) $(CFLAGS) -c kernel/pic.c -o $@

$(IRQ_OBJ): kernel/irq.c kernel/irq.h kernel/pic.h
	$(CC) $(CFLAGS) -c kernel/irq.c -o $@

# Link kernel
# Link kernel
$(KERNEL_ELF): $(ENTRY_OBJ) $(ISR_OBJ) $(KERNEL_OBJ) $(IDT_OBJ) $(EXCEPTION_OBJ) $(PIC_OBJ) $(IRQ_OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(ENTRY_OBJ) $(ISR_OBJ) $(KERNEL_OBJ) $(IDT_OBJ) $(EXCEPTION_OBJ) $(PIC_OBJ) $(IRQ_OBJ)

# Convert ELF to raw binary
$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

# Assemble bootloader using actual kernel size
$(BOOT_BIN): boot/boot.asm $(KERNEL_BIN) tools-build.sh
	./tools-build.sh

# Create disk image with enough room for the kernel
$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(IMAGE)
	truncate -s $$((512 + (($$(stat -c %s $(KERNEL_BIN)) + 511) / 512) * 512)) $(IMAGE)

# Run in QEMU
run: $(IMAGE)
	$(QEMU) -drive format=raw,file=$(IMAGE)

# Remove build artifacts
# Remove build artifacts
clean:
	rm -f $(BOOT_BIN) $(ENTRY_OBJ) $(ISR_OBJ) $(KERNEL_OBJ) $(IDT_OBJ) $(EXCEPTION_OBJ) $(PIC_OBJ) $(IRQ_OBJ) $(KERNEL_ELF) $(KERNEL_BIN) $(IMAGE)

# Rebuild everything
rebuild: clean all

test-exception:
	$(MAKE) clean
	$(MAKE) CFLAGS="$(CFLAGS) -DTEST_EXCEPTION=$(EXCEPTION)" run

.PHONY: all run clean rebuild test-exception

