bits 32

global isr0
extern exception_handler

isr0:
    cli
    pushad

    call exception_handler

.hang:
    hlt
    jmp .hang
