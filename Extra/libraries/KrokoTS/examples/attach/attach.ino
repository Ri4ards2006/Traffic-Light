#define MAX_FIELDS 2

#include<KrokoTS.h>

#define X1 A0
#define X2 A2
#define Y1 A1
#define Y2 A3
#define LED 11

KrokoTS Touch(X1, X2, Y1, Y2);

void setup() {
  pinMode(LED, OUTPUT);
  Touch.attachClick(0, 1, 500, 1, 900, on);
  Touch.attachClick(1, 501, 900, 1, 900, off);

}

void loop() {
  Touch.tick();
}

void on() {
  digitalWrite(LED, HIGH);
}

void off() {
  digitalWrite(LED, LOW);
}