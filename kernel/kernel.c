void kernel_main()
{
    volatile char *vga = (volatile char *)0xB8000;
    volatile char *msg = "KERNEL has started!";
    for (int i = 0; i < 80 * 25 * 2; i++)
    {
        vga[i] = 0;
    }

    for (int i = 0; i < 24; i++)
    {
        vga[i * 2] = msg[i];
        vga[i * 2 + 1] = 0x07;
    }
    while (1)
    {
    }
}