#include <TimerOne.h>

#include <MsTimer2.h>

#include <LiquidCrystal.h>

// LiquidCrystal lcd(42, 43, 44, 45, 46, 47);

int switch_state;
int int_sw = 2;
int LED1 = 22;
int LED2 = 24;
int BUZZER = 8;
int LED1_state = HIGH;
int LED2_state = LOW;

void setup() {
  // put your setup code here, to run once:

  // lcd.begin(16, 2);
  // lcd.print("hello!");
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  Timer1.initialize(2000000); // 2초
  Timer1.attachInterrupt(flush);
}

void loop() {
  // put your main code here, to run repeatedly:
  // lcd.setCursor(0, 1);
  // lcd.print(millis() / 1000);
  switch_state = digitalRead(2);
  if (switch_state == HIGH) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

void flush() {
  if (LED1_state == HIGH) {
    digitalWrite(LED1, LED1_state);
  } else if (LED1_state == LOW) {
    digitalWrite(LED1, LED1_state);
  }

  if (LED2_state == HIGH) {
    digitalWrite(LED2, LED2_state);
  } else if (LED2_state == LOW) {
    digitalWrite(LED2, LED2_state);
  }

  LED1_state = !LED1_state;
  LED2_state = !LED2_state;
}