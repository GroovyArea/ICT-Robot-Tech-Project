#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

/**
  전처리기
*/
#define IN1 22  //모터 핀
#define IN2 24
#define IN3 26
#define IN4 28

#define SS_PIN 5  //RFID 핀
#define RST_PIN 4

#define c_4 261  // 계이름 도 주파수
#define e_4 329
#define g_4 392



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
const int EMERGENCY_BUTTON = 2;     //비상정지 스위치 인터럽트 2번핀
const int FIRST_FLOOR_BUTTON = 18;  //1층 스위치
const int SECOND_FLOOR_BUTTON = 3;  //2층 스위치
const int THIRD_FLOOR_BUTTON = 19;  //3층 스위치

const int ONE_FLOOR = 5;
const int TWO_FLOOR = 10;

/**
  버저 상수
*/
const int BUZZER = 4;

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
byte nuidPICC[4];                             //카드 ID를 비교하기 위한 배열
byte managerCard[4] = { 115, 202, 136, 12 };  //관리자 카드 ID
byte residentCard[4] = { 19, 241, 90, 2 };    //입주민 카드 ID

/**
  RFID 객체 생성
*/
MFRC522 mfrc522(SS_PIN, RST_PIN);  // RFID 객체

/**
  I2C_LCD 객체 생성
*/
LiquidCrystal_I2C lcd(0x27, 16, 2);  //LCD 핀

/**
  Step motor 관련 변수
*/
bool direction = true;
int steps = 0;
unsigned long lastTime;
unsigned long currentMillis;
long time;

int presentRotateCount = 0;  // 현재까지 스텝 돈 수

bool isInterrupt = false;  // 인터럽트 걸렸니?

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
  pinMode(SECOND_FLOOR_BUTTON, INPUT);
  pinMode(THIRD_FLOOR_BUTTON, INPUT);

  pinMode(BUZZER, OUTPUT);

  Serial.begin(9600);

  lcd.init();       // LCD 초기화
  lcd.backlight();  // 백라이트 켜기

  SPI.begin();
  mfrc522.PCD_Init();
  ShowReaderDetails();
  Serial.println("Scan PICC to see UID, type, and data blocks...");

  noTone(BUZZER);

  // 인터럽트
  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(EMERGENCY_BUTTON), emergencyCommunicate, FALLING);
  //attachInterrupt(digitalPinToInterrupt(FIRST_FLOOR_BUTTON), bottonFloorStop, FALLING);
  attachInterrupt(digitalPinToInterrupt(SECOND_FLOOR_BUTTON), middleFloorStop, FALLING);
  //attachInterrupt(digitalPinToInterrupt(THIRD_FLOOR_BUTTON), topFloorStop, FALLING);
}

unsigned long currentMs;

void loop() {
  // if (!mfrc522.PICC_IsNewCardPresent()) return;
  // if (!mfrc522.PICC_ReadCardSerial()) return;
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  // String tagedCard = "";

  // // 새 카드 접촉 시
  // if (mfrc522.uid.uidByte[0] != nuidPICC[0] || mfrc522.uid.uidByte[1] != nuidPICC[1] || mfrc522.uid.uidByte[2] != nuidPICC[2] || mfrc522.uid.uidByte[3] != nuidPICC[3]) {
  //   for (byte i = 0; i < 4; i++) {
  //     nuidPICC[i] = mfrc522.uid.uidByte[i];  // 배열에 카드 번호 저장
  //   }

  //   for( byte i =0; i<mfrc522.uid.size; i++) {
  //     tagedCard.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
  //     tagedCard.concat(String(mfrc522.uid.uidByte[i], DEC));
  //   }
  // }

  int firstBtn = digitalRead(FIRST_FLOOR_BUTTON);
  int thirdBtn = digitalRead(THIRD_FLOOR_BUTTON);

  /* 1층 눌럿을 경우 */
  if (firstBtn == HIGH) {
    presentFloorDigitOff();
    if (presentFloor == eFloor::SECOND) {
      downPointerShift();
      reverseRotate();
      arrivedMusicPlay();
    } else if (presentFloor == eFloor::THIRD) {
      downPointerShift();
      reverseRotate();
      reverseRotate();
      arrivedMusicPlay();
    }

    presentFloor = eFloor::FIRST;
    presentFloorDigitDisplay();
    presentRotateCount = 0;
  }

  /* 3층 눌럿을 경우 */
  if (thirdBtn == HIGH) {
    presentFloorDigitOff();
    if (presentFloor == eFloor::FIRST) {
      upPointerShift();
      forwardRotate();
      forwardRotate();
      arrivedMusicPlay();
    } else if (presentFloor == eFloor::SECOND) {
      upPointerShift();
      forwardRotate();
      arrivedMusicPlay();
    }

    presentFloor = eFloor::THIRD;
    presentFloorDigitDisplay();
    presentRotateCount = 0;
  }
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


// 위로 향하는 화살표 lcd
void upPointerShift() {
}

// 아래로 향하는 화살표 lcd
void downPointerShift() {
}



void forwardRotate() {
  presentFloorDigitOff();
  direction = true;

  int totalRotateCount = 4095 * 1;

  while (totalRotateCount > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepperAct();

      time = time + micros() - lastTime;
      lastTime = micros();

      totalRotateCount--;
      presentRotateCount++;

      if (presentRotateCount == 4095) {
        break;
      }
    }
  }
}

void reverseRotate() {
  presentFloorDigitOff();
  direction = false;

  int totalRotateCount = 4095 * 1;

  while (totalRotateCount > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepperAct();

      time = time + micros() - lastTime;
      lastTime = micros();

      totalRotateCount--;
      presentRotateCount++;

      if (presentRotateCount == 4095) {
        break;
      }
    }
  }
}

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

void runStep(int step1, int step2, int step3, int step4) {
  digitalWrite(IN1, step1);
  digitalWrite(IN2, step2);
  digitalWrite(IN3, step3);
  digitalWrite(IN4, step4);
}

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

void delay_(int delayTime) {
  int count = 0;

  while (count != delayTime) {
    delayMicroseconds(1000);
    count++;
  }

  count = 0;
}

/**
  비상정지 인터럽트 시 행동
*/
void emergencyCommunicate() {
  // 경비실로 연결, 시리얼 모니터로 계속 받고 LCD에 내용 디스플레이
  // 비상정지 버튼이나, 특정 문자 입력 시 종료
}


/**
  2층 눌렀을 때
*/
void middleFloorStop() {
  presentFloorDigitOff();
  if (presentRotateCount == 0) {
    if (presentFloor == eFloor::FIRST) {
      upPointerShift();
      forwardRotate();
    } else if (presentFloor == eFloor::THIRD) {
      downPointerShift();
      reverseRotate();
    }
    presentFloor = eFloor::SECOND;
    presentFloorDigitDisplay();
  } else {
    if (direction == true) {
      forwardRotate();
    } else {
      reverseRotate();
    }
    presentFloor = eFloor::SECOND;
    presentFloorDigitDisplay();
    delay_(1000);
  }
  presentRotateCount = 0;
  isInterrupt = true;
}