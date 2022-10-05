#include <TimerOne.h>

#include <MsTimer2.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(42, 43, 44, 45, 46, 47);

int switch_state;
int LED = 22;
int TRIGGER = 12;
int ECHO = 13;
int LED_state = HIGH;
int LCD_state = HIGH;

const byte interruptPin = 2;
volatile int fallingCount = 0;
volatile int risingCount = 0;
volatile long cm;  // 거리를 전역적으로 사용하기

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);

  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(interruptPin), fallingAct, FALLING);  // Falling 부터 진행

  lcd.begin(16, 2);
}

void loop() {  // 항상 초음파 거리는 lcd에 디스플레이
  // put your main code here, to run repeatedly:
  cm = getDistanceOfCm();
  lcd.setCursor(0, 0);
  lcd.print(cm);
}

// 첫번째 Falling edge 시 동작 함수
void countOneAct() {
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);

  flushLed();
}

// lcd 점멸
void countFiveAct() {
  if (LCD_state == HIGH) {
    lcd.setCursor(0, 1);
    lcd.print("Warning!!");
  } else if (LCD_state == LOW) {
    lcd.clear();
  }

  LCD_state = !LCD_state;
}

// led 점멸
void flushLed() {
  digitalWrite(LED, LED_state);
  LED_state = !LED_state;
}

// 초음파센서 작동 및 cm 변환 함수
long getDistanceOfCm() {
  digitalWrite(TRIGGER, LOW);  // 출력 깨끗히!
  delayMicroseconds(2);

  digitalWrite(TRIGGER, HIGH);  // 출력 시작
  delayMicroseconds(10);

  digitalWrite(TRIGGER, LOW);  // 출력 종료

  long duration = pulseIn(ECHO, HIGH);  // 에코가 트리거가 HIGH 였을 시간을 저장
  return (duration / 2) / 29.1;         // cm로 변환하여 반환
}

//Falling edge Interrupt Service Routine
void fallingAct() {

  fallingCount++;  // Falling count 1 증가

  if (fallingCount == 1) {
    MsTimer2::set(1000, countOneAct);
    MsTimer2::start();
  }

  if (fallingCount == 3) {
    lcd.clear();

    if (cm < 20) {
      MsTimer2::stop();
      digitalWrite(LED, HIGH);
    } else {
      MsTimer2::set(1000, flushLed);
      MsTimer2::start();
    }
  }

  if (fallingCount > 3) {
    MsTimer2::stop();
    noInterrupts();  // Rising Edge를 검출해야 하므로 인터럽트 종료
    interrupts();
    attachInterrupt(digitalPinToInterrupt(interruptPin), risingAct, RISING);
  }
}

void risingAct() {
  risingCount++;

  if (risingCount == 5) {
    lcd.clear();

    if (cm < 20) {
      MsTimer2::stop();
      MsTimer2::set(1000, flushLed);
      MsTimer2::start();
    } else {
      MsTimer2::stop();
      digitalWrite(LED, HIGH);
      MsTimer2::set(2000, countFiveAct);
      MsTimer2::start();
    }
  }
}