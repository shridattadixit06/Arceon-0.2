#include "idt.h"

#define IDT_PRESENT       0x80
#define IDT_INTERRUPT_GATE 0x0E
extern void isr0(void);
static struct idt_entry idt[IDT_ENTRIES];

static struct idt_ptr idtr;

static void idt_set_gate(
    int vector,
    uint32_t handler_address
)
{
    idt[vector].offset_low =
        handler_address & 0xFFFF;

    idt[vector].selector = 0x08;

    idt[vector].zero = 0;

    idt[vector].type_attr =
        IDT_PRESENT | IDT_INTERRUPT_GATE;

    idt[vector].offset_high =
        (handler_address >> 16) & 0xFFFF;
}
void idt_init(void)
{
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)&idt;
    idt_set_gate(0, (uint32_t)isr0);
    __asm__ volatile (
        "lidt (%0)"
        :
        : "r" (&idtr)
    );
}