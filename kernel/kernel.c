#include "idt.h"
#ifndef TEST_EXCEPTION
#define TEST_EXCEPTION -1
#endif
void kernel_main(void)
{
    volatile char *vga = (volatile char *)0xB8000;
    const char *msg = "Arceon 0.2 - Protected Mode";

    /* Clear the VGA text screen. */
    for (int i = 0; i < 80 * 25; i++)
    {
        vga[i * 2] = ' ';
        vga[i * 2 + 1] = 0x07;
    }

    /* Display the boot message. */
    for (int i = 0; msg[i] != '\0'; i++)
    {
        vga[i * 2] = msg[i];
        vga[i * 2 + 1] = 0x0F;
    }

    /* Install the CPU exception handlers. */
    idt_init();
    #if TEST_EXCEPTION == 6

        /* Test: Invalid Opcode (#UD) */
        __asm__ volatile ("ud2");

    #elif TEST_EXCEPTION == 0

        /* Test: Divide Error (#DE) */
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
    /* Keep the kernel running. */
    while (1)
    {
        __asm__ volatile ("hlt");
    }
}