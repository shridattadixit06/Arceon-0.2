
void kernel_main(void)
{
    volatile char *vga = (volatile char *)0xB8000;
    const char *msg = "Arceon 0.2 - Protected Mode";

    for (int i = 0; i < 80 * 25; i++)
    {
        vga[i * 2] = ' ';
        vga[i * 2 + 1] = 0x07;
    }

    for (int i = 0; msg[i] != '\0'; i++)
    {
        vga[i * 2] = msg[i];
        vga[i * 2 + 1] = 0x0F;
    }

    while (1)
    {
        __asm__ volatile ("hlt");
    }
}
