bits 16
org 0x7c00

CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    cli
    mov [boot_drive], dl
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
    je load_kernel

    int 0x10

    inc si
    jmp print_loop

load_kernel:

    xor ax, ax
    mov es, ax

    mov bx, 0x8000

    mov ah, 0x02
    mov al, 0x01
    mov ch, 0x00
    mov cl, 0x02
    mov dh, 0x00
    mov dl, [boot_drive]

    int 0x13

    jc disk_error

    jmp setup_gdt


disk_error:

    mov si, disk_error_message

disk_error_loop:

    mov ah, 0x0e
    lodsb

    cmp al, 0
    je hang

    int 0x10
    jmp disk_error_loop

disk_error_message db 'Disk read error', 0

setup_gdt:

    lgdt [gdt_descriptor]

    ;Enabled Protected mode 

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ;farjump to 32bit Code

    jmp CODE_SEG:protected_mode


message db 'Arceon 0.2', 0
message1 db 'Welcome to Arceon 0.2!',10, 0
message2 db 'Hey',0
boot_drive db 0

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
    mov edi, 0xB8000
    
    ; Current column
    xor ebx, ebx
    
    mov esi, message1
    call print_string

    mov esi, message2
    call print_string



hang:
    hlt
    jmp hang

print_string:
    mov al, [esi]

    cmp al, 0
    je print_d

    ; Newline check
    cmp al, 10
    je newline

    mov [edi], al
    mov byte [edi + 1], 0x07

    inc esi
    add edi, 2
    inc ebx

    jmp print_string

newline:

    ; Number of columns remaining
    mov eax, 80
    sub eax, ebx

    ; Convert columns to bytes
    shl eax, 1

    ; Move to beginning of next row
    add edi, eax

    ; Reset column
    xor ebx, ebx

    ; Move past newline character
    inc esi

    jmp print_string

print_d:
    ret


times 510-($-$$) db 0
dw 0xaa55