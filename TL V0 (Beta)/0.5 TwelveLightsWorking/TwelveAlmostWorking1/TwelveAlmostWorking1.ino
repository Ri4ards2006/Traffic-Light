// Konstanten für die Pin-Nummern der Taster
const int buttonPins[4] = {28, 25, 27, 30}; // Taster an den Pins

// Gruppen von Pins für die Fußgängerampeln (jeweils 4 Pins pro Gruppe)
const int pedestrianGroups[4][4] = {
  {52, 53, 50, 51},  // Gruppe 1: gerade Pins (52, 50), ungerade Pins (53, 51)
  {48, 49, 46, 47},  // Gruppe 2: gerade Pins (48, 46), ungerade Pins (49, 47)
  {44, 45, 42, 43},  // Gruppe 3: gerade Pins (44, 42), ungerade Pins (45, 43)
  {40, 41, 38, 39}   // Gruppe 4: gerade Pins (40, 38), ungerade Pins (41, 39)
};

// Konstanten für die Pin-Nummern der Hauptampeln (Rot, Gelb, Grün)
const int A1_red = 2;
const int A1_yellow = 3;
const int A1_green = 4;
const int A2_red = 5;
const int A2_yellow = 6;
const int A2_green = 7;
const int A3_red = 8;
const int A3_yellow = 9;
const int A3_green = 10;
const int A4_red = 11;
const int A4_yellow = 12;
const int A4_green = 13;

// Variablen für die Tasterzustände
int buttonStates[4] = {0, 0, 0, 0};

// Die Phasen der Fahrzeugampeln für die Kreuzung (0 bis 3)
int phase = 0;

void setup() {
  // Taster als Eingänge mit Pull-Up-Widerständen setzen
  for (int i = 0; i < 4; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  // Alle Pins für die Fußgängerampeln als Ausgänge konfigurieren und auf ROT setzen
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      pinMode(pedestrianGroups[i][j], OUTPUT);
      digitalWrite(pedestrianGroups[i][j], HIGH); // Standardzustand: ROT
    }
  }

  // Alle Pins für die Hauptampeln als Ausgänge konfigurieren und auf ROT setzen
  pinMode(A1_red, OUTPUT);
  pinMode(A1_yellow, OUTPUT);
  pinMode(A1_green, OUTPUT);
  pinMode(A2_red, OUTPUT);
  pinMode(A2_yellow, OUTPUT);
  pinMode(A2_green, OUTPUT);
  pinMode(A3_red, OUTPUT);
  pinMode(A3_yellow, OUTPUT);
  pinMode(A3_green, OUTPUT);
  pinMode(A4_red, OUTPUT);
  pinMode(A4_yellow, OUTPUT);
  pinMode(A4_green, OUTPUT);

  // Alle Hauptampeln auf ROT setzen (Initialzustand)
  digitalWrite(A1_red, HIGH);
  digitalWrite(A1_yellow, LOW);
  digitalWrite(A1_green, LOW);

  digitalWrite(A2_red, HIGH);
  digitalWrite(A2_yellow, LOW);
  digitalWrite(A2_green, LOW);

  digitalWrite(A3_red, HIGH);
  digitalWrite(A3_yellow, LOW);
  digitalWrite(A3_green, LOW);

  digitalWrite(A4_red, HIGH);
  digitalWrite(A4_yellow, LOW);
  digitalWrite(A4_green, LOW);

  // Serielle Kommunikation starten
  Serial.begin(9600);
}

void loop() {
  // Tasterzustände auslesen und Fußgängerampeln entsprechend steuern
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);

    if (buttonStates[i] == HIGH) {
      // Taster i gedrückt: Fußgängerampeln für diese Gruppe auf Grün
      digitalWrite(pedestrianGroups[i][0], LOW);  // Gerade Pin 1 aus
      digitalWrite(pedestrianGroups[i][2], LOW);  // Gerade Pin 2 aus
      digitalWrite(pedestrianGroups[i][1], HIGH); // Ungerade Pin 1 an
      digitalWrite(pedestrianGroups[i][3], HIGH); // Ungerade Pin 2 an

      Serial.print("Taster ");
      Serial.print(i + 1);
      Serial.println(" gedrückt: Fußgängerampeln auf Grün.");
    } else {
      // Taster i nicht gedrückt: Fußgängerampeln auf Rot
      digitalWrite(pedestrianGroups[i][0], HIGH); // Gerade Pin 1 an
      digitalWrite(pedestrianGroups[i][2], HIGH); // Gerade Pin 2 an
      digitalWrite(pedestrianGroups[i][1], LOW);  // Ungerade Pin 1 aus
      digitalWrite(pedestrianGroups[i][3], LOW);  // Ungerade Pin 2 aus

      Serial.print("Taster ");
      Serial.print(i + 1);
      Serial.println(" nicht gedrückt: Fußgängerampeln auf Rot.");
    }
  }

  // Kreuzungslogik für die Fahrzeugampeln
  // Jede Phase lässt zwei Ampeln gleichzeitig schalten, während die anderen rot bleiben.
  switch (phase) {
    case 0:  // Phase 0: Fahrtrichtung 1 und 2 grün
      // Richtung 1 und 2 grün
      digitalWrite(A1_red, LOW);
      digitalWrite(A1_yellow, LOW);
      digitalWrite(A1_green, HIGH);
      digitalWrite(A2_red, LOW);
      digitalWrite(A2_yellow, LOW);
      digitalWrite(A2_green, HIGH);

      // Richtung 3 und 4 rot
      digitalWrite(A3_red, HIGH);
      digitalWrite(A3_yellow, LOW);
      digitalWrite(A3_green, LOW);
      digitalWrite(A4_red, HIGH);
      digitalWrite(A4_yellow, LOW);
      digitalWrite(A4_green, LOW);
      break;

    case 1:  // Phase 1: Richtung 1 und 2 Gelb -> Richtung 3 und 4 grün
      // Richtung 1 und 2 gelb
      digitalWrite(A1_red, LOW);
      digitalWrite(A1_yellow, HIGH);
      digitalWrite(A1_green, LOW);
      digitalWrite(A2_red, LOW);
      digitalWrite(A2_yellow, HIGH);
      digitalWrite(A2_green, LOW);

      // Richtung 3 und 4 grün
      digitalWrite(A3_red, LOW);
      digitalWrite(A3_yellow, LOW);
      digitalWrite(A3_green, HIGH);
      digitalWrite(A4_red, LOW);
      digitalWrite(A4_yellow, LOW);
      digitalWrite(A4_green, HIGH);
      break;

    case 2:  // Phase 2: Richtung 3 und 4 Gelb -> Richtung 1 und 2 rot
      // Richtung 1 und 2 rot
      digitalWrite(A1_red, HIGH);
      digitalWrite(A1_yellow, LOW);
      digitalWrite(A1_green, LOW);
      digitalWrite(A2_red, HIGH);
      digitalWrite(A2_yellow, LOW);
      digitalWrite(A2_green, LOW);

      // Richtung 3 und 4 gelb
      digitalWrite(A3_red, LOW);
      digitalWrite(A3_yellow, HIGH);
      digitalWrite(A3_green, LOW);
      digitalWrite(A4_red, LOW);
      digitalWrite(A4_yellow, HIGH);
      digitalWrite(A4_green, LOW);
      break;

    case 3:  // Phase 3: Richtung 3 und 4 rot -> Richtung 1 und 2 grün
      // Richtung 1 und 2 grün
      digitalWrite(A1_red, LOW);
      digitalWrite(A1_yellow, LOW);
      digitalWrite(A1_green, HIGH);
      digitalWrite(A2_red, LOW);
      digitalWrite(A2_yellow, LOW);
      digitalWrite(A2_green, HIGH);

      // Richtung 3 und 4 rot
      digitalWrite(A3_red, HIGH);
      digitalWrite(A3_yellow, LOW);
      digitalWrite(A3_green, LOW);
      digitalWrite(A4_red, HIGH);
      digitalWrite(A4_yellow, LOW);
      digitalWrite(A4_green, LOW);
      break;
  }

  // Phase wechseln (alle 10 Sekunden)
  phase = (phase + 1) % 4;

  // Abstand für Lesbarkeit
  Serial.println("=========================");
  delay(10000); // Pause zwischen Phasenwechseln
}
