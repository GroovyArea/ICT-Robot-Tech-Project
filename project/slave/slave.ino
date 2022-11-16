#include <Wire.h>
#include <LiquidCrystal.h>

#define c_4 261  // 계이름 도 주파수
#define e_4 329
#define g_4 392

#define BUZZER 13

/**
  LCD 객
*/
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

/**
  버저 상수
*/
const int BUZZER = 4;

void setup() {
  lcd.begin(16, 2);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("hello");
  play();
}

// 엘베 도착음
void arrivedMusicPlay() {
  tone(BUZZER, c_4);
  delay_(400);
  tone(BUZZER, e_4);
  delay_(400);
  tone(BUZZER, g_4);
  delay_(400);
  noTone(BUZZER);
}

void delay_(int delayTime) {
  int count = 0;

  while (count != delayTime) {
    delayMicroseconds(1000);
    count++;
  }

  count = 0;
}