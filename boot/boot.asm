;boot.asm
    mov ax, 0x07c0
    mov ds, ax
    
    xor bx, bx ; bx = 0
    mov es, bx ; Destination segment = 0
    ;mov bx, 0x7e00 ; Right after the bootsector in ram
    mov bx, 0x8000 ; Try something else

    mov dl, 0x00 ; Drive (A:)
    mov al, 0x01 ; Amount of sectors to read
    mov ch, 0x00 ; Cylinder
    mov cl, 0x02 ; Sector (starts at 1)
    mov dh, 0x00 ; Head
    
    mov ah, 0x02 ; Read sector from drive
    int 0x13

;    mov si, msg
;    cld
;
;ch_loop:
;    lodsb
;    or al, al ; if (al == 0)
;    jz hang   ;     goto hang
;    mov ah, 0x0E
;    mov bh, 0
;    int 0x10
;    jmp ch_loop
 
hang:
    jmp hang

;;msg db 'Hello World', 13, 10, 0

    times 510-($-$$) db 0
    db 0x55
    db 0xAA
