#include "irq.h"
#include "pic.h"

#define IRQ_COUNT 16
#define IRQ_VECTOR_BASE 32

static irq_callback_t irq_callbacks[IRQ_COUNT] = {0};

void irq_register_handler(uint8_t irq, irq_callback_t callback)
{
    if (irq >= IRQ_COUNT)
        return;

    irq_callbacks[irq] = callback;
}

void irq_handler(uint32_t vector)
{
    if (vector < IRQ_VECTOR_BASE ||
        vector >= IRQ_VECTOR_BASE + IRQ_COUNT)
    {
        return;
    }

    uint8_t irq = vector - IRQ_VECTOR_BASE;

    if (irq_callbacks[irq] != 0)
    {
        irq_callbacks[irq]();
    }

    pic_send_eoi(irq);
}