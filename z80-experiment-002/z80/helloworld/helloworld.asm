start:
    ld hl, message  ; message address
    ld c, 0         ; port number
    ld b, 13        ; byte counter - length of message
    otir
    halt
message:
    .db "Hello World!\n"
