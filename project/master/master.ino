#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <string.h>
#include <stdio.h>

/**
  전처리기
*/
#define IN1 22  //모터 핀
#define IN2 24
#define IN3 26
#define IN4 28

#define SS_PIN 53  //RFID 핀
#define RST_PIN 49

#define c_4 261  // 계이름 도 주파수
#define e_4 329
#define g_4 392

#define SLAVE 20  // 통신용 슬레이브


/**
  엘리베이터 층 enum 
*/
enum class eFloor {
  FIRST,
  SECOND,
  THIRD
};

/**
  현재 층
*/
eFloor presentFloor = eFloor::FIRST;

/**
  엘리베이터 관련 상수
*/
const int EMERGENCY_BUTTON = 2;     //비상 호출 스위치
const int FIRST_FLOOR_BUTTON = 18;  //1층 스위치
const int SECOND_FLOOR_BUTTON = 3;  //2층 스위치 인터럽트
const int THIRD_FLOOR_BUTTON = 19;  //3층 스위치

const int ONE_FLOOR = 5;
const int TWO_FLOOR = 10;

const int INTERRUPTED_UP_PIN = 5;
const int INTERRUPTED_DOWN_PIN = 6;
const int INTERRUPTED_SECOND_ARRIVED_PIN = 7;

/**
  버저 상수
*/
const int BUZZER = 4;

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


/**
  7-Segment 층별 배열
*/
int firstFloor[7] = { 40, 41, 42, 43, 44, 45, 46 };   //세그먼트 1층
int secondFloor[7] = { 33, 34, 35, 36, 37, 38, 39 };  //세그먼트 2층
int thirdFloor[7] = { 23, 25, 27, 29, 30, 31, 32 };   //세그먼트 3층


/**
  7-Segment 숫자 디스플레이 배열
*/
int segmentNumbers[10][7] = {
  //세그먼트 숫자 표시
  { 1, 1, 1, 1, 1, 1, 0 },  //0
  { 0, 1, 1, 0, 0, 0, 0 },  //1
  { 1, 1, 0, 1, 1, 0, 1 },  //2
  { 1, 1, 1, 1, 0, 0, 1 },  //3
  { 0, 1, 1, 0, 0, 1, 1 },  //4
  { 1, 0, 1, 1, 0, 1, 1 },  //5
  { 1, 0, 1, 1, 1, 1, 1 },  //6
  { 1, 1, 1, 0, 0, 0, 0 },  //7
  { 1, 1, 1, 1, 1, 1, 1 },  //8
  { 1, 1, 1, 1, 0, 1, 1 }   //9
};


/**
  RFID 관련
*/
byte nuidPICC[4];                           //카드 ID를 비교하기 위한 배열
const String MANAGER_CARD = "83 4F 6D 02";  // 관리자 카드 번호
MFRC522 mfrc522(SS_PIN, RST_PIN);  // RFID 객체


/**
  Step motor 관련 변수
*/
bool direction = true;
int steps = 0;
unsigned long lastTime;
unsigned long currentMillis;
long time;
int presentRotateCount = 0;  // 현재까지 스텝 돈 수


/**
  릴레이 관련 변수
*/
const int RELAY_PIN = 13;
bool isTaged = true;



void setup() {
  for (int i = 0; i < 7; i++) {
    pinMode(firstFloor[i], OUTPUT);
    pinMode(secondFloor[i], OUTPUT);
    pinMode(thirdFloor[i], OUTPUT);
  }

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(FIRST_FLOOR_BUTTON, INPUT);
  pinMode(THIRD_FLOOR_BUTTON, INPUT);

  pinMode(INTERRUPTED_UP_PIN, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);

  Serial.begin(9600);

  SPI.begin();
  mfrc522.PCD_Init();
  ShowReaderDetails();
  Serial.println("Scan PICC to see UID, type, and data blocks...");

  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(SECOND_FLOOR_BUTTON), middleFloorStop, FALLING);

  Wire.begin();  // I2C 통신 초기화 및 시작

  digitalWrite(INTERRUPTED_UP_PIN, LOW);
  digitalWrite(INTERRUPTED_DOWN_PIN, LOW);
  digitalWrite(INTERRUPTED_SECOND_ARRIVED_PIN, LOW);
}

void loop() {
  int firstBtn = digitalRead(FIRST_FLOOR_BUTTON);
  int thirdBtn = digitalRead(THIRD_FLOOR_BUTTON);
  int emergencyBtn = digitalRead(EMERGENCY_BUTTON);

  /* 비상 호출 눌렀을 경우 */
  if (emergencyBtn == HIGH) {
    transferData(EMERGENCY_STOPPED);
  }

  /* 1층 눌럿을 경우 */
  if (firstBtn == HIGH) {
    presentFloorDigitOff();

    if (presentFloor == eFloor::SECOND) {
      transferData(DOWN);
      reverseRotate();
    } else if (presentFloor == eFloor::THIRD) {
      transferData(DOWN);
      reverseRotate();
      reverseRotate();
    }

    transferData(FIRST_FLOOR_ARRIVED);
    presentFloor = eFloor::FIRST;
    presentFloorDigitDisplay();
    presentRotateCount = 0;
  }

  /* 3층 눌럿을 경우 */
  if (thirdBtn == HIGH) {
    presentFloorDigitOff();

    if (presentFloor == eFloor::FIRST) {
      transferData(UP);
      forwardRotate();
      forwardRotate();
    } else if (presentFloor == eFloor::SECOND) {
      transferData(UP);
      forwardRotate();
    }

    transferData(THIRD_FLOOR_ARRIVED);
    presentFloor = eFloor::THIRD;
    presentFloorDigitDisplay();
    presentRotateCount = 0;
  }

  /* 태그 카드 관련 */
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  String tagedCard = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagedCard.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tagedCard.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  tagedCard.toUpperCase();

  if (tagedCard.substring(1) == MANAGER_CARD) {
    relayToggle();
    transferData(MANAGER_TAGED);
  }
}


// 릴레이 토글 함수
void relayToggle() {
  digitalWrite(RELAY_PIN, isTaged);
  presentFloorDigitOff();
  isTaged = !isTaged;
}

// 층별 Segment display on
void presentFloorDigitDisplay() {
  if (presentFloor == eFloor::FIRST) {
    for (int i = 0; i < 7; i++) {
      digitalWrite(firstFloor[i], segmentNumbers[1][i]);
    }
  } else if (presentFloor == eFloor::SECOND) {
    for (int i = 0; i < 7; i++) {
      digitalWrite(secondFloor[i], segmentNumbers[2][i]);
    }
  } else {
    for (int i = 0; i < 7; i++) {
      digitalWrite(thirdFloor[i], segmentNumbers[3][i]);
    }
  }
}


// 층별 Segment display off
void presentFloorDigitOff() {
  for (int i = 0; i < 7; i++) {
    digitalWrite(firstFloor[i], LOW);
    digitalWrite(secondFloor[i], LOW);
    digitalWrite(thirdFloor[i], LOW);
  }
}



// I2C 통신 함수
void transferData(char data) {
  Wire.beginTransmission(SLAVE);  // data 전송 시작
  Wire.write(data);               // buffer에 data 저장
  Wire.endTransmission();         // buffer에 저장된 data 전송
}


// 정방향 회전
void forwardRotate() {
  presentFloorDigitOff();
  direction = true;

  int totalRotateCount = 4095 * 4;

  while (totalRotateCount > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepperAct();

      time = time + micros() - lastTime;
      lastTime = micros();

      totalRotateCount--;
      presentRotateCount++;

      if (presentRotateCount == 4095 * 4) {
        break;
      }
    }
  }
}

// 역방향 회전
void reverseRotate() {
  presentFloorDigitOff();
  direction = false;

  int totalRotateCount = 4095 * 4;

  while (totalRotateCount > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepperAct();

      time = time + micros() - lastTime;
      lastTime = micros();

      totalRotateCount--;
      presentRotateCount++;

      if (presentRotateCount == 4095 * 4) {
        break;
      }
    }
  }
}

// 스텝 모터 케이스 별 동작
void stepperAct() {
  switch (steps) {
    case 0: runStep(LOW, LOW, LOW, HIGH); break;
    case 1: runStep(LOW, LOW, HIGH, HIGH); break;
    case 2: runStep(LOW, LOW, HIGH, LOW); break;
    case 3: runStep(LOW, HIGH, HIGH, LOW); break;
    case 4: runStep(LOW, HIGH, LOW, LOW); break;
    case 5: runStep(HIGH, HIGH, LOW, LOW); break;
    case 6: runStep(HIGH, LOW, LOW, LOW); break;
    case 7: runStep(LOW, LOW, LOW, HIGH); break;
    default: runStep(LOW, LOW, LOW, LOW); break;
  }
  setDirection();
}

// 스텝 모터 동작
void runStep(int step1, int step2, int step3, int step4) {
  digitalWrite(IN1, step1);
  digitalWrite(IN2, step2);
  digitalWrite(IN3, step3);
  digitalWrite(IN4, step4);
}

// 스텝 모터 방향 바꾸기
void setDirection() {
  if (direction == true) {
    steps++;
  }
  if (direction == false) {
    steps--;
  }
  if (steps > 7) {
    steps = 0;
  }
  if (steps < 0) {
    steps = 7;
  }
}

// RFID
void ShowReaderDetails() {
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);

  Serial.print("MFRC522 Software Version: 0x");
  Serial.print(v, HEX);

  if (v == 0x91) Serial.print(" = v1.0");

  else if (v == 0x92) Serial.print(" = v2.0");

  else Serial.print(" (unknown)");

  Serial.println("");

  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println("WARNING: Communication failure, is the MFRC522 properly connected?");
  }
}


// delayMicroseconds를 이용한 delay
void delay_(int delayTime) {
  int count = 0;

  while (count != delayTime) {
    delayMicroseconds(1000);
    count++;
  }

  count = 0;
}





/**
  2층 눌렀을 때 인터럽트 서비스 루틴
*/
void middleFloorStop() {
  presentFloorDigitOff();
  
  if (presentRotateCount == 0) {
    if (presentFloor == eFloor::FIRST) {
      digitalWrite(INTERRUPTED_UP_PIN, HIGH);
      forwardRotate();
      digitalWrite(INTERRUPTED_SECOND_ARRIVED_PIN, HIGH);
    } else if (presentFloor == eFloor::THIRD) {
      digitalWrite(INTERRUPTED_DOWN_PIN, HIGH);
      reverseRotate();
      digitalWrite(INTERRUPTED_SECOND_ARRIVED_PIN, HIGH);
    }

    digitalWrite(INTERRUPTED_UP_PIN, LOW);
    digitalWrite(INTERRUPTED_DOWN_PIN, LOW);
    presentFloor = eFloor::SECOND; // 현재 층을 2층으로 변경
    presentFloorDigitDisplay();
  } else {
    if (direction == true) {
      digitalWrite(INTERRUPTED_UP_PIN, HIGH);
      forwardRotate();
      digitalWrite(INTERRUPTED_SECOND_ARRIVED_PIN, HIGH);
    } else {
      digitalWrite(INTERRUPTED_DOWN_PIN, HIGH);
      reverseRotate();
      digitalWrite(INTERRUPTED_SECOND_ARRIVED_PIN, HIGH);
    }

    digitalWrite(INTERRUPTED_UP_PIN, LOW);
    digitalWrite(INTERRUPTED_DOWN_PIN, LOW);
    presentFloor = eFloor::SECOND; // 현재 층을 2층으로 변경
    presentFloorDigitDisplay();
    delay_(1000);
  }

  digitalWrite(INTERRUPTED_SECOND_ARRIVED_PIN, LOW);
  presentRotateCount = 0;
}