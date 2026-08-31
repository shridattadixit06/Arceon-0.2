bits 16
org 0x7c00

CODE_SEG equ 0x08
DATA_SEG equ 0x10

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

    ;Enabled Protected mode 

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ;farjump to 32bit Code

    jmp CODE_SEG:protected_mode


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

;32bit code starts here

bits 32
protected_mode:
    mov ax, DATA_SEG

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, 0x90000


    ;Write directly to VGA memory
    mov byte [0xB8000], 'W'
    mov byte [0xB8001], 0x07

    mov byte [0xB8002], 'e'
    mov byte [0xB8003], 0x07

    mov byte [0xB8004], 'l'
    mov byte [0xB8005], 0x07

    mov byte [0xB8006], 'c'
    mov byte [0xB8007], 0x07

    mov byte [0xB800A], 'o'
    mov byte [0xB800B], 0x07

    mov byte [0xB800C], 'm'
    mov byte [0xB800D], 0x07

    mov byte [0xB800E], 'e'
    mov byte [0xB800F], 0x07

hang:
    hlt
    jmp hang

times 510-($-$$) db 0
dw 0xaa55