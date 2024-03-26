#include "z80.h"
#include "mem.h"

const unsigned int mem_size = sizeof(MEM);
const unsigned int SER_CTR = 0;
const unsigned int SER_DATA = 1;

// internal Arduino state 
String cache = "";
volatile bool isIORQ;

const bool LOGGING = true;

void setup() {
  Serial.begin(9600);
  // digital pins default to input mode, and so we don't need to set status for every pin https://docs.arduino.cc/learn/microcontrollers/digital-pins/

  // a CHANGE interrupt on the clock means we are triggering on half clock cycles (rising and falling edges)
  attachInterrupt(digitalPinToInterrupt(RD), readInterrupt, CHANGE); 
  attachInterrupt(digitalPinToInterrupt(WR), writeInterrupt, CHANGE); 
}

void loop() { // loop is needed only to flush the message cache - the work is done in interrupts
  if(cache.length()>0) {
    Serial.print(cache);
    cache="";
  }
} 

void readInterrupt() {
  if(digitalRead(RD)==LOW) { // we are here because RD has changed - if RD is LOW then it has just become LOW - a falling edge and so the beginning of a read
    for(int i=0; i<8; i++) pinMode(D[i], OUTPUT); // switch the data pins to output mode
    unsigned int a=0;
    byte d = 0x00;
    if(digitalRead(MREQ)==LOW) { // this is a read of memory
      for(int i=0; i<16; i++) bitWrite(a,i,digitalRead(A[i])); // get the address as a number
      if(a<mem_size) { 
        d = MEM[a];
      }
    }
    if(digitalRead(IORQ)==LOW) { // read of IO - do we need to check this? is it not implied?
      for(int i=0; i<8; i++) bitWrite(a,i,digitalRead(A[i])); // get the address as a number from the bottom 8 bits
      if(a==SER_CTR) { // simulated control register - all we want to return is whether there is data in the buffer
        bitWrite(d,0,(Serial.available()>0));
      }
      if(a==SER_DATA) {
        int i = Serial.read();
        d = i < 0 ? 0 : i;
      }
    }
    for(int i=0; i<8; i++) digitalWrite(D[i],bitRead(d,i)); // put the data from memory on the bus, bit by bit
    writeLogBusStatus();
    writeLogLn();
  }
  else { // RD is HIGH and has just become HIGH - rising edge and so the end of the read process
    for(int i=0; i<8; i++) pinMode(D[i], INPUT); // switch the pins back to input mode
  }
}

void writeInterrupt() {
  String s;
  if(digitalRead(WR)==LOW) { // we are here because WR has changed - if WR is LOW then it has just become LOW - a falling edge and so the beginning of a write
    for(int i=0; i<8; i++) pinMode(D[i], INPUT);  // switch the data pins to input mode
    isIORQ = (digitalRead(IORQ)==LOW);            // set the flag that we're writing to an IO - will capture on the rising edge
  }
  else { // WR rising edge - this is when we will capture the data that the Z80 has put on the bus
    if(isIORQ) {
      byte d = 0;
      for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
      s = String((char)d);
    }
    else {
      unsigned int a = 0;
      for(int i=0; i<16; i++) bitWrite(a,i,digitalRead(A[i]));
      if(a<mem_size) {
        byte d = 0;
        for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
        MEM[a] = d;
      }
    }
    writeLogBusStatus();
    writeOut(s);
    writeLogLn();
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
    cache += String(s);
    cache += String(' ');
  }
}

void writeLogLn() {
  if(LOGGING) cache += String('\n');
}

void writeOut(String s) {
  cache += String(s);
}