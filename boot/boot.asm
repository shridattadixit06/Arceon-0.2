bits 16
org 0x7c00

CODE_SEG equ 0x08
DATA_SEG equ 0x10

start:
    cli

    ; Initialize real-mode segments
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00

    ; Save BIOS boot drive number
    mov [boot_drive], dl

    ; Load kernel from disk
    jmp load_kernel


; ----------------------------------------
; Load kernel from sector 2 to 0x8000
; ----------------------------------------

load_kernel:

    xor ax, ax
    mov es, ax

    mov bx, 0x8000

    mov ah, 0x02        ; BIOS read sectors
    mov al, 0x01        ; Read 1 sector
    mov ch, 0x00        ; Cylinder 0
    mov cl, 0x02        ; Sector 2
    mov dh, 0x00        ; Head 0
    mov dl, [boot_drive]

    int 0x13

    jc disk_error

    jmp setup_gdt


; ----------------------------------------
; Disk read error
; ----------------------------------------

disk_error:

    mov si, disk_error_message

disk_error_loop:

    mov ah, 0x0e
    lodsb

    cmp al, 0
    je real_mode_hang

    int 0x10
    jmp disk_error_loop

disk_error_message db 'Disk read error', 0


; ----------------------------------------
; Enter Protected Mode
; ----------------------------------------

setup_gdt:

    lgdt [gdt_descriptor]

    ; Enable Protected Mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to reload CS
    jmp CODE_SEG:protected_mode


; ----------------------------------------
; Data
; ----------------------------------------

boot_drive db 0


; ----------------------------------------
; Global Descriptor Table
; ----------------------------------------

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


; ----------------------------------------
; GDTR
; ----------------------------------------

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


; ----------------------------------------
; 32-bit Protected Mode
; ----------------------------------------

bits 32

protected_mode:

    ; Load data segment selector
    mov ax, DATA_SEG

    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set 32-bit stack
    mov esp, 0x90000

    ; Jump to loaded kernel
    jmp 0x8000


; ----------------------------------------
; Real-mode error hang
; ----------------------------------------

bits 16

real_mode_hang:

    cli

real_mode_hang_loop:
    hlt
    jmp real_mode_hang_loop


; ----------------------------------------
; Boot sector padding + signature
; ----------------------------------------

times 510-($-$$) db 0

dw 0xaa55