#include <stdint.h>

struct exception_frame
{
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t saved_esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t vector;
    uint32_t error_code;
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
};

static volatile uint16_t *vga =
    (volatile uint16_t *)0xB8000;

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

static int cursor;

static void write_string(const char *text)
{
    while (*text)
    {
        vga[cursor++] = (uint16_t)*text++ | 0x0C00;
    }
}

static void write_hex(uint32_t value)
{
    const char digits[] = "0123456789ABCDEF";

    for (int shift = 28; shift >= 0; shift -= 4)
    {
        vga[cursor++] =
            (uint16_t)digits[(value >> shift) & 0xF]
            | 0x0C00;
    }
}

void exception_handler(struct exception_frame *frame)
{
    cursor = 0;
    for (int i = 0; i < 80 * 25; i++)
    {
        vga[i] = (uint16_t)' ' | 0x0700;
    }
    write_string("ARCEON EXCEPTION");
    cursor = 80;

    write_string("Vector: ");
    write_hex(frame->vector);

    cursor = 160;
    write_string("Name: ");

    if (frame->vector < 32)
        write_string(exception_names[frame->vector]);
    else
        write_string("Unknown");

    cursor = 240;
    write_string("EIP: 0x");
    write_hex(frame->eip);

    cursor = 320;
    write_string("Error: 0x");
    write_hex(frame->error_code);

    for (;;)
    {
        __asm__ volatile ("cli; hlt");
    }
}