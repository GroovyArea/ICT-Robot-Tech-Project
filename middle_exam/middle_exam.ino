int pin[17] = { -1, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37 };

int col[8] = { pin[9], pin[10], pin[11], pin[12], pin[13], pin[14], pin[15], pin[16] };

int row[8] = { pin[1], pin[2], pin[3], pin[4], pin[5], pin[6], pin[7], pin[8] };

const byte interruptPin = 2;
bool isChanged = true;

void setup() {
  // put your setup code here, to run once:
  pinMode(interruptPin, INPUT_PULLUP);

  for (int i = 1; i <= 16; i++) {
    pinMode(pin[i], OUTPUT);
  }

  noInterrupts();
  interrupts();
  attachInterrupt(digitalPinToInterrupt(interruptPin), fallingAct, FALLING);  // Falling
}

void loop() {
  // put your main code here, to run repeatedly:
}

void initDisplay() {
  for (int i = 0; i < 8; i++) {
    digitalWrite(row[i], LOW);
    digitalWrite(col[i], HIGH);
  }
}

void fallingAct() {
  if (isChanged) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(col[0], LOW);
      digitalWrite(row[i], HIGH);
      digitalWrite(col[i + 1], HIGH);
    }

  } else {
    initDisplay();
  }

  isChanged = !isChanged;
}