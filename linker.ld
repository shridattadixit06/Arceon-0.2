ENTRY(start)

SECTIONS
{
    . = 0x8000;

    .text :
    {
        *(.text*)
    }

    .rodata :
    {
        *(.rodata*)
    }

    .data :
    {
        *(.data*)
    }

    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start = .;

        *(.bss*)
        *(COMMON)

        . = ALIGN(4);
        __bss_end = .;
    }

    ASSERT(__bss_end < 0x90000,
           "Kernel BSS overlaps the stack")
}