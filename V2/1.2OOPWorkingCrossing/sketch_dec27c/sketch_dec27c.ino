// Klasse für einen Taster mit Entprellung
class Button {
public:
    int pin;
    int state;
    int lastButtonState = HIGH;  // Der Ausgangszustand des Tasters
    unsigned long lastDebounceTime = 0;  // Die Zeit, wann der Taster zuletzt gewechselt hat
    unsigned long debounceDelay = 50;  // Entprellverzögerung in Millisekunden

    Button(int bPin) {
        pin = bPin;
        pinMode(pin, INPUT_PULLUP);  // Pull-up-Widerstand aktivieren
    }

    int readState() {
        int reading = digitalRead(pin);

        // Wenn der Tasterzustand sich geändert hat
        if (reading != lastButtonState) {
            lastDebounceTime = millis();  // Zeit speichern, wenn der Tasterzustand geändert wurde
        }

        // Überprüfen, ob die Entprellzeit abgelaufen ist
        if ((millis() - lastDebounceTime) > debounceDelay) {
            // Wenn der Zustand sich geändert hat und der Entprellungszeitraum abgelaufen ist
            if (reading != state) {
                state = reading;  // Den neuen Zustand setzen
            }
        }

        lastButtonState = reading;  // Den letzten Zustand speichern
        return state;
    }
};

// Klasse für Ampeln
class TrafficLight {
public:
    int redPin;
    int yellowPin;
    int greenPin;

    TrafficLight(int rPin, int yPin, int gPin) {
        redPin = rPin;
        yellowPin = yPin;
        greenPin = gPin;
        pinMode(redPin, OUTPUT);
        pinMode(yellowPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
    }

    void setRed() {
        digitalWrite(redPin, HIGH);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, LOW);
    }

    void setYellow() {
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, HIGH);
        digitalWrite(greenPin, LOW);
    }

    void setGreen() {
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, HIGH);
    }

    void turnOff() {
        digitalWrite(redPin, LOW);
        digitalWrite(yellowPin, LOW);
        digitalWrite(greenPin, LOW);
    }
};

// Klasse für Fußgängerampeln
class PedestrianLight {
public:
    int redPin;
    int greenPin;

    PedestrianLight(int rPin, int gPin) {
        redPin = rPin;
        greenPin = gPin;
        pinMode(redPin, OUTPUT);
        pinMode(greenPin, OUTPUT);
    }

    void setRed() {
        digitalWrite(redPin, HIGH);
        digitalWrite(greenPin, LOW);
    }

    void setGreen() {
        digitalWrite(redPin, LOW);
        digitalWrite(greenPin, HIGH);
    }
};

// Konstanten für die Pin-Nummern
const int buttonPins[4] = {28, 25, 27, 30}; // Taster an den Pins
Button buttons[4] = {Button(28), Button(25), Button(27), Button(30)}; // Button Objekte

TrafficLight A1(2, 3, 4); // Ampel A1: Rot, Gelb, Grün
TrafficLight A2(5, 6, 7); // Ampel A2: Rot, Gelb, Grün
TrafficLight A3(8, 9, 10); // Ampel A3: Rot, Gelb, Grün
TrafficLight A4(11, 12, 13); // Ampel A4: Rot, Gelb, Grün

PedestrianLight pedestrianGroups[4][2] = {
    {PedestrianLight(52, 53), PedestrianLight(50, 51)}, // Gruppe 1
    {PedestrianLight(48, 49), PedestrianLight(46, 47)}, // Gruppe 2
    {PedestrianLight(44, 45), PedestrianLight(42, 43)}, // Gruppe 3
    {PedestrianLight(40, 41), PedestrianLight(38, 39)}  // Gruppe 4
};

// Variable für die Ampelphasen
int phase = 1; // Startphase

void setup() {
    // Serielle Kommunikation starten
    Serial.begin(9600);
}

void loop() {
    // Tasterzustände auslesen
    for (int i = 0; i < 4; i++) {
        buttons[i].readState();
    }

    // Logik für die Ampelphasen
    switch (phase) {
        case 1:
            // Phase 1: A1 (Nord) und A2 (Süd) grün, A3 (Ost) und A4 (West) rot
            A1.setGreen();
            A2.setGreen();
            A3.setRed();
            A4.setRed();

            // Fußgängerampeln aktivieren, wenn Taster gedrückt
            for (int i = 0; i < 4; i++) {
                if (buttons[i].state == LOW) {
                    pedestrianGroups[i][0].setGreen();  // Fußgängerampel grün
                    pedestrianGroups[i][1].setGreen();  // Fußgängerampel grün
                } else {
                    pedestrianGroups[i][0].setRed();   // Fußgängerampel rot
                    pedestrianGroups[i][1].setRed();   // Fußgängerampel rot
                }
            }
            phase = 2; // Wechsel zu Phase 2
            break;

        case 2:
            // Phase 2: A1 und A2 gelb, A3 und A4 rot
            A1.setYellow();
            A2.setYellow();
            A3.setRed();
            A4.setRed();
            phase = 3; // Wechsel zu Phase 3
            break;

        case 3:
            // Phase 3: A1 und A2 rot, A3 (Ost) und A4 (West) grün
            A1.setRed();
            A2.setRed();
            A3.setGreen();
            A4.setGreen();

            // Fußgängerampeln aktivieren, wenn Taster gedrückt
            for (int i = 0; i < 4; i++) {
                if (buttons[i].state == LOW) {
                    pedestrianGroups[i][0].setGreen();  // Fußgängerampel grün
                    pedestrianGroups[i][1].setGreen();  // Fußgängerampel grün
                } else {
                    pedestrianGroups[i][0].setRed();   // Fußgängerampel rot
                    pedestrianGroups[i][1].setRed();   // Fußgängerampel rot
                }
            }
            phase = 4; // Wechsel zu Phase 4
            break;

        case 4:
            // Phase 4: A3 und A4 gelb, A1 und A2 rot
            A1.setRed();
            A2.setRed();
            A3.setYellow();
            A4.setYellow();
            phase = 1; // Zurück zu Phase 1
            break;
    }

    // Zeige den aktuellen Zustand der Ampeln in der Konsole an
    displayTrafficLights();
    delay(1000); // Pause für einen Übergang (1 Sekunde)
}

void displayTrafficLights() {
    // Die Ampeln für jede Phase in der Konsole anzeigen
    Serial.println("\nPhase 1: ");
    Serial.println("A1: 🟩  A2: 🟩  A3: 🟥  A4: 🟥");
    displayPedestrianLights(0);  // Fußgängerampeln für Gruppe 1

    delay(1000);

    Serial.println("\nPhase 2: ");
    Serial.println("A1: 🟨  A2: 🟨  A3: 🟥  A4: 🟥");
    displayPedestrianLights(1);  // Fußgängerampeln für Gruppe 2

    delay(1000);

    Serial.println("\nPhase 3: ");
    Serial.println("A1: 🟥  A2: 🟥  A3: 🟩  A4: 🟩");
    displayPedestrianLights(2);  // Fußgängerampeln für Gruppe 3

    delay(1000);

    Serial.println("\nPhase 4: ");
    Serial.println("A1: 🟥  A2: 🟥  A3: 🟨  A4: 🟨");
    displayPedestrianLights(3);  // Fußgängerampeln für Gruppe 4

    delay(1000);
}

void displayPedestrianLights(int groupIndex) {
    // Fußgängerampeln anzeigen
    if (buttons[groupIndex].state == LOW) {
        // Fußgängerampeln auf grün für die Gruppe setzen
        Serial.println("Fußgängerampeln grün");
    } else {
        // Fußgängerampeln auf rot setzen
        Serial.println("Fußgängerampeln rot");
    }
}
