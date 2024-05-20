// Data
const char D[] = {2,3,4,5,6,7,8,9};

// UM245R control inputs
const char RD_ = 10; 
const char WR = 11; 
// UM245R control outputs
const char RXF_ = 12; // data in receive buffer
const char TXE_ = 13; // transmit ready

void setup() {
  Serial.begin(9600);
  pinMode(RD_, OUTPUT);
  pinMode(WR, OUTPUT);
  digitalWrite(RD_,HIGH);
  digitalWrite(WR,LOW);
}

void loop() {

  // if(digitalRead(RXF_)==LOW) { // data in buffer
    for(int i=0; i<8; i++) pinMode(D[i], INPUT);
    digitalWrite(RD_,LOW);
    byte b = 0x00;
    for(int i=0; i<8; i++) bitWrite(b,i,digitalRead(D[i]));
    digitalWrite(RD_,HIGH);
    Serial.write(b);
    Serial.write('\n');
  // }

  if(Serial.available()>0) {
    // if(digitalRead(TXE_)==LOW) { // ok to send
      digitalWrite(WR,HIGH);
      byte b = Serial.read();
      for(int i=0; i<8; i++) pinMode(D[i], OUTPUT);
      for(int i=0; i<8; i++) digitalWrite(D[i],bitRead(b,i));
      digitalWrite(WR,LOW);
      for(int i=0; i<8; i++) pinMode(D[i], INPUT);
    // } 
  }
}
