; **********************************************************************
; *  SIO Test
; **********************************************************************

BUFFER .equ 0x8000      ; start of RAM
STACK .equ 0xffff       ; this should really be 0x0000 as the CPU will dec SP before PUSH
UM345R_S .equ 0x00
UM345R_D .equ 0x01
SIO_A_D .equ 0x04
SIO_B_D .equ 0x05
SIO_A_C .equ 0x06
SIO_B_C .equ 0x07

start:
    ld sp, STACK
    ld hl,start_msg
    call(prt_str)
init:
    ld a,%00011000      ; channel reset
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,4              ; pointer to WR4
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,%11000111      ; clock rate (2), 0, 0, stop bits (2), parity type, parity
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,1              ; pointer to WR1
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,%00000000      ; interrupts etc
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,3              ; pointer to WR3
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,%11100001      ; rx bits (2), auto enables, 0, 0, 0, 0, rx enable
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,5              ; pointer to WR5
    out (SIO_A_C),a
    out (SIO_B_C),a
    ld a,%11101000      ; DTR, tx bits (2), send break, tx enable, 0, RTS, 0
    out (SIO_A_C),a
    out (SIO_B_C),a
flush_B_input:
    ld a,0
    out (SIO_B_C),a     ; Pointer to RR0 for channel B
    in a,(SIO_B_C)      ; read status for channel B
    bit 0,a             ; check bit 0 - receive character available
    jr z,input          ; continue if nothing to read
    in a,(SIO_B_D)      ; otherwise read data received
    jr flush_B_input    ; and repeat
input:
    in a,(UM345R_S)     ; read the USB status
    bit 0,a             ; check bit 0 - receive character available if zero
    jp nz,receive_data  ; jump ahead if nothing to read
    in a,(UM345R_D)     ; read a character
echo_input:
    ld b,a
    ld a,"|"
    out (UM345R_D),a
    ld a,b
    out (UM345R_D),a
send_data:
    out (SIO_A_D),a     ; send data on channel A
receive_data:
    ld a,0
    out (SIO_B_C),a     ; Pointer to RR0 for channel B
    in a,(SIO_B_C)      ; read status for channel B
    bit 0,a             ; check bit 0 - receive character available
    jr z,input          ; loop back if nothing to read
    in a,(SIO_B_D)      ; otherwise read data received
print_rec_data:
    ld b,a
    ld a,"+"
    out (UM345R_D),a
    ld a,b
    out (UM345R_D),a    ; print data USB
    jr input            ; and go back to start
end:
    halt                ; never get here
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
    out (UM345R_D),a
    ret
prt_hex_n:
    add a,"0"           ; for numeric add the base ascii for '0'
    out (UM345R_D),a
    ret
prt_str:                ; print a zero-terminated string, start pointed to by hl
    ld a,(hl)
    cp 0h
    ret z
    out(UM345R_D),a 
    inc hl
    jr prt_str
start_msg   .db "sio_test11\n",0