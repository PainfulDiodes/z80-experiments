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
// Note that we are using the clock to trigger an interrupt. Arduino Mega will handle interrupts only on 2, 3, 18, 19, 20, 21 
// and 20-21 have special considerations - https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
const char CLK =     2; // Z80 6  Input

// Mock Memory
byte MEM[] = {
  0b00111110, // 0x3e LDA,n 
  0xa5,
  0b00111100, // 0x3c INC A 
  0b00110010, // 0x32 LD (nn), A
  0x0F, 
  0x00, 
  0x76,       //      HALT
  0x00,
  0x00, 
  0x00, 
  0x00, 
  0x00,
  0x00, 
  0x00, 
  0x00, 
  0x00,
  0x00, 
  0x00, 
  0x00, 
  0x00,
  0x00, 
  0x00, 
  0x00, 
  0x00,
  0x00,
  0x00,
  0x00, 
  0x00, 
  0x00, 
  0x00 
}; 
const unsigned int mem_size = 32;

// internal Arduino state 
String cache = "";
volatile int resetCycles = -1; // where are we in a reset cycle? special value -1 = we are not in a reset cycle

void setup() {
  Serial.begin(9600);
  // digital pins default to input mode, and so we don't need to set status for every pin https://docs.arduino.cc/learn/microcontrollers/digital-pins/

  // initiate a Z80 reset
  digitalWrite(RESET, LOW);
  pinMode(RESET, OUTPUT); 
  resetCycles = 0; // begin the reset process (requires multiple clock cycles)
    
  // a CHANGE interrupt on the clock means we are triggering on half clock cycles (rising and falling edges)
  attachInterrupt(digitalPinToInterrupt(CLK), clockInterrupt, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(RD), readInterrupt, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(WR), writeInterrupt, CHANGE); 
}

void loop() { // loop is needed only to flush the message cache - the work is done in interrupts
  if(cache.length()>0) {
    Serial.print(cache);
    cache="";
  }
} 

void clockInterrupt() { // interrupt handler

  if(digitalRead(HALT)==LOW) return; // HALT is active low

  writeLog(digitalRead(CLK)==LOW ? "CLK:0" : "CLK:1");
  writeLog(digitalRead(RFSH)==LOW ? "RFSH" : "....");
  writeLog(digitalRead(M1)==LOW   ? "M1" : "..");
  writeLog(digitalRead(RD)==LOW   ? "RD" : "..");
  writeLog(digitalRead(WR)==LOW   ? "WR" : "..");
  writeLog(digitalRead(MREQ)==LOW ? "MREQ" : "....");

  // build a number from the address bits
  unsigned int a = 0;
  for(int i=0; i<16; i++) a = bitWrite(a,i,digitalRead(A[i]));

  char s[20];
  sprintf(s,"A:%04x",a);
  writeLog(s);

  // build a number from the data bits
  byte d = 0;
  for(int i=0; i<8; i++) d = bitWrite(d,i,digitalRead(D[i]));

  sprintf(s,"D:%02x",d);
  writeLog(s);

  // process a reset - requires multiple clock cycles so handling here in the clock interrupt
  if(resetCycles>=0) { // -1 means we're not resetting, anything else means we're reseting for a count of clock cycles
    writeLog("Resetting...");
    if(++resetCycles>6) { // reset is complete (requires at least 3 full clock cycles, and we interrupt on a half cycle)
      writeLog("DONE");
      resetCycles=-1; // set to indicate a reset is not active
      pinMode(RESET, INPUT); // high impedance state, mimicing an open-collector output - the Z80 pin will be high by its pullup resistor - so this sets RESET high
    }
  }

  writeLogLn();
}

void readInterrupt() {
  if(digitalRead(HALT)==LOW) return; // HALT is active low - do nothing when halted

  if(digitalRead(RD)==LOW) { // we are here because RD has changed - if RD is LOW then it has just become LOW - a falling edge and so the beginning of a read
    if(digitalRead(MREQ)==LOW) { // this is a read of memory
      for(int i=0; i<8; i++) pinMode(D[i], OUTPUT); // switch the data pins to output mode
      unsigned int a=0;
      for(int i=0; i<16; i++) a = bitWrite(a,i,digitalRead(A[i])); // get the address as a number
      byte d;
      if(a<mem_size) { 
        d = MEM[a];
      }
      else {
        d = 0x00;
      }
      for(int i=0; i<8; i++) digitalWrite(D[i],bitRead(d,i)); // put the data from memory on the bus, bit by bit
    }
  }
  else { // RD is HIGH and has just become HIGH - rising edge and so the end of the read process
    for(int i=0; i<8; i++) pinMode(D[i], INPUT); // switch the pins back to input mode
  }
}

void writeInterrupt() {
  if(digitalRead(HALT)==LOW) return; // HALT is active low - do nothing when halted


  if(digitalRead(WR)==LOW) { // we are here because WR has changed - if WR is LOW then it has just become LOW - a falling edge and so the beginning of a write
    for(int i=0; i<8; i++) pinMode(D[i], INPUT); // switch the data pins to input mode
  }
  else { // WR rising edge - this is when we will capture the data that the Z80 has put on the bus
    unsigned int a = 0;
    for(int i=0; i<16; i++) a = bitWrite(a,i,digitalRead(A[i]));
    if(a<mem_size) {
      byte d = 0;
      for(int i=0; i<8; i++) d = bitWrite(d,i,digitalRead(D[i]));
      MEM[a] = d;
    }
  }
}

void writeLog(String s) {
  cache += String(s);
  cache += String(' ');
}

void writeLogLn() {
  cache += String('\n');
}
