; **********************************************************************
; *  SIO Test
; **********************************************************************

BUFFER .equ 0x8000      ; start of RAM
STACK .equ 0xffff       ; this should really be 0x0000 as the CPU will dec SP before PUSH
UM245RS .equ 0x00
UM245RD .equ 0x01
SIOAD .equ 0x04
SIOBD .equ 0x05
SIOAC .equ 0x06
SIOBC .equ 0x07

start:
    ld sp, STACK
test:
    ld a,"Z"            ; test data
    out (SIOAD),a       ; send to SIO port A
    ld a,"_"            ; replace A with a different value
    in a,(SIOBD)        ; read from SIO port B
    out (UM245RD),a     ; send result back via USB - on success should be test data
    jr test             ; repeat
end:
    halt
prt_hex:                ; print contents of A as 2 digit hex to the port number in C
    ld b,a              ; copy value into B
    srl a               ; shift A right x4 e.g. transform 10110010 to 00001011
    srl a
    srl a
    srl a
    call prt_hex_dgt    ; most significant digit
    ld a,b              ; get the original copy back
    and %00001111       ; clears the top 4 bits
    call prt_hex_dgt    ; least significant digit
    ret
prt_hex_dgt:
    cp 0x0a             ; is it an alpha or numeric?
    jr c,prt_hex_n      ; numeric
prt_hex_a:              ; or drop through to alpha
    add a,"W"           ; for alpha add the base ascii for 'a' but then sub 10 as hex 'a' is 10d => 'W'
    out (c),a
    ret
prt_hex_n:
    add a,"0"           ; for numeric add the base ascii for '0'
    out (c),a
    ret