#include <stdint.h>
#include "include/idt.h"
#include "include/pic.h"
#include "include/timer.h"

#ifndef TEST_EXCEPTION
#define TEST_EXCEPTION -1
#endif

#define VGA_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static volatile char *vga = (volatile char *)VGA_MEMORY;

static void vga_write_at(int row, int col, const char *text, uint8_t attribute)
{
    while (*text && col < VGA_WIDTH) {
        int position = row * VGA_WIDTH + col;
        vga[position * 2] = *text++;
        vga[position * 2 + 1] = attribute;
        col++;
    }
}

static void vga_clear_row(int row)
{
    for (int col = 0; col < VGA_WIDTH; col++) {
        int position = row * VGA_WIDTH + col;
        vga[position * 2] = ' ';
        vga[position * 2 + 1] = 0x07;
    }
}

static void display_ticks(uint32_t ticks)
{
    char digits[10];
    int count = 0;

    do {
        digits[count++] = '0' + (ticks % 10);
        ticks /= 10;
    } while (ticks != 0 && count < 10);

    vga_write_at(2, 0, "PIT ticks: ", 0x0F);

    for (int i = 0; i < 10; i++) {
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
    __asm__ volatile ("cli");

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga[i * 2] = ' ';
        vga[i * 2 + 1] = 0x07;
    }

    vga_clear_row(0);
    vga_write_at(0, 0, "Arceon 0.2", 0x0F);

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

    pic_remap();
    pic_disable();

    timer_init(100);
    pic_clear_mask(0);

    __asm__ volatile ("sti");

    while (1) {
        display_ticks(timer_get_ticks());
        __asm__ volatile ("hlt");
    }
}