#include <stdint.h>
#include "include/idt.h"
#include "include/pic.h"
#include "include/timer.h"
#include "include/keyboard.h"

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

    vga_write_at(0, 0, "Arceon 0.2", 0x0F);
    vga_write_at(4, 0, "Keyboard test:", 0x0B);

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
    keyboard_init();

    pic_clear_mask(0);

    __asm__ volatile ("sti");

    int cursor_row = 5;
    int cursor_col = 0;

    while (1) {
        display_ticks(timer_get_ticks());

        int c;

        while ((c = keyboard_getchar()) != -1) {
            if (c == '\n') {
                cursor_row++;
                cursor_col = 0;

                if (cursor_row >= VGA_HEIGHT) {
                    for (int row = 5; row < VGA_HEIGHT; row++) {
                        vga_clear_row(row);
                    }

                    cursor_row = 5;
                }
            }
            else if (c == '\b') {
                if (cursor_col > 0) {
                    cursor_col--;

                    int position =
                        cursor_row * VGA_WIDTH + cursor_col;

                    vga[position * 2] = ' ';
                    vga[position * 2 + 1] = 0x07;
                }
            }
            else if (c == '\t') {
                for (int i = 0; i < 4; i++) {
                    if (cursor_col >= VGA_WIDTH)
                        break;

                    int position =
                        cursor_row * VGA_WIDTH + cursor_col;

                    vga[position * 2] = ' ';
                    vga[position * 2 + 1] = 0x0F;

                    cursor_col++;
                }
            }
            else if (c >= 32 && c <= 126) {
                char character[2] = {(char)c, '\0'};

                vga_write_at(
                    cursor_row,
                    cursor_col,
                    character,
                    0x0F
                );

                cursor_col++;

                if (cursor_col >= VGA_WIDTH) {
                    cursor_col = 0;
                    cursor_row++;

                    if (cursor_row >= VGA_HEIGHT) {
                        for (int row = 5; row < VGA_HEIGHT; row++) {
                            vga_clear_row(row);
                        }

                        cursor_row = 5;
                    }
                }
            }
        }

        __asm__ volatile ("hlt");
    }
}