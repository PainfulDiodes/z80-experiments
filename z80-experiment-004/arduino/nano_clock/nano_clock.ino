const char CLK =     13; // Z80 6  Input
const char CLKINH =  14; // Inhibit external clock module
const char RESET =   19; // Z80 26 Input

bool ARD_CLOCK = false;

void setup() {
  Serial.begin(9600);
  if(ARD_CLOCK) { // arduino clock
    digitalWrite(CLKINH,LOW);
    pinMode(CLKINH,OUTPUT);
    pinMode(CLK,OUTPUT);
  }
  else { // external clock
    pinMode(CLKINH,INPUT);
    pinMode(CLK,INPUT);
  }
  delay(300);
  outputLow(RESET);
  delay(300);
  outputOpen(RESET);
}

void loop() {
   if(ARD_CLOCK) {
    digitalWrite(CLK,0);
    delay(1);
    digitalWrite(CLK,1);
    delay(1);
  }
}

void outputOpen(char c) {
  pinMode(c, INPUT);
}

void outputLow(char c) {
  digitalWrite(c,LOW);
  pinMode(c, OUTPUT);
}
