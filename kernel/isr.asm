bits 32

extern exception_handler

; Exceptions where the CPU does NOT push an error code.
%macro ISR_NOERR 1
global isr%1
isr%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

; Exceptions where the CPU ALREADY pushes an error code.
%macro ISR_ERR 1
global isr%1
isr%1:
    push dword %1
    jmp isr_common
%endmacro

; CPU exceptions: vectors 0–31

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR   8
ISR_NOERR 9
ISR_ERR   10
ISR_ERR   11
ISR_ERR   12
ISR_ERR   13
ISR_ERR   14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR   17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_ERR   21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_ERR   29
ISR_ERR   30
ISR_NOERR 31

isr_common:
    cli
    cld
    pushad

    ; Pass a pointer to the saved exception frame.
    mov eax, esp

    ; Align the stack for the C function call.
    and esp, 0xFFFFFFF0
    sub esp, 12

    push eax
    call exception_handler

.hang:
    cli
    hlt
    jmp .hang


; ----------------------------------------
; Hardware IRQ stubs: vectors 32–47
; ----------------------------------------

extern irq_handler

%macro IRQ_STUB 2
global irq%1
irq%1:
    push dword 0
    push dword %2
    jmp irq_common
%endmacro

IRQ_STUB 0, 32
IRQ_STUB 1, 33
IRQ_STUB 2, 34
IRQ_STUB 3, 35
IRQ_STUB 4, 36
IRQ_STUB 5, 37
IRQ_STUB 6, 38
IRQ_STUB 7, 39
IRQ_STUB 8, 40
IRQ_STUB 9, 41
IRQ_STUB 10, 42
IRQ_STUB 11, 43
IRQ_STUB 12, 44
IRQ_STUB 13, 45
IRQ_STUB 14, 46
IRQ_STUB 15, 47


irq_common:
    cld
    pushad

    ; Preserve the original saved-register frame.
    mov ebp, esp

    ; Align the stack for the C function call.
    and esp, 0xFFFFFFF0
    sub esp, 12

    ; Pass the IRQ vector to the C handler.
    push dword [ebp + 32]
    call irq_handler

    ; Restore the original saved-register frame.
    mov esp, ebp
    popad

    ; Discard the dummy error code and vector.
    add esp, 8

    ; Return to the interrupted code.
    iret

