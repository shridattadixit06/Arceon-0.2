#include <stdint.h>

#include "idt.h"
#include "pic.h"
#include "timer.h"

#ifndef TEST_EXCEPTION
#define TEST_EXCEPTION -1
#endif

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80

static volatile char *vga = (volatile char *)VGA_MEMORY;

static void vga_write_at(
    int row,
    int col,
    const char *text,
    uint8_t attribute
)
{
    while (*text)
    {
        int position = row * VGA_WIDTH + col;

        vga[position * 2] = *text++;
        vga[position * 2 + 1] = attribute;

        col++;
    }
}

static void display_ticks(uint32_t ticks)
{
    char digits[10];
    int count = 0;

    do
    {
        digits[count++] = '0' + (ticks % 10);
        ticks /= 10;
    }
    while (ticks != 0 && count < 10);

    vga_write_at(2, 0, "PIT ticks: ", 0x0F);

    for (int i = 0; i < 10; i++)
    {
        int position = 11 + i;

        if (i < count)
            vga[position * 2] = digits[count - 1 - i];
        else
            vga[position * 2] = ' ';

        vga[position * 2 + 1] = 0x0F;
    }
}

void kernel_main(void)
{
    /* Prevent hardware interrupts during initialization. */
    __asm__ volatile ("cli");

    const char *msg = "Arceon 0.2 - Protected Mode";

    for (int i = 0; i < 80 * 25; i++)
    {
        vga[i * 2] = ' ';
        vga[i * 2 + 1] = 0x07;
    }

    vga_write_at(0, 0, msg, 0x0F);

    /* Install CPU exception and hardware IRQ gates. */
    idt_init();

#if TEST_EXCEPTION == 6

    __asm__ volatile ("ud2");

#elif TEST_EXCEPTION == 0

    __asm__ volatile (
        "xor %%edx, %%edx\n"
        "mov $10, %%eax\n"
        "xor %%ecx, %%ecx\n"
        "div %%ecx\n"
        :
        :
        : "eax", "ecx", "edx"
    );

#endif

    /* Remap the PIC and mask every IRQ initially. */
    pic_remap();
    pic_disable();

    /* Configure the PIT to generate 100 ticks per second. */
    timer_init(100);

    /* Unmask only IRQ0, the PIT timer. */
    pic_clear_mask(0);

    /* All interrupt handlers and hardware are ready. */
    __asm__ volatile ("sti");

    while (1)
    {
        display_ticks(timer_get_ticks());

        __asm__ volatile ("hlt");
    }
}