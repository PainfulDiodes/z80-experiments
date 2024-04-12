// Define constants for the Z80 pins and the Arduino Mega pins to which they are connected

// Address Bus
const char A[] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37}; // Z80 30-40,1-5 Output

// Data Bus
const char D[] = {38,39,40,41,42,43,44,45}; // Z80 14,15,12,8,7,9,10,13 Input/Output

// System Control
const char M1 =     46; // Z80 27 Output
const char MREQ =   47; // Z80 19 Output
const char IORQ =  48; // Z80 20 Output
const char RFSH =   49; // Z80 28 Output

// System Control - used for interrupts
const char RD =     18; // Z80 21 Output
const char WR =     19; // Z80 22 Output

// CPU Control
const char HALT =   50; // Z80 18 Output
const char WAIT =   51; // Z80 24 Input
// const char INT =     4; // Z80 16 Input // not currently needed but planning out the pins
// const char NMI =     5; // Z80 17 Input // not currently needed but planning out the pins
const char RESET =   6; // Z80 26 Input

// CPU Bus Control
// const char BUSRQ =   7; // Z80 25 Input // not currently needed but planning out the pins
// const char BUSACK =  8; // Z80 23 Output // not currently needed but planning out the pins

// Clock
const char CLK =     2; // Z80 6  Input