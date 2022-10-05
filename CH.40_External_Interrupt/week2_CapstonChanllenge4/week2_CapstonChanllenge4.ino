#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define led1 22
#define led2 23
#define led3 24
#define led4 25

#define sw1 2
#define sw2 3

volatile int state = HIGH;
volatile int count_sw1 = 0;
volatile int count_sw2 = 4;
const byte interruptPin1 = 2;
const byte interruptPin2 = 3;

void setup() {

  // put your setup code here, to run once:
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);
  pinMode(sw2, INPUT_PULLUP);

  lcd.init();
  lcd.backlight();

  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(interruptPin1), leastToMost, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), mostToLeast, RISING);
}

void loop() {

  if (count_sw1 > 0 && count_sw1 < 5) {
    lcd.clear();
    lcd.print("Falling Edge");
    lcd.setCursor(0, 2);
    lcd.print(count_sw1);
  }

  if (count_sw1 >= 5) {
    count_sw1 = 0;
    PORTA = 0x01;
  }

  if (count_sw2 < 4 && count_sw2 >= 0) {
    lcd.clear();
    lcd.print("Rising Edge");
    lcd.setCursor(0, 1);
    lcd.print(count_sw1);
  }

  if (count_sw2 < 0) {
    count_sw2 = 0;
    PORTA = 0x04;
  }
}

void leastToMost() {
  count_sw1++;
  PORTA = 0x01 << count_sw1;
}

void mostToLeast() {
  count_sw2--;
  PORTA = 0x04 >> count_sw2;
}