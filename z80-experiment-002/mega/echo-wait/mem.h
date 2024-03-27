//  1    0000              SER_CTR EQU 0x00    ; serial control register address
//  2    0000              SER_DATA EQU 0x01   ; serial data register address
//  3    0000
//  4    0000              start:
//  5    0000 DB 00            in a,SER_CTR    ; read the serial control register
//  6    0002 CB 47            bit 0,a         ; check bit 0 - receive character available
//  7    0004 CA 00 00         jp z,start      ; if zero (nothing to read) then loop back
//  8    0007 DB 01            in a,SER_DATA   ; read a character
//  9    0009 D3 01            out SER_DATA,a  ; write the character
// 10    000B C3 00 00         jp start        ; repeat
// 11    000E
byte MEM[] = { 
0xdb, 0x00, 0xcb, 0x47, 0xca, 0x00, 0x00, 0xdb,
0x01, 0xd3, 0x01, 0xc3, 0x00, 0x00
}; 
