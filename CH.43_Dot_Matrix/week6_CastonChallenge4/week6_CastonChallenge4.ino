#include <FrequencyTimer2.h>

#define I \
  { \
    { 0, 0, 0, 0, 0, 0, 0, 0 }, \
      { 0, 0, 0, 0, 0, 0, 0, 0 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 0, 0, 0, 0, 0, 0, 0, 0 }, \
      { 0, 0, 0, 0, 0, 0, 0, 0 }, \
  }

#define N \
  { \
    { 1, 1, 1, 1, 1, 1, 1, 1 }, \
      { 0, 0, 0, 0, 0, 0, 1, 0 }, \
      { 0, 0, 0, 0, 0, 1, 0, 0 }, \
      { 0, 0, 0, 0, 1, 0, 0, 0 }, \
      { 0, 0, 0, 1, 0, 0, 0, 0 }, \
      { 0, 0, 1, 0, 0, 0, 0, 0 }, \
      { 0, 1, 0, 0, 0, 0, 0, 0 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
  }



#define D \
  { \
    { 0, 0, 0, 1, 1, 0, 0, 0 }, \
      { 0, 0, 1, 0, 0, 1, 0, 0 }, \
      { 0, 1, 0, 0, 0, 0, 1, 0 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
  }


#define U \
  { \
    { 1, 1, 1, 1, 1, 0, 0, 0 }, \
      { 0, 0, 0, 0, 0, 1, 0, 0 }, \
      { 0, 0, 0, 0, 0, 0, 1, 0 }, \
      { 0, 0, 0, 0, 0, 0, 0, 1 }, \
      { 0, 0, 0, 0, 0, 0, 0, 1 }, \
      { 0, 0, 0, 0, 0, 0, 1, 0 }, \
      { 0, 0, 0, 0, 0, 1, 0, 0 }, \
      { 1, 1, 1, 1, 1, 0, 0, 0 }, \
  }


#define K \
  { \
    { 0, 0, 0, 0, 0, 0, 0, 0 }, \
      { 0, 0, 0, 0, 0, 0, 0, 0 }, \
      { 1, 0, 0, 0, 0, 0, 0, 1 }, \
      { 0, 1, 0, 0, 0, 0, 1, 0 }, \
      { 0, 0, 1, 0, 0, 1, 0, 0 }, \
      { 0, 0, 0, 1, 1, 0, 0, 0 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
  }


#define M \
  { \
    { 1, 1, 1, 1, 1, 1, 1, 1 }, \
      { 1, 0, 0, 0, 0, 0, 0, 0 }, \
      { 0, 1, 0, 0, 0, 0, 0, 0 }, \
      { 0, 0, 1, 0, 0, 0, 0, 0 }, \
      { 0, 0, 1, 0, 0, 0, 0, 0 }, \
      { 0, 1, 0, 0, 0, 0, 0, 0 }, \
      { 1, 0, 0, 0, 0, 0, 0, 0 }, \
      { 1, 1, 1, 1, 1, 1, 1, 1 }, \
  }


#define LOVE \
  { \
    { 0, 1, 1, 1, 0, 0, 0, 0 }, \
      { 1, 0, 0, 0, 1, 0, 0, 0 }, \
      { 0, 1, 0, 0, 0, 1, 0, 0 }, \
      { 0, 0, 1, 0, 0, 0, 1, 0 }, \
      { 0, 0, 1, 0, 0, 0, 1, 0 }, \
      { 0, 1, 0, 0, 0, 1, 0, 0 }, \
      { 1, 0, 0, 0, 1, 0, 0, 0 }, \
      { 0, 1, 1, 1, 0, 0, 0, 0 }, \
  }

int pin[17] = { -1, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37 };

int col[8] = { pin[9], pin[10], pin[11], pin[12], pin[13], pin[14], pin[15], pin[16] };

int row[8] = { pin[1], pin[2], pin[3], pin[4], pin[5], pin[6], pin[7], pin[8] };

int sw = 2;
byte columnIndex = 0;  // Column Index Variable
byte LED[8][8];

int pattern = 0;

const int Patterns_no = 7;        // Total Pattern Number
byte patterns[Patterns_no][8][8]  // Pattern
  = { I, N, D, U, K, M, LOVE };



void setup() {
  for (int i = 1; i <= 16; i++) {
    pinMode(pin[i], OUTPUT);
  }

  for (int i = 0; i < 8; i++) {
    digitalWrite(row[i], LOW);
    digitalWrite(col[i], HIGH);
  }

  // setOnOverflow를 사용하는 Frequency Timer Interrupt
  FrequencyTimer2::setOnOverflow(display);

  pinMode(sw, INPUT);

  Pattern_output(pattern);
}

void loop() {
  // row : high, col : low 일때 led On!
  int sw_status = digitalRead(sw);

  if (sw_status == HIGH) {
    pattern++;
    if (pattern == 7) {
      pattern = 0;
    } else {
      delay(300);
    }
  }

  Pattern_output(pattern);
}

void Pattern_output(int pattern) {
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      LED[i][j] = patterns[pattern][i][j];
    }
  }
}

// Interrupt Service Routine
void display() {

  digitalWrite(col[columnIndex], HIGH);
  columnIndex++;

  if (columnIndex == 8) {
    columnIndex = 0;
  }

  for (int row1 = 0; row1 < 8; row1++) {
    if (LED[columnIndex][7 - row1] == 1) {
      digitalWrite(row[row1], HIGH);
    } else {
      digitalWrite(row[row1], LOW);
    }
  }

  digitalWrite(col[columnIndex], LOW);
}