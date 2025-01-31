; **********************************************************************
; *  SIO Test
; **********************************************************************

SIO_CTRL .equ 0         ; serial control register address
SIO_DATA .equ 1         ; serial data register address
BUFFER .equ 0x8000      ; start of RAM
STACK .equ 0xffff       ; this should really be 0x0000 as the CPU will dec SP before PUSH

start:
    ld sp, STACK
testout:
    ld a,"Z"
    out (SIO_DATA),a 
end:
    halt