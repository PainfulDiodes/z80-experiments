// Define constants for the Z80 pins and the Arduino Mega pins to which they are connected

// Address Bus
const char A[] = {22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37}; // Z80 30-40,1-5 Output

// System Control
const char M1 =     46; // Z80 27 Output
const char MREQ =   47; // Z80 19 Output
const char RFSH =   49; // Z80 28 Output

// System Control - used for interrupts
const char RD =     18; // Z80 21 Output

// Clock
// Note that we are using the clock to trigger an interrupt. Arduino Mega will handle interrupts only on 2, 3, 18, 19, 20, 21 
// and 20-21 have special considerations - https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
const char CLK =     2; // Z80 6  Input

// Arduino state 
String cache = "";

void setup() {
  Serial.begin(9600);
  // digital pins default to input mode, and so we don't need to set status for every pin https://docs.arduino.cc/learn/microcontrollers/digital-pins/
  attachInterrupt(digitalPinToInterrupt(CLK), clockInterrupt, CHANGE); // RISING / FALLING / CHANGE
}

void loop() { // loop is needed only to flush the message cache - the work is done in interrupts
  if(cache.length()>0) {
    Serial.print(cache);
    cache="";
  }
} 

void clockInterrupt() { // interrupt handler

  writeLog(digitalRead(CLK)==LOW ? "CLK:0" : "CLK:1");
  writeLog(digitalRead(RFSH)==LOW ? "RFSH" : "....");
  writeLog(digitalRead(M1)==LOW   ? "M1" : "..");
  writeLog(digitalRead(RD)==LOW   ? "RD" : "..");
  writeLog(digitalRead(MREQ)==LOW ? "MREQ" : "....");

  // build a number from the address bits
  unsigned int a = 0;
  for(int i=0; i<16; i++) a = bitWrite(a,i,digitalRead(A[i]));

  char s[20];
  sprintf(s,"A:%04x",a);
  writeLog(s);

  writeLogLn();
}

void writeLog(String s) {
  cache += String(s);
  cache += String(' ');
}

void writeLogLn() {
  cache += String('\n');
}
