#include <Stepper.h>

#define INT1_PIN 22
#define INT2_PIN 24
#define INT3_PIN 26
#define INT4_PIN 28

// Stepper 객체 생성
Stepper motor(64, INT1_PIN, INT2_PIN, INT3_PIN, INT4_PIN);

void setup() {
  for (int pin = INT1_PIN; pin < INT4_PIN; pin++) {
    pinMode(pin, OUTPUT);
  }
  Serial.begin(9600);
  motor.setSpeed(100);  // speed 설정

  Serial.println("Speed : 100rpm");
  Serial.print("Enter steps : ");
}

void loop() {
  if (Serial.available()) {
    int steps = Serial.parseInt();
    Serial.println(steps);

    int round = steps * 4096;

    motor.step(round);
    Serial.print("Enter steps : ");
  }
}