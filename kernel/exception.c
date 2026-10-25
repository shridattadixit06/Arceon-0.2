#include <stdint.h>

static volatile char *vga = (volatile char *)0xB8000;

static const char *exception_names[32] = {
    "Divide Error",
    "Debug",
    "NMI",
    "Breakpoint",
    "Overflow",
    "BOUND Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved"
};

void exception_handler(uint32_t vector)
{
    const char *prefix = "ARCEON EXCEPTION: ";
    int pos = 0;

    for (int i = 0; prefix[i] != '\0'; i++)
    {
        vga[pos * 2] = prefix[i];
        vga[pos * 2 + 1] = 0x0C;
        pos++;
    }

    const char *name =
        vector < 32 ? exception_names[vector] : "Unknown";

    for (int i = 0; name[i] != '\0'; i++)
    {
        vga[pos * 2] = name[i];
        vga[pos * 2 + 1] = 0x0C;
        pos++;
    }

    for (;;)
    {
        __asm__ volatile ("cli; hlt");
    }
}