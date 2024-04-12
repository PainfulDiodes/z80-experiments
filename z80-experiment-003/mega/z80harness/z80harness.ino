#include "z80-mapping.h"

byte MEM_BUFFER[256];
byte MEM_STACK[256];

const unsigned int LOW_RAM =     0x8000;
const unsigned int HIGH_RAM =    0xffff;
const unsigned int LOW_BUFFER =  0x8000;
const unsigned int HIGH_BUFFER = 0x80ff;
const unsigned int LOW_STACK =   0xff00;
const unsigned int HIGH_STACK =  0xffff;

const unsigned int SER_CTR =  0;
const unsigned int SER_DATA = 1;

// internal Arduino state 
volatile bool isIORQ;

bool LOGGING = false;

void setup() {
  Serial.begin(9600);
  // digital pins default to input mode, and so we don't need to set status for every pin https://docs.arduino.cc/learn/microcontrollers/digital-pins/

  pinMode(CLK, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(RD), readInterrupt, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(WR), writeInterrupt, CHANGE); 
}

void loop() {
  digitalWrite(CLK,0);
  digitalWrite(CLK,1);
}


void readInterrupt() {
  // we are here because RD has changed
  if(digitalRead(RD)==LOW) { // if RD is LOW then it has just become LOW - a falling edge and so the beginning of a read
    if(digitalRead(IORQ)==LOW) readStartIO(); // read of IO
    if(digitalRead(MREQ)==LOW) readStartMem(); // this is a read of memory
    writeLogBusStatus();
    writeLogLn(); 
  } 
  else { // if RD is HIGH it has just become HIGH - rising edge and so the end of the read process
    for(int i=0; i<8; i++) pinMode(D[i], INPUT); // switch the pins back to input mode
  }
}

void readStartIO() {
  unsigned int a=0;
  byte d = 0x00;

  for(int i=0; i<8; i++) pinMode(D[i], OUTPUT); // switch the data pins to output mode
  
  for(int i=0; i<8; i++) bitWrite(a,i,digitalRead(A[i])); // get the address as a number from the bottom 8 bits
  
  if(a==SER_CTR) { // simulated control register - all we want to return is whether there is data in the buffer
    bitWrite(d,0,(Serial.available()>0));
  }
  
  if(a==SER_DATA) {
    int i = Serial.read();
    d = i < 0 ? 0 : i;
  }
  
  for(int i=0; i<8; i++) digitalWrite(D[i],bitRead(d,i)); // put the data from memory on the bus, bit by bit
}

void readStartMem() {
  unsigned int a=0;
  byte d = 0x00;

  for(int i=0; i<16; i++) bitWrite(a,i,digitalRead(A[i])); // get the address as a number

  if(a<= HIGH_RAM && a>= LOW_RAM ) { // is this a RAM request? - in reality could just check A15

    for(int i=0; i<8; i++) pinMode(D[i], OUTPUT); // switch the data pins to output mode

    if(a<= HIGH_BUFFER && a>= LOW_BUFFER ) { 
      d = MEM_BUFFER[a-LOW_BUFFER];
    }

    if(a<= HIGH_STACK && a>= LOW_STACK ) { 
      d = MEM_STACK[a-LOW_STACK];
    }

    for(int i=0; i<8; i++) digitalWrite(D[i],bitRead(d,i)); // put the data from memory on the bus, bit by bit
  }
}

void writeInterrupt() {
  // we are here because WR has changed
  if(digitalRead(WR)==LOW) { //  if WR is LOW then it has just become LOW - a falling edge and so the beginning of a write
    for(int i=0; i<8; i++) pinMode(D[i], INPUT);  // switch the data pins to input mode
    isIORQ = (digitalRead(IORQ)==LOW);            // set the flag that we're writing to an IO - will capture on the rising edge
  }
  else { // otherwise WR rising edge - this is when we will capture the data that the Z80 has put on the bus
    if(isIORQ) writeEndIO();
    else writeEndMem();
  }

  writeLogBusStatus();
}

byte writeEndIO() {
    byte d = 0;
    for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
    writeOut(String((char)d));
    writeLogLn();
}

void writeEndMem() {
  unsigned int a = 0;
  byte d = 0;

  for(int i=0; i<16; i++) bitWrite(a,i,digitalRead(A[i]));

  if(a<= HIGH_BUFFER && a>= LOW_BUFFER ) { 
    for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
    MEM_BUFFER[a-LOW_BUFFER] = d;
  }

  if(a<= HIGH_STACK && a>= LOW_STACK ) { 
    for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
    MEM_STACK[a-LOW_STACK] = d;
  }
}

void writeLogBusStatus() {
  if(LOGGING) {
    writeLog(digitalRead(M1)==LOW   ? "M1" : "..");
    writeLog(digitalRead(RD)==LOW   ? "RD" : "..");
    writeLog(digitalRead(WR)==LOW   ? "WR" : "..");
    writeLog(digitalRead(MREQ)==LOW ? "MREQ" : "....");
    writeLog(digitalRead(IORQ)==LOW ? "IORQ" : "....");
    // build a number from the address bits
    unsigned int a = 0;
    for(int i=0; i<16; i++) bitWrite(a,i,digitalRead(A[i]));
    char s[20];
    sprintf(s,"A:%04x",a);
    writeLog(s);
    // build a number from the data bits
    byte d = 0;
    for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
    sprintf(s,"D:%02x",d);
    writeLog(s);
    writeLog("|");
  }
}

void writeLog(String s) {
  if(LOGGING) {
    Serial.print(s);
    Serial.print(' ');
  }
}

void writeLogLn() {
  if(LOGGING) Serial.print('\n');
}

void writeOut(String s) {
  Serial.print(s);
}