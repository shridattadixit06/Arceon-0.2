bits 16
org 0x7c00

start:

    mov ah, 0x0e

    mov si, message

print_loop:

    mov al, [si]

    cmp al, 0
    je hang

    int 0x10

    inc si
    jmp print_loop

hang:
    hlt
    jmp hang

message db 'Arceon', 0
times 510-($-$$) db 0
dw 0xaa55