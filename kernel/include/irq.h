#ifndef IRQ_H
#define IRQ_H

#include <stdint.h>

typedef void (*irq_callback_t)(void);

void irq_register_handler(uint8_t irq, irq_callback_t callback);
void irq_handler(uint32_t vector);

#endif