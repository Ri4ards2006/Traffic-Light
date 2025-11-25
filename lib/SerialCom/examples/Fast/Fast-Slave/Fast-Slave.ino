#include <Arduino.h>
#include "params.h"
#include "LiquidCrystal.h"
#include "MenuOption.h"
#include "LCDMenu.h"
#include "SerialManager.h"

const int rs = 12, en = 11, d4 = 7, d5 = 6, d6 = 5, d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LCDMenu<1> menu(&lcd);

Data sendData = {{0, true}, {0, true}, {0, true}, {0, true}};
Data receiveData = {{0, true}, {0, true}, {0, true}, {0, true}};

SerialManager manager(Serial, receiveData, sendData, SECURE, NONE);

void setup() {
  // put your setup code here, to run once:
  menu.getMenuEntry(0).init("SLAVE" ,"", 0);
  lcd.begin(16, 2);
  lcd.clear();
  delay(500); 

  menu.print();
  Serial.begin(SERIAL_BAUD);
}

void loop() {
  //Serial.println("Arduino UNO");
  receiveData.flow_expi.value=0;
  manager.update();
  menu.setValue((double)receiveData.flow_expi.value);
}