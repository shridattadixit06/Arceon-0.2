void kernel_main()
{
    volatile char *vga = (volatile char *)0xB8000;

    vga[0] = 'K';
    vga[1] = 0x07;

    vga[2] = 'E';
    vga[3] = 0x07;

    vga[4] = 'R';
    vga[5] = 0x07;

    vga[6] = 'N';
    vga[7] = 0x07;

    vga[8] = 'E';
    vga[9] = 0x07;

    vga[10] = 'L';
    vga[11] = 0x07;

    while (1)
    {
    }
}