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

/**
  엘리베이터 층 enum 
*/
typedef enum ElevatorState {
  FIRST = 1,
  SECOND,
  THIRD
} FLOOR;

/**
  현재 층
*/
FLOOR presentFloor = FIRST;

/**
  엘리베이터 관련 상수
*/
const int EMERGENCY_BUTTON = 2;      //비상정지 스위치 인터럽트 2번핀
const int FIRST_FLOOR_BUTTON = 14;   //1층 스위치
const int SECOND_FLOOR_BUTTON = 15;  //2층 스위치
const int THIRD_FLOOR_BUTTON = 16;   //3층 스위치

/**
  버저 상수
*/
const int BUZZER = 17;

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
byte nuidPICC[4];
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

  pinMode(BUZZER, OUTPUT);

  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(EMERGENCY_BUTTON), emergencyAct, FALLING);

  Serial.begin(9600);

  lcd.init();                      // LCD 초기화
  lcd.backlight();                // 백라이트 켜기

  SPI.begin();
  mfrc522.PCD_Init();
  ShowReaderDetails();
  Serial.println("Scan PICC to see UID, type, and data blocks...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;
  
  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

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

/**
  비상정지 인터럽트 시 행동
*/
void emergencyAct() {
}