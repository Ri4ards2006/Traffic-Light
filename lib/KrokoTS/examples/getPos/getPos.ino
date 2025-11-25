#include<KrokoTS.h>

#define X1 A0
#define X2 A2
#define Y1 A1
#define Y2 A3
#define LED 11

KrokoTS Touch(X1, X2, Y1, Y2);

void setup() {
  pinMode(LED, OUTPUT);

}

void loop() {
  int x = Touch.getX();
  int y = Touch.getY();
  if (x < 1000 && y < 1000) {
    if (y < 500) digitalWrite(LED, (x > 500) ? HIGH : LOW);
    else analogWrite(LED, map(x, 50, 950, 0, 255));

  }
}