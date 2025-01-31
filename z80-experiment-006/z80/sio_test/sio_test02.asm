; **********************************************************************
; *  SIO Test
; **********************************************************************

BUFFER .equ 0x8000      ; start of RAM
STACK .equ 0xffff       ; this should really be 0x0000 as the CPU will dec SP before PUSH

start:
    ld sp, STACK
    ld d,0
test:
    ld a,d              ; A is the value to print
    ld c,d              ; C is the port number we print to
    call prt_hex
    ld a, "\n"
    out (c), a
    inc d               ; next port number
    jr nz, test         ; loop until zero
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