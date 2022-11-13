#include <SPI.h>      //RFID 통신을 위한 SPI 라이브러리
#include <MFRC522.h>  //RFID 모듈 제어 라이브러리

#define RST_PIN 49  //RFID 모듈의 RST 제어 핀 선언
#define SS_PIN 53   //RFID 모듈의 SDA 제어 핀 선언

int segment[7] = { 23, 25, 27, 29, 30, 31, 32 };  //7-세그먼트
int one[7] = { 0, 1, 1, 0, 0, 0, 0 };
int two[7] = { 1, 1, 0, 1, 1, 0, 1 };
int three[7] = { 1, 1, 1, 1, 0, 0, 1 };
int four[7] = { 0, 1, 1, 0, 0, 1, 1 };

MFRC522 mfrc522(SS_PIN, RST_PIN);
//RFID 모듈 객체 생성
//RFID 모듈 초기화
// SS_PIN : SPI 통신 시 RFID 모듈 선택 핀 번호
// RST_PIN: 리셋 및 전원 Off 핀 번호

String firstCard = "83 4F 6D 02";
String secondCard = "13 B7 93 03";
String firstTag = "1C F2 B0 17";
String secondTag = "1C 6E 4D 17";

void setup() {
  int length = sizeof(segment) / sizeof(int);

  for (int i = 0; i < length; i++) {
    pinMode(i, OUTPUT);
  }

  Serial.begin(9600);
  SPI.begin();

  // SPI 통신 초기화
  // RFID모듈의 SPI통신을 위한 설정
  mfrc522.PCD_Init();

  // RFID 모듈 초기화
  ShowReaderDetails();

  // RFID 버전 정보 및 통신 여부 출력
  Serial.println("Scan PICC to see UID, type, and data blocks...");
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  // 새로운 카드 접촉 체크
  // 새로운 RFID카드 여부 체크
  // 새로운 RFID카드 여부 리턴, return값: true-> 새로운 RFID카드

  if (!mfrc522.PICC_ReadCardSerial()) return;
  // 카드 정보 읽기
  // RFID카드 UID 읽을 수 있는 카드 여부 체크
  // UID를 읽을 수 있는 카드 여부 리턴, return값: true-> UID를 읽을 수 있는 카드

  mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
  // RFID 카드의 UID 정보 출력
  // RFID카드의 UID 정보 읽어서 출력
  // mfrc522.uid: 읽을 RFID카드의 UID

  String content = "";

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  content.toUpperCase();

  if (content.substring(1) == firstTag) {
    for (int i = 0; i < sizeof(one) / sizeof(int); i++) {
      digitalWrite(segment[i], one[i]);
    }
  } else if (content.substring(1) == secondTag) {
    for (int i = 0; i < sizeof(two) / sizeof(int); i++) {
      digitalWrite(segment[i], two[i]);
    }
  } else if (content.substring(1) == firstCard) {
    for (int i = 0; i < sizeof(three) / sizeof(int); i++) {
      digitalWrite(segment[i], three[i]);
    }
  } else {
    for (int i = 0; i < sizeof(four) / sizeof(int); i++) {
      digitalWrite(segment[i], four[i]);
    }
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