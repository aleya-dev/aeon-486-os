[bits 16]
[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; Kernel offset address
KERNEL_NUM_SECTORS equ 16 ; 1 sector = 512bytes; max 0x80
KERNEL_STACK_ADR equ 0x90000

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Bootsector begin
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov [BOOT_DRIVE], dl ; Store the boot drive

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Set up a stack
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov bp, 0x9000
    mov sp, bp

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Load the kernel
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    mov bx, KRN_LOADING
    call print_ln

    mov dl, [BOOT_DRIVE]

    mov ah, 0x02 ; int 0x13 mode: Read sector from drive
    mov al, KERNEL_NUM_SECTORS
    mov cl, 0x02 ; Sector (starts at 1)
    mov ch, 0x00 ; Cylinder
    mov dh, 0x00 ; Head

    xor bx, bx ; bx = 0
    mov es, bx ; Destination segment = 0
    mov bx, KERNEL_OFFSET

    int 0x13

    jc disk_error ; if error, goto disk_error

    cmp al, KERNEL_NUM_SECTORS ; if (sectors_read == KERNEL_NUM_SECTORS)
    je disk_success            ;    goto disk_success

disk_error:
    mov bx, KRN_DISK_ERROR
    call print_ln
    jmp hang

disk_success:
    mov bx, KRN_LOADED
    call print_ln

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Enter protected mode
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    cli ; 1. disable interrupts
    lgdt [gdt_descriptor] ; 2. load the GDT descriptor
    mov eax, cr0
    or eax, 0x1 ; 3. set 32-bit mode bit in cr0
    mov cr0, eax
    jmp CODE_SEG:protected_mode_entry ; 4. far jump by using a different segment

[bits 32]
protected_mode_entry:
    mov ax, DATA_SEG ; 5. update the segment registers
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Set up a stack
    mov ebp, KERNEL_STACK_ADR
    mov esp, ebp

    call KERNEL_OFFSET

    ; Endless loop if the kernel ever closes
hang:
    jmp hang

[bits 16]
print_ln:
    pusha

print_ln_start:
    ; char *bx = "the string";
    mov al, [bx]        ; al = *bx
    cmp al, 0           ; if al == '\0'
    je print_ln_done    ;   goto print_ln_done

    mov ah, 0x0e        ; Bios print character
    int 0x10

    add bx, 1           ; ++bx
    jmp print_ln_start

print_ln_done:
    mov al, 0x0a ; '\n'
    int 0x10
    mov al, 0x0d ; '\r'
    int 0x10

    popa
    ret

BOOT_DRIVE db 0

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; Set up GDT
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
gdt_start: ; don't remove the labels, they're needed to compute sizes and jumps
    ; the GDT starts with a null 8-byte
    dd 0x0 ; 4 byte
    dd 0x0 ; 4 byte

; GDT for code segment. base = 0x00000000, length = 0xfffff
; for flags, refer to os-dev.pdf document, page 36
gdt_code:
    dw 0xffff    ; segment length, bits 0-15
    dw 0x0       ; segment base, bits 0-15
    db 0x0       ; segment base, bits 16-23
    db 10011010b ; flags (8 bits)
    db 11001111b ; flags (4 bits) + segment length, bits 16-19
    db 0x0       ; segment base, bits 24-31

; GDT for data segment. base and length identical to code segment
; some flags changed, again, refer to os-dev.pdf
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

; GDT descriptor
gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size (16 bit), always one less of its true size
    dd gdt_start ; address (32 bit)

; define some constants for later use
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

KRN_LOADING: db "[AEON] Loading kernel...", 0
KRN_DISK_ERROR: db "[AEON] Disk error reading kernel", 0
KRN_LOADED: db "[AEON] OK. Entering protected mode."

times 510-($-$$) db 0
db 0x55
db 0xAA
