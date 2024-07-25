void setup() {
  for(int i = 2; i<=13; i++) {
    pinMode(i,OUTPUT);
  }
}

void loop() {
  delay(1);
    digitalWrite(13,HIGH);
  delay(1);
    digitalWrite(13,LOW);
}