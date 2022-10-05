int switch_state;
int int_sw = 2;
int LED1 = 22;
int LED2 = 24;
int BUZZER = 8;
int LED1_state = HIGH;
int LED2_state = LOW;

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
}

void loop() {
  flush();

  switch_state = digitalRead(2);
  if (switch_state == HIGH) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }
}

void flush() {
  if (LED1_state == HIGH) {
    digitalWrite(LED1, LED1_state);
    delay(2000);
  } else if (LED1_state == LOW) {
    digitalWrite(LED1, LED1_state);
    delay(2000);
  }

  if (LED2_state == HIGH) {
    digitalWrite(LED2, LED2_state);
    delay(2000);
  } else if (LED2_state == LOW) {
    digitalWrite(LED2, LED2_state);
    delay(2000);
  }

  LED1_state = !LED1_state;
  LED2_state = !LED2_state;
}