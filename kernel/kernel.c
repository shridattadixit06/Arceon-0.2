#include "include/idt.h"
#include "include/pic.h"
#include "include/timer.h"
#include "include/keyboard.h"
#include "include/shell.h"

#ifndef TEST_EXCEPTION
#define TEST_EXCEPTION -1
#endif

void kernel_main(void)
{
    __asm__ volatile("cli");

    idt_init();

#if TEST_EXCEPTION == 6
    __asm__ volatile("ud2");
#elif TEST_EXCEPTION == 0
    __asm__ volatile(
        "xor %%edx, %%edx\n"
        "mov $10, %%eax\n"
        "xor %%ecx, %%ecx\n"
        "div %%ecx\n"
        :
        :
        : "eax", "ecx", "edx");
#endif

    pic_remap();
    pic_disable();

    timer_init(100);
    keyboard_init();

    pic_clear_mask(0);

    shell_init();

    __asm__ volatile("sti");

    while (1)
    {
        shell_update_ticks();
        shell_update();

        __asm__ volatile("hlt");
    }
}