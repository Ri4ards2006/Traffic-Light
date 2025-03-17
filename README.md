# 🚦 Traffic-Light Simulation with Arduino

![Traffic-Light](https://cdn.pixabay.com/photo/2016/11/29/09/24/traffic-lights-1868599_1280.jpg)  
*A simulation of a busy intersection – programmed with Arduino C++.*

This project simulates a realistic intersection with multiple traffic lights and ultrasonic sensors to manage traffic and pedestrian flow. It uses an Arduino Mega 2560 and ESP-32 to control a physical model built on a 40x40 cm wooden board with 3D-printed component housings.

---

## 📑 Table of Contents

- [🌟 Features](#features)
- [🛠️ Technologies](#technologies)
- [🔧 Installation and Setup](#installation-and-setup)
- [🚀 Usage](#usage)
- [📂 Project Structure](#project-structure)
- [💻 Code Overview](#code-overview)
- [🛑 Challenges and Solutions](#challenges-and-solutions)
- [🔩 Hardware Setup Tips](#hardware-setup-tips)
- [📸 Visuals and Media](#visuals-and-media)
- [🚀 Future Enhancements](#future-enhancements)
- [🤝 Contributing](#contributing)
- [📚 Resources](#resources)
- [📜 License](#license)

---

## 🌟 Features

- **Intersection Simulation:**
  - 🚗 4 vehicle traffic lights (red, yellow, green LEDs)
  - 🚶 8 pedestrian traffic lights (red, green LEDs)
  - 📡 4 ultrasonic sensors (HC-SR04) for vehicle and pedestrian detection
- **Operation Modes:**
  - 🌞 **Day Mode:** Fixed cycles with pedestrian requests via buttons
  - 🌙 **Night Mode:** Sensor-driven dynamic control
  - 🚨 **Blinking Mode:** Activates during inactivity in night mode
- **Physical Model:**
  - Built on a 40x40 cm wooden board
  - 3D-printed housings for components
  - Realistic design with decorative elements like grass and trees
- **Debugging:** Serial output for real-time monitoring

---

## 🛠️ Technologies

### Software
| Technology            | Description                              |
|-----------------------|------------------------------------------|
| **Arduino IDE**       | For programming the microcontroller      |
| **C++ (Arduino)**     | Language for the firmware                |

### Hardware
| Component             | Quantity | Description                              |
|-----------------------|----------|------------------------------------------|
| **Arduino Mega 2560** | 1        | Main controller                          |
| **ESP-32**            | 1        | Additional control and connectivity      |
| **Traffic Light Modules** | 12   | 4 vehicle, 8 pedestrian lights           |
| **Ultrasonic Sensors (HC-SR04)** | 4 | Detect vehicles/pedestrians          |
| **Buttons**           | 5        | 4 pedestrian requests, 1 mode switch     |
| **Breadboards**       | 3        | Circuit prototyping                      |
| **Wooden Board**      | 1        | 40x40 cm base for the model             |
| **Decorative Items**  | Varies   | Grass, trees, and road elements          |

#### Arduino Mega 2560
![Arduino Mega 2560](https://upload.wikimedia.org/wikipedia/commons/thumb/3/38/Arduino_Mega_2560.jpg/800px-Arduino_Mega_2560.jpg)  
*The Arduino Mega 2560 used as the main controller for this project.*

---

## 🔧 Installation and Setup

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/Ri4ards2006/Traffic-Light.git
 

2. **Prepare Hardware:**
   - Connect traffic lights, sensors, and buttons to the Arduino Mega 2560. Refer to the circuit diagram in the `docs/` folder.
   - 3D print component housings using the STL files in the `design/` folder.
   - Assemble the model on a 40x40 cm wooden board, adding decorative elements.

3. **Upload Arduino Code:**
   - Open the Arduino IDE.
   - Load the main script from the `arduino/traffic_light.ino` file.
   - Select **Arduino Mega 2560** as the board and upload the code.

4. **Power On:**
   - Connect the Arduino to a power source (e.g., via USB or external power supply).
   - Ensure all components are securely connected.

---

## 🚀 Usage

1. **Start the System:**
   - Power on the Arduino Mega 2560 to activate the traffic lights and sensors.

2. **Interact with the Simulation:**
   - Use the mode switch button to toggle between 🌞 Day Mode and 🌙 Night Mode.
   - Press pedestrian request buttons to trigger pedestrian crossing phases.
   - Observe the traffic lights and sensor behavior in real-time.

3. **Monitor Output:**
   - Open the Serial Monitor in Arduino IDE (115200 baud) to view debug information, such as current mode, active phase, and sensor readings.

---

## 📂 Project Structure

```
traffic-light-simulation/
├── arduino/          # Arduino C++ scripts and circuit diagrams
│   └── traffic_light.ino    # Main Arduino firmware
├── design/           # 3D print files (STL) for component housings
├── docs/             # Circuit diagrams and additional documentation
└── README.md         # Project documentation
```

---

## 💻 Code Overview

The firmware is written in **C++ for Arduino** and runs on the Arduino Mega 2560. It controls the traffic lights, ultrasonic sensors, and buttons, implementing a state machine for day/night modes and pedestrian handling. Below is a detailed breakdown:

### 1. **Configuration Section**
- **Pin Assignments:** Maps pins to hardware components.
  ```cpp
  const uint8_t AMPELN[4][3] = {
    {22, 23, 24},  // North (Red, Yellow, Green)
    {25, 26, 27},  // South
    {28, 29, 30},  // East
    {31, 32, 33}   // West
  };
  const uint8_t FUSSGAENGER_LICHTER[4][2] = {
    {6, 7},   // North (Red, Green)
    {8, 9},   // South
    {10, 11}, // East
    {12, 13}  // West
  };
  ```
- **Timing Parameters:** Defines durations for phases (e.g., green phase: 10 seconds).
  ```cpp
  enum ZEITEN {
    TAGES_PHASEN[] = {10000, 3000, 10000, 3000}, // Green, Yellow, Green, Yellow
    NACHT_GRUEN = 15000,  // Night green phase
    FUSSG_GEHZEIT = 8000  // Pedestrian crossing time
  };
  ```
- **Sensor Settings:** Configures detection range and polling intervals.
  ```cpp
  #define SENSOR_AKTIV_DISTANZ 150  // Detect vehicles up to 150 cm
  #define SENSOR_UPDATE 500         // Sensor polling interval (ms)
  ```

### 2. **Global Variables**
- Tracks system state, including mode, phase, and sensor data.
  ```cpp
  enum MODI { TAG, NACHT };  // Day or Night mode
  volatile MODI aktueller_modus = TAG;
  uint8_t aktuelle_phase = 0;  // Current phase in day mode
  bool auto_erkannt[4] = {false};  // Vehicle detection states
  ```

### 3. **Setup Function**
- Initializes serial communication, sets initial light states, and configures interrupts for buttons.
  ```cpp
  void setup() {
    Serial.begin(115200);
    Serial.println(F("🚦 Systemstart 🚦"));
    ampeln_reseten();  // Set all lights to default (red)
    init_taster();     // Configure button interrupts
    Serial.println(F("System ready!"));
  }
  ```

### 4. **Main Loop**
- Manages mode-specific logic, pedestrian requests, and debug output.
  ```cpp
  void loop() {
    verwalte_modus();         // Handle day/night mode logic
    verarbeite_fussgaenger(); // Process pedestrian requests
    zeige_status();           // Output status to Serial Monitor
    delay(10);                // Prevent CPU overload
  }
  ```

### 5. **Day Mode Logic**
- Implements fixed cycles for traffic lights (e.g., North/South green, then East/West green).
  ```cpp
  void tagesmodus_logik() {
    switch(aktuelle_phase) {
      case 0:  // North/South green
        ampeln_setzen(0, GRUEN);
        ampeln_setzen(1, GRUEN);
        ampeln_setzen(2, ROT);
        ampeln_setzen(3, ROT);
        phasen_wechsel(TAGES_PHASEN[0]);
        break;
      // Additional phases for yellow, East/West green, etc.
    }
  }
  ```

### 6. **Night Mode Logic**
- Uses sensor data to dynamically control lights, with a blinking mode when no vehicles are detected.
  ```cpp
  void nachtmodus_logik() {
    sensor_update();  // Update sensor readings
    if (!autos_erkannt()) {
      blinkmodus_aktivieren();  // Blink yellow lights
      return;
    }
    aktive_richtung = naechste_richtung_finden();  // Find next direction with vehicle
    verarbeite_nachtzyklus(aktive_richtung);      // Set green for detected direction
  }
  ```

### 7. **Pedestrian Handling**
- Processes pedestrian requests with debouncing and manages crossing cycles.
  ```cpp
  void fussgaenger_phase_starten(uint8_t richtung) {
    ampeln_setzen(richtung, GELB);  // Yellow transition
    delay(2000);
    digitalWrite(FUSSGAENGER_LICHTER[richtung][GRUEN], HIGH);  // Pedestrian green
    delay(FUSSG_GEHZEIT);
    // Warning blink before resetting
  }
  ```

### 8. **Sensor Processing**
- Reads ultrasonic sensors to detect vehicles.
  ```cpp
  void sensor_update() {
    if (millis() - letzte_sensor_aktualisierung > SENSOR_UPDATE) {
      for (uint8_t i = 0; i < 4; i++) {
        auto_erkannt[i] = (SONAR[i].ping_cm() > 0 && SONAR[i].ping_cm() < SENSOR_AKTIV_DISTANZ);
      }
      letzte_sensor_aktualisierung = millis();
    }
  }
  ```

### 9. **Debugging**
- Outputs system status to Serial Monitor for monitoring.
  ```cpp
  void zeige_status() {
    static uint32_t letztes_update = 0;
    if (millis() - letztes_update > 1000) {
      Serial.println(F("\n=== Systemstatus ==="));
      Serial.print(F("Modus: "));
      Serial.println(aktueller_modus == TAG ? "TAG" : "NACHT");
      Serial.println(F("Sensorwerte:"));
      for (uint8_t i = 0; i < 4; i++) {
        Serial.print(i);
        Serial.print(F(": "));
        Serial.print(SONAR[i].ping_cm());
        Serial.print(F("cm "));
        Serial.println(auto_erkannt[i] ? " 🟢" : " 🔴");
      }
      letztes_update = millis();
    }
  }
  ```

---

## 🛑 Challenges and Solutions

| Challenge                            | Solution                                      |
|--------------------------------------|-----------------------------------------------|
| **Many components**                  | Prototyped on breadboards, then soldered directly for stability |
| **Button debouncing**                | Used interrupts with `millis()` for timing    |
| **Day/Night mode integration**       | Implemented a state machine for smooth transitions |
| **3D printing accuracy**             | Iterated designs to fit pre-made modules      |
| **Soldering issues**                 | Removed poor-quality pins and soldered wires directly |

---

## 🔩 Hardware Setup Tips

- **Power Supply:** The Arduino Mega 2560 can be powered via USB, but for stability with many LEDs, use an external 9V power supply.
- **Wiring:** Use color-coded wires (e.g., red for power, black for ground) to avoid confusion.
- **Sensor Placement:** Position ultrasonic sensors to avoid interference; ensure they have a clear line of sight.
- **Soldering:** Test connections with a multimeter before final assembly to avoid short circuits.

---

## 📸 Visuals and Media

### Arduino Mega 2560
![Arduino Mega 2560](https://upload.wikimedia.org/wikipedia/commons/thumb/3/38/Arduino_Mega_2560.jpg/800px-Arduino_Mega_2560.jpg)  
*The heart of the project: Arduino Mega 2560 controlling the traffic lights.*

### Traffic Light Setup
![Traffic Light Model](https://cdn.pixabay.com/photo/2016/11/29/09/24/traffic-lights-1868599_1280.jpg)  
*Example of a traffic light setup similar to the project.*

### Ultrasonic Sensor (HC-SR04)
![HC-SR04 Sensor](https://upload.wikimedia.org/wikipedia/commons/thumb/5/54/HC-SR04.jpg/220px-HC-SR04.jpg)  
*HC-SR04 ultrasonic sensor used for vehicle detection.*

---

## 🚀 Future Enhancements

- **Dynamic Timing:** Allow runtime adjustment of phase durations via a potentiometer or serial input.
- **AI Optimization:** Integrate machine learning to optimize traffic flow based on sensor data.
- **Wireless Control:** Use the ESP-32 for remote control via Wi-Fi or Bluetooth.
- **Realistic Visuals:** Add more detailed road markings and traffic signs to the model.

---

## 🤝 Contributing

Contributions are welcome! To contribute:
1. Fork the repository.
2. Create a new branch for your changes.
3. Submit a pull request with a detailed description.

---

## 📚 Resources

- [Traffic Light Control with Arduino](https://www.geeksforgeeks.org/traffic-light-control-project-using-arduino/)
- [Arduino Traffic Light Controller Tutorial](https://www.instructables.com/Arduino-Traffic-Light-Controller/)
- [Crossroad Traffic Lights with Arduino](https://www.hackster.io/43949/arduino-crossroad-and-traffic-lights-342e82)
- [Arduino Traffic Light Simulation Guide](https://www.electronicshub.org/arduino-traffic-light-controller/)
- [HC-SR04 Ultrasonic Sensor Guide](https://randomnerdtutorials.com/complete-guide-for-ultrasonic-sensor-hc-sr04/)
- [Arduino Mega 2560 Documentation](https://www.arduino.cc/en/Main/ArduinoBoardMega2560)

---

## 📜 License

This project is licensed under the MIT License – see [LICENSE](LICENSE) for details.

---

### Changes Made
-
