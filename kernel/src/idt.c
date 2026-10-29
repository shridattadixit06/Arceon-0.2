#include "idt.h"

#define IDT_PRESENT        0x80
#define IDT_INTERRUPT_GATE 0x0E

extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

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
    void (*handlers[32])(void) = {
        isr0,  isr1,  isr2,  isr3,
        isr4,  isr5,  isr6,  isr7,
        isr8,  isr9,  isr10, isr11,
        isr12, isr13, isr14, isr15,
        isr16, isr17, isr18, isr19,
        isr20, isr21, isr22, isr23,
        isr24, isr25, isr26, isr27,
        isr28, isr29, isr30, isr31
    };
    void (*irq_handlers[16])(void) = {
        irq0,  irq1,  irq2,  irq3,
        irq4,  irq5,  irq6,  irq7,
        irq8,  irq9,  irq10, irq11,
        irq12, irq13, irq14, irq15
    };
    /* Clear all 256 IDT entries first. */
    for (int i = 0; i < IDT_ENTRIES; i++)
    {
        idt[i].offset_low = 0;
        idt[i].selector = 0;
        idt[i].zero = 0;
        idt[i].type_attr = 0;
        idt[i].offset_high = 0;
    }

    /* Register CPU exception handlers 0–31. */
    for (int i = 0; i < 32; i++)
    {
        idt_set_gate(i, (uint32_t)handlers[i]);
    }
    
    /* Register hardware IRQ handlers 32–47. */
    for (int i = 0; i < 16; i++)
    {
        idt_set_gate(32 + i, (uint32_t)irq_handlers[i]);
    }
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint32_t)idt;

    __asm__ volatile (
        "lidt (%0)"
        :
        : "r" (&idtr)
        : "memory"
    );
}