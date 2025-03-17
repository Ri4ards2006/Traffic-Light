# SerialCom

## Sections

- [Installation](#installation)
  - [ArduinoIDE](#arduinoide)
  - [PlatformIO (recommended)](#platformio)
- [Usage](#usage)
- [Code](#code)
  - [Data](#data)
  - [SerialManger](#serialmanager)

---

## Installation

### ArduinoIDE

1. Create a folder named "SerialCom"
1. Copy all files from the src subdirectory into the new folder.

    ```C
    |--SerialCom
    |  |- Communication.h
    |  |- Data.h
    |  |- SerialManager.cpp
    |  |- SerialManager.h
    |  |- README --> THIS FILE
    ```

1. zip it
1. Open ArduinoIDE -> Sketch -> Include library -> Add ZIP Library...
1. Test by checking: Sketch -> Include library -> SerialCom

### PlatformIO

Copy and past the hole folder in your lib directory:

```C

|--lib
|  |
|  |--SerialCom
|  |  |--examples
|  |     |--Fast
|  |        |- Master.cpp
|  |        |- params.h
|  |        |- Slave.h
|  |     |--Secure
|  |        |- Master.cpp
|  |        |- params.h
|  |        |- Slave.h
|  |  |--src
|  |     |- Communication.h
|  |     |- Data.h
|  |     |- SerialManager.cpp
|  |     |- SerialManager.h
|  |  |- README --> THIS FILE
|
|- platformio.ini
|--src
   |- main.c

```

---

## Usage

### What you need

- at least two Arduinos (can be different models)
- 2 cables for
- a power suply

### Circuit

![circuit for communication Arduino-Arduino  through the serial ports (RX and TX)](https://robotic-controls.com/sites/default/files/learn/Arduino-ArduinoSerial.png)

### Example

We will use to control the onboard LED on the other board.

First change go to data and 

```C
/*
 *	vvv Change Data to fit your need! vvv
 */

using PIN_status = CheckedValue<HIGH>;

struct Data
{
    Pressure press_expi;
    Pressure press_inspi;
    Flow flow_expi;
    Flow flow_inspi;
};

/*
 *	^^^ Change Data to fit your need! ^^^
 */
```


```C
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

```

---

## Code

### Data

Data is the only file in the library you can change to adape the Data struct for your own popuse.

### SerialManager

Main library to include.

---

## License

Copyright © 2006-2022 Willi Zschiebsch. All rights reserved. Copyright (c) 2010 Arduino LLC. All right reserved.

This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
