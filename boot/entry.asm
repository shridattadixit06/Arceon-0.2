bits 32

global start
extern kernel_main
extern __bss_start
extern __bss_end

start:
    ; Clear the BSS section before entering C.
    cld
    mov edi, __bss_start
    mov ecx, __bss_end
    sub ecx, edi
    xor eax, eax
    rep stosb

    ; Enter the C kernel.
    call kernel_main

hang:
    cli
    hlt
    jmp hang