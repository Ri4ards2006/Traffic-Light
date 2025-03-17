// Konstanten für die Pin-Nummern der Taster
const int buttonPins[4] = {28, 25, 27, 30}; // Taster an den Pins

// Gruppen von Pins für die Ampeln (jeweils 4 Pins pro Gruppe)
const int pedestrianGroups[4][4] = {
  {52, 53, 50, 51},  // Gruppe 1: gerade Pins (52, 50), ungerade Pins (53, 51)
  {48, 49, 46, 47},  // Gruppe 2: gerade Pins (48, 46), ungerade Pins (49, 47)
  {44, 45, 42, 43},  // Gruppe 3: gerade Pins (44, 42), ungerade Pins (45, 43)
  {40, 41, 38, 39}   // Gruppe 4: gerade Pins (40, 38), ungerade Pins (41, 39)
};

// Variablen für die Tasterzustände
int buttonStates[4] = {0, 0, 0, 0};

void setup() {
  // Taster als Eingänge mit Pull-Up-Widerständen setzen
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Alle Pins (in allen Gruppen) als Ausgänge konfigurieren und auf AN (ROT) setzen
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      pinMode(pedestrianGroups[i][j], OUTPUT);
      digitalWrite(pedestrianGroups[i][j], HIGH); // Standardzustand: AN
    }
  }

  // Serielle Kommunikation starten
  Serial.begin(9600);
}

void loop() {
  // Tasterzustände auslesen
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);

    if (buttonStates[i] == HIGH) {
      // Taster i gedrückt: Gerade Pins aus, ungerade Pins an
      digitalWrite(pedestrianGroups[i][0], LOW);  // Gerade Pin 1 aus
      digitalWrite(pedestrianGroups[i][2], LOW);  // Gerade Pin 2 aus
      digitalWrite(pedestrianGroups[i][1], HIGH); // Ungerade Pin 1 an
      digitalWrite(pedestrianGroups[i][3], HIGH); // Ungerade Pin 2 an

      Serial.print("Taster ");
      Serial.print(i + 1);
      Serial.println(" gedrückt: Gerade aus, Ungerade an.");
    } else {
      // Taster i nicht gedrückt: Alle Pins auf ROT
      digitalWrite(pedestrianGroups[i][0], HIGH); // Gerade Pin 1 an
      digitalWrite(pedestrianGroups[i][2], HIGH); // Gerade Pin 2 an
      digitalWrite(pedestrianGroups[i][1], LOW);  // Ungerade Pin 1 aus
      digitalWrite(pedestrianGroups[i][3], LOW);  // Ungerade Pin 2 aus

      Serial.print("Taster ");
      Serial.print(i + 1);
      Serial.println(" nicht gedrückt: Alles auf ROT.");
    }
  }

  // Abstand für Lesbarkeit
  Serial.println("=========================");
  delay(500); // Pause zwischen Updates
}
