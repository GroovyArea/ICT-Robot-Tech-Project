# Project 

## 작품 명
- 엘리베이터

<br>

## 작품 개괄
- Arduino Mega 2560 보드 이용
- I2C 를 이용한 두 보드 모듈 간 통신
- 승강 시 중간 층 인터럽트 구현
- SPI 통신을 이용한 RFID 관리자 기능 구현 (엘리베이터 정비 중 구현)
  - 릴레이 모듈 사용으로 전원 제어

<br>

## 작품 사진
<br>
<img src = "https://user-images.githubusercontent.com/89288109/205474964-8418323b-8f9f-4aec-94db-acf7054738d9.png" width="600" height = "500">

- 정면

<br>
<img src = "https://user-images.githubusercontent.com/89288109/205475036-59a81acf-dc3a-4c10-ada1-73bff477e16b.png" width="600" height = "600">

- 윗면

## 모듈 및 소자 설명

### Arduino mega 2560 - `Master`
- Step motor 1ea (+ Motor driver)
- Relay module 1ea
- Tect switch 4ea
- 7-segment 3ea
- RFId 1ea

=> Master 모듈

<br>

### Arduino mega 2560 - `Slave`
- LCD 1ea
- Piezo buzzer 1ea

=> Slave 모듈

<br>

## 회로도 (통신 선 포커스)

<img src="https://user-images.githubusercontent.com/89288109/205475141-d318d9f3-ef19-4edd-832e-e139b5206093.png" width="800" height="200">

- 5, 6, 7번 핀끼리 연결
- 20, 21번 핀 연결
  - I2C 통신
- 릴레이 모듈로 스위치 전원 제어  

<br>

## 코드 설명
```c++
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
```
- 엘리베이터 현재 층 상태
- 런타임에 상태를 나타내기 위한 멤버 변수가 필요함
- Literal 값 보다는 enumeration class를 정의해 층 수를 표시  
[**의미** 있는 값을 표시]

<br>

```c++
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
```
- 1층을 누른 경우의 loop() 함수 행위 (3층도 동일)
- 단순 나열 식 코드보다는 중복을 제거하기 위해 함수를 따로 정의했다.

<br>

```c++
// I2C 통신 함수
void transferData(char data) {
  Wire.beginTransmission(SLAVE);  // data 전송 시작
  Wire.write(data);               // buffer에 data 저장
  Wire.endTransmission();         // buffer에 저장된 data 전송
}
```
- I2C 통신을 이용
  - delay()를 사용하는 동작이 존재함
- Multi thread의 구현이 불가능하므로 모듈 1개를 더 추가하기로 결정
- 현재 상태 (무슨 층인지, 모터 바퀴 수, 그 외 현재 상태 등등) 을 다른 모듈도 공유해야 하기 때문에, 통신을 사용해서 정의한 표준 data를 주고 받으며 하나의 프로세스로 이어짐.

<br>

```c++
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
```
- 통신 데이터 - 상수로 정의
- 두 모듈 간 상태를 공유

<br>

```c++
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
 
  ....
  ```
  - 다른 모듈의 코드 (slave)
  - I2C 통신으로 전송 받은 데이터를 switch 문으로 case를 나누어 추가 동작 명령
  - rec[] 는 통신 데이터를 받기 위한 배열
    - loop()의 반복 때문에 1번만 실행 후 멈추기 위해서 rec[0]에 garbage 값을 추가함.

<br>

```c++
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
```
- 2층 (중간 층) 인터럽트 서비스 루틴
- (단순히 2층을 눌렀을 경우)

<br>

```c++
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
```
- 2층 (중간 층) 인터럽트 서비스 루틴
- 1 ~ 3층 or 3 ~ 1층 이동 중
- 중간 층인 2층에서 잠시 정지 후 초기 목적지로 이동

<br>

```c++
/*
   엘리베이터 상수
*/
const int INTERRUPTED_UP_PIN = 5;
const int INTERRUPTED_DOWN_PIN = 6;
const int INTERRUPTED_SECOND_ARRIVED_PIN = 7;
```
- ISR 동작 시 간접적인 통신 이용 함수 정의
- digitalWrite() 함수 사용
  - 이유 : 인터럽스 서비스 루틴 실행 시 I2C 통신 불가
  - 2층에서도 여러 소자가 동작해야 된다.
- 간접적으로 하드웨어 신호를 조작해 해결
- HIGH, LOW 알맞게 흘려 보냄.

<br>

```c++
  bool isUp = digitalRead(INTERRUPTED_UP_PIN);
  bool isDown = digitalRead(INTERRUPTED_DOWN_PIN);
  bool isSecondArrived = digitalRead(INTERRUPTED_SECOND_ARRIVED_PIN);

  if (isUp == HIGH) {
    goUp();
  }

  if (isDown == HIGH) {
    goDown();
  }
 ```
 - 이렇게 Master에서 write한 신호를 read로 읽음
 - 그 상태에 맞게 정의된 행위를 동시에 실행

 <br>

 ```c++
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
```
- RFID 모듈에 태그 된 카드를 읽어 미리 정의한 상수와 비교
- 같을 경우, 릴레이 모듈 실행 (스위치 전력 차단)과 slave 모듈에게 상태 데이터 전송
