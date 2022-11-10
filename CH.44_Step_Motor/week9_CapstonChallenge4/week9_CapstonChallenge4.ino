#define INT1 22
#define INT2 24
#define INT3 26
#define INT4 28

const int TWO_REPS = 8191;

int steps = 0;
int stepsLeft = TWO_REPS;  // 전체 이동할 step

bool direction = true;  // true : 정방향, false : 역방향

unsigned long lastTime;  // 이전 회전 시간 저장

unsigned long currentMillis;  // 현재 시간 저장

long time;  // 2바퀴 회전 쇼요 시간

void setup() {
  Serial.begin(9600);
  pinMode(INT1, OUTPUT);
  pinMode(INT2, OUTPUT);
  pinMode(INT3, OUTPUT);
  pinMode(INT4, OUTPUT);
}

int count = 0;

void loop() {
  while (stepsLeft > 0) {
    currentMillis = micros();

    if (currentMillis - lastTime >= 1000) {
      stepper();
      time = time + micros() - lastTime;
      lastTime = micros();
      stepsLeft--;
    }
  }

  Serial.println(steps);  // 정 : 0, 역 : 7 출력
  Serial.println("Wait!!");
  delay(3000);

  direction = !direction;  // 방향 변경

  stepsLeft = TWO_REPS;
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
  digitalWrite(INT1, step1);
  digitalWrite(INT2, step2);
  digitalWrite(INT3, step3);
  digitalWrite(INT4, step4);
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