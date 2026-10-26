# Arceon 0.2.0

**Arceon** is an experimental educational x86 operating system built from scratch to explore low-level systems programming, computer architecture, and operating system internals.

Arceon is developed in C and x86 assembly, with a focus on understanding how a machine transitions from BIOS boot into a freestanding kernel.

> **Status:** Experimental / Educational  
> **Version:** 0.2.0  
> **Architecture:** x86, 32-bit protected mode  
> **Language:** C and NASM Assembly  
> **Emulator:** QEMU

---

## Overview

Arceon 0.2.0 builds on the initial bootable foundation by implementing a transition from **16-bit Real Mode to 32-bit Protected Mode**, loading a freestanding C kernel, and introducing interrupt descriptor tables and exception handling.

The project also includes exception-testing support and VGA text-output functions to help debug and inspect kernel behavior.

## Features

### Bootloader and Kernel

- Custom BIOS bootloader written in NASM
- Kernel loading through BIOS `INT 13h`
- Kernel loaded at physical address `0x8000`
- Global Descriptor Table (GDT) initialization
- Transition from 16-bit Real Mode to 32-bit Protected Mode
- 32-bit segment register and stack initialization
- Assembly-to-C kernel entry point
- Freestanding C kernel linked using a custom linker script

### Interrupts and Exception Handling

- Interrupt Descriptor Table (IDT) implementation
- Assembly interrupt and exception entry stubs
- C-based exception handling
- Invalid Opcode exception testing support
- Configurable exception-testing build target for debugging

### VGA Output and Development

- Direct VGA text-mode memory access
- VGA output helper functions
- Makefile-based build system
- Automated bootloader assembly based on kernel size
- QEMU-based development and testing workflow

## Boot Flow

```text
BIOS
  |
  v
Bootloader at 0x7C00
  |
  v
Load kernel at 0x8000
  |
  v
Initialize GDT
  |
  v
Enable Protected Mode
  |
  v
Initialize 32-bit environment
  |
  v
Assembly kernel entry
  |
  v
kernel_main()
  |
  v
Kernel output and exception handling
```

## Project Structure

```text
Arceon-0.2/
├── boot/
│   ├── boot.asm
│   └── entry.asm
├── kernel/
│   ├── kernel.c
│   ├── idt.c
│   ├── idt.h
│   ├── isr.asm
│   └── exception.c
├── linker.ld
├── Makefile
├── tools-build.sh
└── README.md
```

## Requirements

The following tools are required to build and run Arceon from source:

- NASM — assembler
- GCC with 32-bit compilation support
- GNU Binutils — linker and binary utilities
- GNU Make
- QEMU — x86 system emulator

On Debian or Ubuntu-based systems, the typical packages include:

```bash
sudo apt update
sudo apt install nasm gcc binutils make qemu-system-x86
```

Depending on your distribution, additional multilib or 32-bit development packages may be required.

## Building from Source

Clone the repository:

```bash
git clone https://github.com/shridattadixit06/Arceon-0.2.git
cd Arceon-0.2
```

Build the disk image:

```bash
make
```

The build process assembles the bootloader and kernel entry code, compiles the freestanding C kernel, links the kernel, converts it to a raw binary, and constructs the disk image.

Expected build artifacts include:

- `boot.bin` — boot sector
- `kernel.elf` — linked kernel ELF
- `kernel.bin` — raw kernel binary
- `arceon.img` — raw bootable disk image

## Running Arceon

Launch the generated disk image in QEMU:

```bash
make run
```

Alternatively, run the image directly:

```bash
qemu-system-i386 -drive format=raw,file=arceon.img
```

Arceon is intended to be explored in a virtual machine. It is experimental and is not designed for everyday desktop use.

## Exception Testing

Arceon includes a Makefile target for building and running exception tests.

For example, to test an Invalid Opcode exception, use the existing test target with the corresponding exception configuration:

```bash
make test-exception EXCEPTION=6
```

Exception testing intentionally triggers a CPU exception. Follow the implementation's expected test procedure and inspect the resulting kernel output.

## Memory Layout

| Component | Address |
|---|---:|
| BIOS bootloader | `0x7C00` |
| Kernel load address | `0x8000` |
| 32-bit stack | `0x90000` |
| VGA text memory | `0xB8000` |

These addresses describe the current implementation's intended memory layout.

## Learning Objectives

Arceon is intended as a practical learning project for:

- x86 boot sequences and BIOS services
- Real Mode and Protected Mode
- GDTs, segmentation, and descriptor tables
- Kernel loading and linker scripts
- Freestanding C and assembly integration
- Interrupts and CPU exception handling
- VGA text-mode output
- Low-level debugging and emulation

## Roadmap

### Arceon 0.3 — Console and VGA Driver

Planned improvements include:

- Organizing VGA text output into a reusable driver interface
- Improving screen clearing, cursor handling, and formatted output
- Adding keyboard input and the foundation for an interactive console

### Future Development

Longer-term directions may include:

- Physical memory management
- A basic filesystem
- Kernel command interpreter
- Process management and system calls
- A native command-line shell

These are future goals and are not claimed as implemented features in Arceon 0.2.0.

## Release Downloads

Download the **Arceon 0.2.0** release from the repository's GitHub Releases page:

**[Arceon 0.2.0 Releases](https://github.com/shridattadixit06/Arceon-0.2/releases)**

The release provides a raw disk image for virtual-machine testing and a SHA-256 checksum for download verification.

## Limitations and Disclaimer

Arceon is an experimental educational operating system under active development.

- It is not intended for production or daily use.
- Hardware support is limited to the facilities currently implemented.
- Process management, a general-purpose filesystem, and a full user-space environment are not yet part of the documented feature set.
- Run Arceon in an emulator or virtual machine rather than on a primary physical system.

## License

See the repository's license file, if present, for the applicable terms governing use, modification, and redistribution.

---

**Arceon 0.2.0 — Exploring operating systems from the ground up.**
