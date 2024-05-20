#include "z80-mapping.h"

void setup() {
  Serial.begin(9600);
  // digital pins default to input mode, and so we don't need to set status for every pin https://docs.arduino.cc/learn/microcontrollers/digital-pins/

  pinMode(CLK, OUTPUT);
}

void loop() {
  digitalWrite(CLK,0);
  // delay();
  // writeLogBusStatus(0);
  digitalWrite(CLK,1);
  // writeLogBusStatus(1);
}

void writeLogBusStatus(int c) {
  Serial.print(c==HIGH ? "CLK1 " : "CLK0 ");
  Serial.print(digitalRead(USBWR)==HIGH   ? "USBWR " : "..... ");
  Serial.print(digitalRead(USB_RD)==LOW   ? "USBRD " : "...... ");
  Serial.print(digitalRead(RFSH)==LOW   ? "RFSH " : ".... ");
  Serial.print(digitalRead(RD)==LOW   ? "RD " : ".. ");
  Serial.print(digitalRead(WR)==LOW   ? "WR " : ".. ");
  Serial.print(digitalRead(MREQ)==LOW ? "MREQ " : ".... ");
  Serial.print(digitalRead(IORQ)==LOW ? "IORQ " : ".... ");
  // build a number from the address bits
  unsigned int a = 0;
  for(int i=0; i<16; i++) bitWrite(a,i,digitalRead(A[i]));
  char s[20];
  sprintf(s,"A:%04x ",a);
  Serial.print(s);
  // build a number from the data bits
  byte d = 0;
  for(int i=0; i<8; i++) bitWrite(d,i,digitalRead(D[i]));
  sprintf(s,"D:%02x ",d);
  Serial.print(s);
  Serial.print("\n");
}
