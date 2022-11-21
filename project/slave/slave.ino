#include <Wire.h>
#include <LiquidCrystal.h>

#define slave 20

#define c_4 261  // 계이름 도 주파수
#define e_4 329
#define g_4 392

/**
  통신 용 상수
*/
const char UP = 'u';
const char DOWN = 'd';
const char FIRST_FLOOR_ARRIVED = 'f';
const char SECOND_FLOOR_ARRIVED = 's';
const char THIRD_FLOOR_ARRIVED = 't';
const char EMERGENCY_STOPPED = 'e';
const char MANAGER_TAGED = 'm';

bool isRelayToggled = false;  //초기 릴레이 상태
/**
  통신 데이터 저장 배열
*/
byte rec[128];


/**
  LCD 객체
*/
LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

/**
  버저 상수
*/
const int BUZZER = 13;
/*
   엘리베이터 상수
*/
const int INTERRUPTED_UP_PIN = 5;
const int INTERRUPTED_DOWN_PIN = 6;
const int INTERRUPTED_SECOND_ARRIVED_PIN = 7;

/**
    LCD 화살표 배열
*/

byte arrowR1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11111
};
byte arrowR2[8] = {
  0b10000,
  0b11000,
  0b11100,
  0b11110,
  0b11111,
  0b11000,
  0b11000,
  0b11000
};
byte arrowL2[8] = {
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b11111,
  0b00011,
  0b00011,
  0b00011
};
byte arrowL1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00001,
  0b00011,
  0b00111,
  0b01111,
  0b11111
};
byte arrowL3[8] = {
  0b00011,
  0b00011,
  0b00011,
  0b00011,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte arrowR3[8] = {
  0b11000,
  0b11000,
  0b11000,
  0b11000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};
byte arrowDR2[8] = {
  0b11000,
  0b11000,
  0b11000,
  0b11111,
  0b11110,
  0b11100,
  0b11000,
  0b10000
};
byte arrowDR1[8] = {
  0b11111,
  0b11110,
  0b11100,
  0b11000,
  0b10000,
  0b00000,
  0b00000,
  0b00000
};
byte arrowDL1[8] = {
  0b11111,
  0b01111,
  0b00111,
  0b00011,
  0b00001,
  0b00000,
  0b00000,
  0b00000
};
byte arrowDL2[8] = {
  0b00011,
  0b00011,
  0b00011,
  0b11111,
  0b01111,
  0b00111,
  0b00011,
  0b00001
};
byte arrowDR3[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00011,
  0b00011,
  0b00011,
  0b00011
};
byte arrowDL3[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11000,
  0b11000,
  0b11000,
  0b11000
};
int x = 300;

void setup() {
  Serial.begin(9600);
  Wire.begin(slave);
  lcd.begin(16, 2);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  pinMode(INTERRUPTED_UP_PIN, INPUT);
  pinMode(INTERRUPTED_DOWN_PIN, INPUT);
  pinMode(INTERRUPTED_SECOND_ARRIVED_PIN, INPUT);
}


void loop() {
  LcdDisplay();
  bool isUp = digitalRead(INTERRUPTED_UP_PIN);
  bool isDown = digitalRead(INTERRUPTED_DOWN_PIN);
  bool isSecondArrived = digitalRead(INTERRUPTED_SECOND_ARRIVED_PIN);

  if (isUp == HIGH) {
    goUp();
  }

  if (isDown == HIGH) {
    goDown();
  }

  if (isSecondArrived == HIGH) {
    rec[0] = SECOND_FLOOR_ARRIVED;
  }
}
void LcdClear() {
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("                ");
}
void LcdDisplay() {
  Wire.onReceive(record);
  char recivedNum = rec[0];

  switch (recivedNum) {
    case FIRST_FLOOR_ARRIVED:
      LcdClear();
      lcd.setCursor(0, 0);
      lcd.print("FIRST_FLOOR");
      arrivedMusicPlay();
      rec[0] = '.';
      break;
    case SECOND_FLOOR_ARRIVED:
      LcdClear();
      lcd.setCursor(0, 0);
      lcd.print("Second_Floor     ");
      arrivedMusicPlay();
      rec[0] = '.';
      break;
    case THIRD_FLOOR_ARRIVED:
      LcdClear();
      lcd.setCursor(0, 0);
      lcd.print("THIRD_FLOOR     ");
      arrivedMusicPlay();
      rec[0] = '.';
      break;
    case UP:
      goUp();  //올라가는함수
      break;
    case DOWN:
      goDown();  //내려가는함수
      break;
    case EMERGENCY_STOPPED:
      emergencyStop();
      break;
    case MANAGER_TAGED:
      managerLock();
      rec[0] = '.';
      break;
  }
}
void managerLock() {
  if (isRelayToggled == false) {
    LcdClear();
    lcd.setCursor(0, 0);
    lcd.print("Under");
    lcd.setCursor(0,1);
    lcd.print("Maintenance");
  } else {
    LcdClear();
  }

  isRelayToggled = !isRelayToggled;
}

void emergencyStop() {
  lcd.setCursor(0, 0);
  lcd.print("Calling the ");
  lcd.setCursor(0, 1);
  lcd.print("guard room");
}
/*
    master로부터 받은 데이터를 rec에 저장
*/
void record(int receiveNum) {
  for (int i = 0; i < receiveNum; i++) {
    rec[i] = Wire.read();
  }
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

void goDown() {
  lcd.createChar(1, arrowDL1);
  lcd.createChar(2, arrowDR1);
  lcd.createChar(3, arrowDL2);
  lcd.createChar(4, arrowDR2);
  lcd.createChar(5, arrowDR3);
  lcd.createChar(6, arrowDL3);

  lcd.clear();
  lcd.setCursor(14, 0);
  lcd.write(3);
  lcd.setCursor(15, 0);
  lcd.write(4);
  delay(x);
  lcd.clear();



  lcd.setCursor(14, 0);
  lcd.write(5);
  lcd.setCursor(15, 0);
  lcd.write(6);
  lcd.setCursor(14, 1);
  lcd.write(3);
  lcd.setCursor(15, 1);
  lcd.write(4);
  delay(x);
  lcd.clear();

  lcd.setCursor(14, 1);
  lcd.write(5);
  lcd.setCursor(15, 1);
  lcd.write(6);
  delay(x);
  lcd.clear();

  lcd.setCursor(14, 0);
  lcd.write(1);
  lcd.setCursor(15, 0);
  lcd.write(2);
  delay(x);
  lcd.clear();
}


void goUp() {
  lcd.clear();
  lcd.createChar(1, arrowL1);
  lcd.createChar(2, arrowL2);
  lcd.createChar(3, arrowL3);
  lcd.createChar(4, arrowR1);
  lcd.createChar(5, arrowR2);
  lcd.createChar(6, arrowR3);


  lcd.setCursor(0, 1);
  lcd.write(2);
  lcd.setCursor(1, 1);
  lcd.write(5);
  delay(x);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.write(2);
  lcd.setCursor(1, 0);
  lcd.write(5);
  lcd.setCursor(0, 1);
  lcd.write(3);
  lcd.setCursor(1, 1);
  lcd.write(6);
  delay(x);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.write(3);
  lcd.setCursor(1, 0);
  lcd.write(6);
  lcd.setCursor(0, 1);
  lcd.write(1);
  lcd.setCursor(1, 1);
  lcd.write(4);
  delay(x);
  lcd.clear();
}