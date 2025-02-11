/*
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const int DISP_WIDTH = 16;
const int DISP_HEIGHT = 2;
int xpos=0;
int linebuff[DISP_WIDTH];

void setup() {
  lcd.begin(DISP_WIDTH, DISP_HEIGHT);
  Serial.begin(9600);
  lcd.blink();
  lcd.cursor();
  lcd.setCursor(xpos, 1);
  for(int i=0; i<DISP_WIDTH; i++) {
    linebuff[i]=' ';
  }
}

void loop() {
  int i = Serial.read();
  if(i>=0) {
    if(i < 0x20) {
      if(i=='\r') eol();
      if(i=='\b' && xpos>0) {
        xpos--;
        lcd.setCursor(xpos, 1);
        linebuff[xpos]=' ';
        lcd.write(' ');
        lcd.setCursor(xpos, 1);
      }
    }
    else {
      lcd.write(i);
      linebuff[xpos]=i;
      xpos++;
      if(xpos>=DISP_WIDTH) eol();
    }
  }
}

void eol() {
    lcd.setCursor(0,0);
    for(int i=0; i<DISP_WIDTH; i++) {
      lcd.write(linebuff[i]);
    }
    lcd.setCursor(0,1);
    for(int i=0; i<DISP_WIDTH; i++) {
      lcd.write(' ');
      linebuff[i]=' ';
    }
    lcd.setCursor(0,1);
    xpos=0;
}
