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

// Taster-Objekte für jede Ampelphase
Button buttons[4] = {
    Button(2),  // Button für Ampel 1
    Button(3),  // Button für Ampel 2
    Button(4),  // Button für Ampel 3
    Button(5)   // Button für Ampel 4
};

// Ampeln für Fahrzeuge (Nord, Süd, Ost, West)
TrafficLight trafficLights[4] = {
    TrafficLight(6, 7, 8),   // Ampel 1 (Nord)
    TrafficLight(9, 10, 11), // Ampel 2 (Süd)
    TrafficLight(12, 13, 14),// Ampel 3 (Ost)
    TrafficLight(15, 16, 17) // Ampel 4 (West)
};

// Fußgängerampeln (jeweils für jede Richtung)
PedestrianLight pedestrianLights[4][2] = {
    {PedestrianLight(18, 19), PedestrianLight(20, 21)}, // Fußgänger Nord
    {PedestrianLight(22, 23), PedestrianLight(24, 25)}, // Fußgänger Süd
    {PedestrianLight(26, 27), PedestrianLight(28, 29)}, // Fußgänger Ost
    {PedestrianLight(30, 31), PedestrianLight(32, 33)}  // Fußgänger West
};

// Variable für die Ampelphasen
int phase = 1;

// Setup-Funktion
void setup() {
    Serial.begin(9600);

    // Alle Fußgängerampeln standardmäßig auf ROT setzen
    for (int i = 0; i < 4; i++) {
        pedestrianLights[i][0].setRed();
        pedestrianLights[i][1].setRed();
    }

    // Alle Verkehrampeln auf ROT setzen
    for (int i = 0; i < 4; i++) {
        trafficLights[i].setRed();
    }
}

// Loop-Funktion
void loop() {
    // Tasterzustände auslesen
    for (int i = 0; i < 4; i++) {
        buttons[i].readState();  // Entprellung für jeden Taster
    }

    // Logik für die Ampelphasen
    switch (phase) {
        case 1:
            // Phase 1: A1 (Nord) und A2 (Süd) grün, A3 (Ost) und A4 (West) rot
            trafficLights[0].setGreen();
            trafficLights[1].setGreen();
            trafficLights[2].setRed();
            trafficLights[3].setRed();
            updatePedestrianLights();
            phase = 2;
            break;

        case 2:
            // Phase 2: A1 und A2 gelb, A3 und A4 rot
            trafficLights[0].setYellow();
            trafficLights[1].setYellow();
            trafficLights[2].setRed();
            trafficLights[3].setRed();
            phase = 3;
            break;

        case 3:
            // Phase 3: A1 und A2 rot, A3 (Ost) und A4 (West) grün
            trafficLights[0].setRed();
            trafficLights[1].setRed();
            trafficLights[2].setGreen();
            trafficLights[3].setGreen();
            updatePedestrianLights();
            phase = 4;
            break;

        case 4:
            // Phase 4: A3 und A4 gelb, A1 und A2 rot
            trafficLights[0].setRed();
            trafficLights[1].setRed();
            trafficLights[2].setYellow();
            trafficLights[3].setYellow();
            phase = 1;
            break;
    }

    displayTrafficLights();  // Zeige den Zustand in der Konsole an
    delay(1000);  // Eine Sekunde warten, um die Simulation sichtbar zu machen
}

// Fußgängerampeln basierend auf den Tastern aktualisieren
void updatePedestrianLights() {
    for (int i = 0; i < 4; i++) {
        if (buttons[i].state == LOW) {  // Taster gedrückt
            pedestrianLights[i][0].setGreen();  // Fußgänger dürfen gehen
            pedestrianLights[i][1].setRed();   // Fußgänger an der anderen Seite warten
        } else {
            pedestrianLights[i][0].setRed();   // Fußgänger dürfen nicht gehen
            pedestrianLights[i][1].setGreen(); // Fußgänger an der anderen Seite gehen
        }
    }
}

// Funktion zum Anzeigen des Ampelstatus im Seriellen Monitor
void displayTrafficLights() {
    Serial.println("Phase " + String(phase));
    for (int i = 0; i < 4; i++) {
        Serial.print("A" + String(i + 1) + ": ");
        if (digitalRead(trafficLights[i].greenPin) == HIGH) {
            Serial.println("🟩");
        } else if (digitalRead(trafficLights[i].yellowPin) == HIGH) {
            Serial.println("🟨");
        } else {
            Serial.println("🟥");
        }
    }

    for (int i = 0; i < 4; i++) {
        Serial.print("Pedestrian group " + String(i + 1) + ": ");
        if (digitalRead(pedestrianLights[i][0].greenPin) == HIGH) {
            Serial.println("🚶‍♂️🟩");
        } else {
            Serial.println("🚶‍♂️🟥");
        }
    }
}
