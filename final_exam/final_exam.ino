/**
  전처리기
*/
#define IN1 22  //모터 핀
#define IN2 24
#define IN3 26
#define IN4 28

/**
  상수
*/
const int SWITCH = 2;

const int TILT = 8;

const int ONE_ROTATE = 4095;

/**
  Step motor 관련 변수
*/
bool direction = true;
int steps = 0;
unsigned long lastTime;
unsigned long currentMillis;
long time;

bool isStopped = true;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(SWITCH, INPUT);

  pinMode(TILT, INPUT);

  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(SWITCH), isr, FALLING);
}

void loop() {
  int tiltInput = digitalRead(TILT);

  if (tiltInput == HIGH) {
    Serial.println("Counterclockwise");
    forwardRotate();
  } else {
    reverseRotate();
  }

  if (isStopped == false) {
    while (true) {
      steps = 0;

      if (isStopped == true) {
        break;
      }
    }
  }
}

// 정방향 회전
void forwardRotate() {
  direction = true;
  rotate();
}


// 역방향 회전
void reverseRotate() {
  direction = false;
  rotate();
}

void rotate() {
  int totalRotateCount = ONE_ROTATE;

  while (totalRotateCount > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepperAct();

      time = time + micros() - lastTime;
      lastTime = micros();

      totalRotateCount--;
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

void isr() {
  Serial.println("Step Motor Stop");
  isStopped = !isStopped;
}