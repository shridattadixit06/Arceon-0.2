#include "timer.h"
#include <stdint.h>

#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_INPUT_FREQUENCY 1193182u

static volatile uint32_t ticks = 0;

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile (
        "outb %0, %1"
        :
        : "a"(value), "Nd"(port)
    );
}

static void timer_callback(void)
{
    ticks++;
}

void timer_init(uint32_t frequency)
{
    if (frequency == 0)
        return;

    uint32_t divisor = PIT_INPUT_FREQUENCY / frequency;

    if (divisor == 0)
        divisor = 1;

    if (divisor > 65535)
        divisor = 65535;

    /* Register our callback for hardware IRQ0. */
    extern void irq_register_handler(
        uint8_t irq,
        void (*callback)(void)
    );

    irq_register_handler(0, timer_callback);

    /* Channel 0, low byte/high byte, square-wave mode. */
    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}

uint32_t timer_get_ticks(void)
{
    return ticks;
}