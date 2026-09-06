# Arceon 0.2

Arceon is an experimental operating system project built from scratch to explore x86 architecture, bootloaders, protected mode, and kernel development.

## Arceon 0.2

This version implements the transition from **16-bit Real Mode to 32-bit Protected Mode** and loads a C kernel from disk.

### Features

* x86 bootloader written in NASM
* Kernel loading using BIOS `INT 13h`
* Global Descriptor Table (GDT)
* Protected Mode initialization
* 32-bit segment setup
* 32-bit stack initialization
* Assembly-to-C kernel entry
* Direct VGA text output
* Makefile-based build system
* QEMU testing environment

### Boot Flow

```text
BIOS
 ↓
Bootloader @ 0x7C00
 ↓
Load Kernel @ 0x8000
 ↓
Load GDT
 ↓
Enable Protected Mode
 ↓
Initialize 32-bit Environment
 ↓
Kernel Entry
 ↓
kernel_main()
```

### Project Structure

```text
Arceon/
├── boot/
│   ├── entry.asm
│   └── boot.asm
├── kernel/
│   └── kernel.c
├── linker.ld
├── Makefile
└── README.md
```

### Requirements

* NASM
* GCC with 32-bit support
* GNU Binutils
* Make
* QEMU

### Build

```bash
make
```

### Run

```bash
make run
```

### Clean

```bash
make clean
```

### Memory Layout

| Component  |   Address |
| ---------- | --------: |
| Bootloader |  `0x7C00` |
| Kernel     |  `0x8000` |
| Stack      | `0x90000` |
| VGA Memory | `0xB8000` |

### Learning Focus

Arceon 0.2 focuses on:

* x86 Real Mode
* BIOS services
* GDT and segmentation
* Protected Mode
* Kernel loading
* Linker scripts
* Assembly and C integration
* Low-level memory access

## Next

**Arceon 0.3** will focus on building a basic VGA text driver and improving kernel output handling.

---

**Status:** Experimental / Educational
