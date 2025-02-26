// Konstanten für die Pin-Nummern der Taster
const int buttonPins[4] = {28, 25, 27, 30}; // Taster an den Pins

// Pins für Fußgängerampeln (jede Gruppe hat 4 Ampeln)
const int pedestrianGroups[4][4] = {
  {53, 52, 51, 50},  // Ampelgruppe 1
  {49, 48, 47, 46},  // Ampelgruppe 2
  {45, 44, 43, 42},  // Ampelgruppe 3
  {41, 40, 39, 38}   // Ampelgruppe 4
};

// Variablen für die Tasterzustände
int buttonStates[4] = {0, 0, 0, 0};

void setup() {
  // Taster als Eingänge mit Pull-Up-Widerständen setzen
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Fußgängerampeln als Ausgänge definieren und auf AUS (LOW) setzen
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      pinMode(pedestrianGroups[i][j], OUTPUT);
      digitalWrite(pedestrianGroups[i][j], LOW); // Ampeln aus
    }
  }

  // Serielle Kommunikation starten
  Serial.begin(9600);
}

void loop() {
  // Tasterzustände auslesen und entsprechende Ampelgruppe steuern
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);

    // Steuerung der Ampelgruppe basierend auf Tasterzustand
    if (buttonStates[i] == HIGH) {
      // Taster gedrückt: Alle Ampeln der Gruppe auf GRÜN
      for (int j = 0; j < 4; j++) {
        digitalWrite(pedestrianGroups[i][j], HIGH); // Ampel an
      }
      Serial.print("Ampelgruppe ");
      Serial.print(i + 1);
      Serial.println(": GRÜN (Taster gedrückt)");
    } else {
      // Taster nicht gedrückt: Alle Ampeln der Gruppe auf ROT
      for (int j = 0; j < 4; j++) {
        digitalWrite(pedestrianGroups[i][j], LOW); // Ampel aus
      }
      Serial.print("Ampelgruppe ");
      Serial.print(i + 1);
      Serial.println(": ROT (Taster nicht gedrückt)");
    }
  }

  // Abstand für Lesbarkeit
  Serial.println("=========================");
  delay(1000); // Pause zwischen Updates
}
