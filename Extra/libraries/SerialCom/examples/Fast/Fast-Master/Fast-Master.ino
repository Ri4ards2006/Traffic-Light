#include <Arduino.h>
#include "params.h"
#include "LiquidCrystal.h"
#include "MenuOption.h"
#include "LCDMenu.h"
#include "SerialManager.h"

const int rs = 12, en = 11, d4 = 6, d5 = 7, d6 = 8, d7 = 9;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LCDMenu<1> menu(&lcd);

Data input = {{0, true}, {0, true}, {0, true}, {0, true}};
Data output = {{0, true}, {0, true}, {0, true}, {0, true}};
SerialManager manager(input, output, NONE, FAST);




void setup() {
  // put your setup code here, to run once:
  menu.getMenuEntry(0).init("MASTER" ,"", 0);

  lcd.begin(16, 2);
  lcd.clear();
  delay(500); 

  menu.print();
  Serial.begin(SERIAL_BAUD);
}

void loop() {
  //Serial.println("Arduino nano 328P new Bootloader");
  //Serial.println("Test");
  menu.getMenuEntry(0).setValue(input.flow_expi.value);
  manager.update();

  ++input.flow_expi.value;
  delay(1000);
}