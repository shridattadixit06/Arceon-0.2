bits 16
org 0x7c00

start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    mov ah, 0x0e
    mov si, message

print_loop:
    mov al, [si]

    cmp al, 0
    je setup_gdt

    int 0x10

    inc si
    jmp print_loop


setup_gdt:

    lgdt [gdt_descriptor]

hang:
    hlt
    jmp hang


message db 'Arceon 0.2', 0


gdt_start:

    ; Null descriptor
    dq 0


    ; Code segment
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00


    ; Data segment
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x92
    db 0xCF
    db 0x00


gdt_end:


gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


times 510-($-$$) db 0
dw 0xaa55