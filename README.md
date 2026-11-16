
# Arceon 0.2.0

**Arceon** is an experimental educational x86 operating system built from scratch to explore low-level systems programming, computer architecture, and operating system internals.

Developed using C and x86 Assembly, Arceon explores how a machine transitions from BIOS boot into a freestanding kernel and how the kernel interacts with hardware through interrupts, timers, keyboard input, and VGA text output.

> **Status:** Experimental / Educational  
> **Version:** 0.2.0  
> **Architecture:** x86, 32-bit Protected Mode  
> **Languages:** C and NASM Assembly  
> **Emulator:** QEMU

---

## Overview

Arceon 0.2.0 builds on its initial bootable foundation by implementing a transition from **16-bit Real Mode to 32-bit Protected Mode**, loading a freestanding C kernel, and establishing the foundations of interrupt handling and hardware interaction.

The kernel includes an Interrupt Descriptor Table (IDT), exception handling, Programmable Interrupt Controller (PIC) configuration, IRQ handling, a Programmable Interval Timer (PIT), keyboard input, and a basic interactive console.

The project is designed as a hands-on exploration of operating system development, with an emphasis on understanding the mechanisms behind kernel initialization and hardware interaction.

## Features

### Bootloader and Kernel

- Custom BIOS bootloader written in NASM.
- Kernel loading through BIOS `INT 13h`.
- Kernel loaded at physical address `0x8000`.
- Global Descriptor Table (GDT) initialization.
- Transition from 16-bit Real Mode to 32-bit Protected Mode.
- 32-bit segment register and stack initialization.
- Assembly-to-C kernel entry point.
- Freestanding C kernel linked using a custom linker script.
- Explicit BSS section initialization before entering the C kernel.

### Interrupts and Exception Handling

- Interrupt Descriptor Table (IDT) implementation.
- Assembly interrupt and exception entry stubs.
- C-based exception handling.
- Exception handling for CPU-generated exceptions.
- Configurable exception-testing build target.
- Programmable Interrupt Controller (PIC) remapping.
- IRQ registration and dispatching.

### Hardware Interaction

- Programmable Interval Timer (PIT) initialization.
- Timer interrupt handling and tick counting.
- PS/2 keyboard input handling.
- Keyboard interrupt handling through IRQ dispatching.
- Direct VGA text-mode memory access.
- VGA text-output helper functions.

### Kernel Console

- Basic interactive kernel console.
- Keyboard-driven command input.
- Built-in commands:
  - `help` — Display available commands.
  - `about` — Display information about Arceon.
  - `ticks` — Display the timer tick count.
  - `clear` — Clear the console screen.

### Development and Build System

- Makefile-based build system.
- Automated bootloader assembly based on kernel size.
- Custom linker script.
- QEMU-based development and testing workflow.
- Configurable exception-testing builds.

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
Clear BSS section
 |
 v
kernel_main()
 |
 v
Initialize IDT and exception handling
 |
 v
Initialize PIC and IRQ handling
 |
 v
Initialize PIT and keyboard
 |
 v
Start interactive kernel console
```

## Project Structure

```text
Arceon-0.2/
├── boot/
│   ├── boot.asm
│   └── entry.asm
│
├── kernel/
│   ├── include/
│   │   ├── idt.h
│   │   ├── irq.h
│   │   ├── keyboard.h
│   │   ├── pic.h
│   │   └── timer.h
│   │
│   ├── src/
│   │   ├── exception.c
│   │   ├── idt.c
│   │   ├── irq.c
│   │   ├── keyboard.c
│   │   ├── pic.c
│   │   └── timer.c
│   │
│   ├── kernel.c
│   └── isr.asm
│
├── linker.ld
├── Makefile
├── tools-build.sh
└── README.md
```

## Requirements

The following tools are required to build and run Arceon from source:

- NASM — Assembler.
- GCC with 32-bit compilation support.
- GNU Binutils — Linker and binary utilities.
- GNU Make.
- QEMU — x86 system emulator.

On Debian or Ubuntu-based systems:

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

The build process assembles the bootloader and kernel entry code, compiles the freestanding C kernel, links the kernel using the custom linker script, converts the kernel ELF into a raw binary, and constructs the bootable disk image.

Expected build artifacts include:

- `boot.bin` — Boot sector.
- `kernel.elf` — Linked kernel ELF.
- `kernel.bin` — Raw kernel binary.
- `arceon.img` — Raw bootable disk image.

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

For example, to test an Invalid Opcode exception:

```bash
make test-exception EXCEPTION=6
```

To test a Divide Error exception:

```bash
make test-exception EXCEPTION=0
```

Exception testing intentionally triggers a CPU exception. These tests are intended to verify the kernel's exception-handling path and should be run in QEMU.

## Memory Layout

| Component | Address |
|---|---:|
| BIOS bootloader | `0x7C00` |
| Kernel load address | `0x8000` |
| 32-bit stack | `0x90000` |
| VGA text memory | `0xB8000` |

The kernel linker script defines the boundaries of the BSS section using `__bss_start` and `__bss_end`.

During startup, the assembly entry point clears this region before calling `kernel_main()`. This ensures that uninitialized global and static variables begin with zero-initialized storage, as required by the C execution environment.

The linker also checks that the BSS section does not overlap the reserved stack region.

These addresses describe the current implementation's intended memory layout.

## Learning Objectives

Arceon is intended as a practical learning project for exploring:

- x86 boot sequences and BIOS services.
- Real Mode and Protected Mode.
- GDTs, segmentation, and descriptor tables.
- Kernel loading and linker scripts.
- Freestanding C and assembly integration.
- BSS initialization and kernel startup.
- Interrupts and CPU exception handling.
- PIC configuration and IRQ dispatching.
- Timer interrupts and keyboard input.
- VGA text-mode output.
- Basic kernel console design.
- Low-level debugging and emulation.

## Roadmap

### Future Development

Potential areas of development include:

- Physical memory management.
- Paging and virtual memory.
- Kernel heap allocation.
- Process and thread management.
- User mode and kernel mode separation.
- System calls.
- Inter-process communication (IPC).
- Filesystem implementation.
- Kernel object and capability-based resource management.

These are future development goals and are not claimed as implemented features in Arceon 0.2.0.

## Release Downloads

Download the **Arceon 0.2.0** release from the repository's GitHub Releases page:

**[Arceon 0.2.0 Releases](https://github.com/shridattadixit06/Arceon-0.2/releases)**

The release provides a raw disk image for virtual-machine testing and a SHA-256 checksum for download verification.

## Limitations and Disclaimer

Arceon is an experimental educational operating system under active development.

- It is not intended for production or daily use.
- Hardware support is limited to the facilities currently implemented.
- It does not yet provide a general-purpose filesystem or user-space environment.
- Process management, virtual memory, and user-mode isolation are not yet implemented.
- Run Arceon in an emulator or virtual machine rather than on a primary physical system.

## License

See the repository's license file for the applicable terms governing use, modification, and redistribution.

---

**Arceon 0.2.0 — Exploring operating systems from the ground up.**
