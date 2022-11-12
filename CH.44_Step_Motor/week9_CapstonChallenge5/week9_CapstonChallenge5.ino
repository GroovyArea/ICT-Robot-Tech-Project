#define IN1 22
#define IN2 24
#define IN3 26
#define IN4 28

const int TRIGGER = 12;
const int ECHO = 13;

bool direction = true;
int steps = 0;

volatile long distance;

unsigned long lastTime;
unsigned long currentMillis;
long time;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ECHO, INPUT);
  pinMode(TRIGGER, OUTPUT);
}

void loop() {
  distance = getDistanceOfCm();
  Serial.println(distance);

  if (distance < 10) {
    halfRoundRotate();
  } else if (distance >= 10 && distance <= 30) {
    forwardRotate();
  } else {
    reverseRotate();
  }
}

// 180 왕복 운동
void halfRoundRotate() {
  int halfDegree = 4095 / 2;

  while (halfDegree > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepper();

      time = time + micros() - lastTime;
      lastTime = micros();

      if (distance > 10) {
        break;
      }

      halfDegree--;
    }
  }

  direction = !direction;
  halfDegree = 4095 / 2;
}

// 정방향 회전
void forwardRotate() {
  direction = true;

  int forwardSteps = 4095;

  while (forwardSteps > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepper();

      time = time + micros() - lastTime;
      lastTime = micros();

      if (distance <= 10 || distance >= 30) {
        break;
      }

      forwardSteps--;
    }
  }

  forwardSteps = 4095;
}

// 역방향 회전
void reverseRotate() {
  direction = false;
  int reverseSteps = 4095;

  while (reverseSteps > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepper();

      time = time + micros() - lastTime;
      lastTime = micros();

      if (distance < 30) {
        break;
      }

      reverseSteps--;
    }
  }

  reverseSteps = 4095;
}

void stepper() {
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