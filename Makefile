# Tools
ASM      = nasm
CC       = gcc
LD       = ld
OBJCOPY  = objcopy
QEMU     = qemu-system-i386

# Compiler / Linker flags

CFLAGS = -m32 \
         -ffreestanding \
         -fno-pie \
         -fno-pic \
         -fno-asynchronous-unwind-tables

LDFLAGS = -m elf_i386 \
          -T linker.ld


# Output files

BOOT_BIN   = boot.bin
ENTRY_OBJ  = entry.o
KERNEL_OBJ = kernel.o
KERNEL_ELF = kernel.elf
KERNEL_BIN = kernel.bin
IMAGE      = arceon.img


# Default target

all: $(IMAGE)


# Bootloader

$(BOOT_BIN): boot/boot.asm
	$(ASM) -f bin $< -o $@


# Kernel entry assembly

$(ENTRY_OBJ): boot/entry.asm
	$(ASM) -f elf32 $< -o $@


# Kernel C code

$(KERNEL_OBJ): kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@


# Link kernel

$(KERNEL_ELF): $(ENTRY_OBJ) $(KERNEL_OBJ) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(ENTRY_OBJ) $(KERNEL_OBJ)


# Convert ELF → raw binary

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@


# Create disk image

$(IMAGE): $(BOOT_BIN) $(KERNEL_BIN)
	cat $(BOOT_BIN) $(KERNEL_BIN) > $(IMAGE)
	truncate -s 1024 $(IMAGE)


# Run in QEMU

run: $(IMAGE)
	$(QEMU) -drive format=raw,file=$(IMAGE)


# Clean build files

clean:
	rm -f $(BOOT_BIN) \
	      $(ENTRY_OBJ) \
	      $(KERNEL_OBJ) \
	      $(KERNEL_ELF) \
	      $(KERNEL_BIN) \
	      $(IMAGE)


# Rebuild everything

rebuild: clean all


.PHONY: all run clean rebuild