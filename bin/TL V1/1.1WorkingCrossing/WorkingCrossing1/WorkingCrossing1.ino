// Konstanten für die Pin-Nummern der Taster
const int buttonPins[4] = {28, 25, 27, 30}; // Taster an den Pins

// Gruppen von Pins für die Fußgängerampeln (jeweils 4 Pins pro Gruppe)
// Hier wurde die Reihenfolge der Pins korrigiert (gerade für rot und ungerade für grün)
const int pedestrianGroups[4][4] = {
  {6 ,  7},  // Gruppe 1: gerade Pins (52, 50) für rot, ungerade Pins (53, 51) für grün
  {8 ,  9},  // Gruppe 2: gerade Pins (48, 46) für rot, ungerade Pins (49, 47) für grün
  {10, 11},  // Gruppe 3: gerade Pins (44, 42) für rot, ungerade Pins (45, 43) für grün
  {12, 13}   // Gruppe 4: gerade Pins (40, 38) für rot, ungerade Pins (41, 39) für grün
};

// Konstanten für die Pin-Nummern der Hauptampeln (Rot, Gelb, Grün)
const int A1_red = 22;
const int A1_yellow = 23;
const int A1_green = 24;
const int A2_red = 25;
const int A2_yellow = 26;
const int A2_green = 27;
const int A3_red = 28;
const int A3_yellow = 29;
const int A3_green = 30;
const int A4_red = 31;
const int A4_yellow = 32;
const int A4_green = 33;

// Variablen für die Tasterzustände
int buttonStates[4] = {0, 0, 0, 0};

// Variable für die Ampelphasen
int phase = 1; // Startphase

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
  // Tasterzustände auslesen
  for (int i = 0; i < 4; i++) {
    buttonStates[i] = digitalRead(buttonPins[i]);
  }

  // Logik für die Ampelphasen
  switch (phase) {
    case 1:
      // Phase 1: A1 (Nord) und A2 (Süd) grün, A3 (Ost) und A4 (West) rot
      digitalWrite(A1_green, HIGH);
      digitalWrite(A1_yellow, LOW);
      digitalWrite(A1_red, LOW);

      digitalWrite(A2_green, HIGH);
      digitalWrite(A2_yellow, LOW);
      digitalWrite(A2_red, LOW);

      digitalWrite(A3_red, HIGH);
      digitalWrite(A3_yellow, LOW);
      digitalWrite(A3_green, LOW);

      digitalWrite(A4_red, HIGH);
      digitalWrite(A4_yellow, LOW);
      digitalWrite(A4_green, LOW);

      // Fußgängerampeln aktivieren, wenn Taster gedrückt
      for (int i = 0; i < 4; i++) {
        if (buttonStates[i] == LOW) {
          digitalWrite(pedestrianGroups[i][1], LOW);  // Ungerade: grün (gehend)
          digitalWrite(pedestrianGroups[i][3], LOW);  // Ungerade: grün (gehend)
          digitalWrite(pedestrianGroups[i][0], HIGH); // Gerade: rot (nicht gehend)
          digitalWrite(pedestrianGroups[i][2], HIGH); // Gerade: rot (nicht gehend)
        } else {
          digitalWrite(pedestrianGroups[i][1], HIGH);  // Ungerade: rot
          digitalWrite(pedestrianGroups[i][3], HIGH);  // Ungerade: rot
          digitalWrite(pedestrianGroups[i][0], LOW);   // Gerade: rot
          digitalWrite(pedestrianGroups[i][2], LOW);   // Gerade: rot
        }
      }

      phase = 2; // Wechsel zu Phase 2 (Gelb für A1 und A2)
      break;

    case 2:
      // Phase 2: A1 und A2 gelb, A3 und A4 rot
      digitalWrite(A1_green, LOW);
      digitalWrite(A1_yellow, HIGH);
      digitalWrite(A1_red, LOW);

      digitalWrite(A2_green, LOW);
      digitalWrite(A2_yellow, HIGH);
      digitalWrite(A2_red, LOW);

      digitalWrite(A3_red, HIGH);
      digitalWrite(A3_yellow, HIGH);
      digitalWrite(A3_green, LOW);

      digitalWrite(A4_red, HIGH);
      digitalWrite(A4_yellow, HIGH);
      digitalWrite(A4_green, LOW);

      phase = 3; // Wechsel zu Phase 3 (A3 und A4 grün)
      break;

    case 3:
      // Phase 3: A1 und A2 rot, A3 (Ost) und A4 (West) grün
      digitalWrite(A1_green, LOW);
      digitalWrite(A1_yellow, LOW);
      digitalWrite(A1_red, HIGH);

      digitalWrite(A2_green, LOW);
      digitalWrite(A2_yellow, LOW);
      digitalWrite(A2_red, HIGH);

      digitalWrite(A3_green, HIGH);
      digitalWrite(A3_yellow, LOW);
      digitalWrite(A3_red, LOW);

      digitalWrite(A4_green, HIGH);
      digitalWrite(A4_yellow, LOW);
      digitalWrite(A4_red, LOW);

      // Fußgängerampeln aktivieren, wenn Taster gedrückt
      for (int i = 0; i < 4; i++) {
        if (buttonStates[i] == LOW) {
          digitalWrite(pedestrianGroups[i][1], LOW);  // Ungerade: grün (gehend)
          digitalWrite(pedestrianGroups[i][3], LOW);  // Ungerade: grün (gehend)
          digitalWrite(pedestrianGroups[i][0], HIGH); // Gerade: rot (nicht gehend)
          digitalWrite(pedestrianGroups[i][2], HIGH); // Gerade: rot (nicht gehend)
        } else {
          digitalWrite(pedestrianGroups[i][1], HIGH);  // Ungerade: rot
          digitalWrite(pedestrianGroups[i][3], HIGH);  // Ungerade: rot
          digitalWrite(pedestrianGroups[i][0], LOW);   // Gerade: rot
          digitalWrite(pedestrianGroups[i][2], LOW);   // Gerade: rot
        }
      }

      phase = 4; // Wechsel zu Phase 4 (Gelb für A3 und A4)
      break;

    case 4:
      // Phase 4: A3 und A4 gelb, A1 und A2 rot
      digitalWrite(A1_green, LOW);
      digitalWrite(A1_yellow, HIGH);
      digitalWrite(A1_red, HIGH);

      digitalWrite(A2_green, LOW);
      digitalWrite(A2_yellow, HIGH);
      digitalWrite(A2_red, HIGH);

      digitalWrite(A3_green, LOW);
      digitalWrite(A3_yellow, HIGH);
      digitalWrite(A3_red, LOW);

      digitalWrite(A4_green, LOW);
      digitalWrite(A4_yellow, HIGH);
      digitalWrite(A4_red, LOW);

      phase = 1; // Zurück zu Phase 1
      break;
  }

  delay(1000); // Pause für einen Übergang (1 Sekunde)
}
