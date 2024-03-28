// 1     0000              start:
// 2     0000 21 0A 00         ld hl, message  ; message address
// 3     0003 0E 00            ld c, 0         ; port number
// 4     0005 06 0D            ld b, 13        ; byte counter - length of message
// 5     0007 ED B3            otir
// 6     0009 76               halt
// 7     000A              message:
// 8     000A 48 65 6C 6C      .db "Hello World!\n"
// 8     000E 6F 20 57 6F
// 8     0012 72 6C 64 21
// 8     0016 0A
// 9     0017
byte MEM[] = { 
0x21, 0x0a, 0x00, 0x0e, 0x00, 0x06, 0x0d, 0xed,
0xb3, 0x76, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
0x57, 0x6f, 0x72, 0x6c, 0x64, 0x21, 0x0a
}; 