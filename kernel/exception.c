#include <stdint.h>

static volatile char *vga = (volatile char *)0xB8000;

void exception_handler(void)
{
    const char *msg = "ARCEON: CPU EXCEPTION";

    for (int i = 0; msg[i] != '\0'; i++)
    {
        vga[i * 2] = msg[i];
        vga[i * 2 + 1] = 0x0C;
    }

    for (;;)
    {
        __asm__ volatile ("cli; hlt");
    }
}
