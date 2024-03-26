SER_CTR EQU 0x00    ; serial control register address
SER_DATA EQU 0x01   ; serial data register address

start:
    in a,SER_CTR    ; read the serial control register
    bit 0,a         ; check bit 0 - receive character available
    jp z,start      ; if zero (nothing to read) then loop back
    in a,SER_DATA   ; read a character
    out SER_DATA,a  ; write the character
    jp start        ; repeat
