// Konstanten für die Pin-Nummern der Ampeln
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

// Fußgängerampeln: Pins gruppiert (je 2 Ampeln = 1 Einheit)
int pedestrianGroups[4][4] = {
  {53, 52, 51, 50},  // Fußgängerampeln 1
  {49, 48, 47, 46},  // Fußgängerampeln 2
  {45, 44, 43, 42},  // Fußgängerampeln 3
  {41, 40, 39, 38}   // Fußgängerampeln 4
};

// Taster-Pin für die Anfragen der einzelnen Fußgängerampeln
const int buttonPin1 = 28;  // Taster für Fußgängerampel 1
const int buttonPin2 = 25;  // Taster für Fußgängerampel 2
const int buttonPin3 = 27;  // Taster für Fußgängerampel 3
const int buttonPin4 = 30;  // Taster für Fußgängerampel 4

bool requestReceived[4] = {false, false, false, false};  // Anfrage für jede Fußgängerampel

// Phasenzeiten
const int redDuration = 5000;
const int yellowDuration = 2000;
const int greenDuration = 5000;

void setup() {
  Serial.begin(9600); // Für Konsolenausgabe

  // Hauptampeln setzen
  pinMode(A1_red, OUTPUT); pinMode(A1_yellow, OUTPUT); pinMode(A1_green, OUTPUT);
  pinMode(A2_red, OUTPUT); pinMode(A2_yellow, OUTPUT); pinMode(A2_green, OUTPUT);
  pinMode(A3_red, OUTPUT); pinMode(A3_yellow, OUTPUT); pinMode(A3_green, OUTPUT);
  pinMode(A4_red, OUTPUT); pinMode(A4_yellow, OUTPUT); pinMode(A4_green, OUTPUT);

  // Fußgängerampeln setzen
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      pinMode(pedestrianGroups[i][j], OUTPUT);
    }
  }

  // Taster für die Anfragen als Input definieren
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  pinMode(buttonPin3, INPUT_PULLUP);
  pinMode(buttonPin4, INPUT_PULLUP);

  // Initialzustand: Alle Ampeln auf Rot
  setTrafficLight(A1_red, A1_yellow, A1_green, HIGH, LOW, LOW);
  setTrafficLight(A2_red, A2_yellow, A2_green, HIGH, LOW, LOW);
  setTrafficLight(A3_red, A3_yellow, A3_green, HIGH, LOW, LOW);
  setTrafficLight(A4_red, A4_yellow, A4_green, HIGH, LOW, LOW);
  setPedestrianGroup(0, HIGH, LOW); // Alle Fußgängerampeln auf Rot
  setPedestrianGroup(1, HIGH, LOW);
  setPedestrianGroup(2, HIGH, LOW);
  setPedestrianGroup(3, HIGH, LOW);
  printState();
}

void loop() {
  // Überprüfen, ob einer der Taster gedrückt wird und entsprechende Anfrage auslösen
  if (digitalRead(buttonPin1) == LOW) {
    requestReceived[0] = true;
  }
  if (digitalRead(buttonPin2) == LOW) {
    requestReceived[1] = true;
  }
  if (digitalRead(buttonPin3) == LOW) {
    requestReceived[2] = true;
  }
  if (digitalRead(buttonPin4) == LOW) {
    requestReceived[3] = true;
  }

  // Wenn eine Anfrage empfangen wurde, prüfen, ob es sicher ist
  for (int i = 0; i < 4; i++) {
    if (requestReceived[i]) {
      handlePedestrianRequest(i);  // Anfrage für die Fußgängerampel i
      requestReceived[i] = false;  // Anfrage zurücksetzen
    }
  }

  // Normale Ampelphasen (Fahrzeugampeln schalten weiter)
  phaseTrafficLights();

  printState();  // Ampelstatus ausgeben
}

void phaseTrafficLights() {
  // Phase 1: Hauptampeln 1 & 2 Grün
  setTrafficLight(A1_red, A1_yellow, A1_green, LOW, LOW, HIGH);
  setTrafficLight(A2_red, A2_yellow, A2_green, LOW, LOW, HIGH);
  setTrafficLight(A3_red, A3_yellow, A3_green, HIGH, LOW, LOW);
  setTrafficLight(A4_red, A4_yellow, A4_green, HIGH, LOW, LOW);
  delay(greenDuration);

  // Gelb-Phase für Hauptampeln 1 & 2
  setTrafficLight(A1_red, A1_yellow, A1_green, LOW, HIGH, LOW);
  setTrafficLight(A2_red, A2_yellow, A2_green, LOW, HIGH, LOW);
  delay(yellowDuration);

  // Phase 2: Hauptampeln 3 & 4 Grün
  setTrafficLight(A1_red, A1_yellow, A1_green, HIGH, LOW, LOW);
  setTrafficLight(A2_red, A2_yellow, A2_green, HIGH, LOW, LOW);
  setTrafficLight(A3_red, A3_yellow, A3_green, LOW, LOW, HIGH);
  setTrafficLight(A4_red, A4_yellow, A4_green, LOW, LOW, HIGH);
  delay(greenDuration);

  // Gelb-Phase für Hauptampeln 3 & 4
  setTrafficLight(A3_red, A3_yellow, A3_green, LOW, HIGH, LOW);
  setTrafficLight(A4_red, A4_yellow, A4_green, LOW, HIGH, LOW);
  delay(yellowDuration);
}

void handlePedestrianRequest(int group) {
  // Überprüfen, ob es sicher ist, Fußgängern grünes Licht zu geben (z.B. Fahrzeugampeln rot)
  bool isTrafficRed = (digitalRead(A1_green) == LOW && digitalRead(A2_green) == LOW && digitalRead(A3_green) == LOW && digitalRead(A4_green) == LOW);

  if (isTrafficRed) {
    // Fußgängerampeln auf Grün schalten, wenn Verkehr auf Rot
    setPedestrianGroup(group, LOW, HIGH);  // Die angeforderte Fußgängerampel auf Grün
    delay(greenDuration);  // Warten, bis Fußgänger sicher gehen können
    setPedestrianGroup(group, HIGH, LOW); // Die angeforderte Fußgängerampel auf Rot
  }
}

void setTrafficLight(int redPin, int yellowPin, int greenPin, int redState, int yellowState, int greenState) {
  digitalWrite(redPin, redState);
  digitalWrite(yellowPin, yellowState);
  digitalWrite(greenPin, greenState);
}

void setPedestrianGroup(int group, int redState, int greenState) {
  for (int i = 0; i < 4; i++) {
    if (i % 2 == 0) {
      digitalWrite(pedestrianGroups[group][i], greenState); // Grün an
    } else {
      digitalWrite(pedestrianGroups[group][i], redState);   // Rot an
    }
  }
}

void printState() {
  Serial.println("====== Ampelstatus ======");
  Serial.println("Autoampeln:");
  Serial.print("A1: "); printLight(A1_red, A1_yellow, A1_green);
  Serial.print("A2: "); printLight(A2_red, A2_yellow, A2_green);
  Serial.print("A3: "); printLight(A3_red, A3_yellow, A3_green);
  Serial.print("A4: "); printLight(A4_red, A4_yellow, A4_green);

  Serial.println("\nFußgängerampeln:");
  for (int i = 0; i < 4; i++) {
    Serial.print("P"); Serial.print(i + 1); Serial.print(": ");
    printLight(pedestrianGroups[i][0], pedestrianGroups[i][1], pedestrianGroups[i][2]);  // Gruppen-Pins der Fußgängerampel anzeigen
    Serial.println();
  }
  Serial.println("=========================\n");
}

void printLight(int redPin, int yellowPin, int greenPin) {
  if (digitalRead(redPin) == HIGH) Serial.print("R ");
  if (digitalRead(yellowPin) == HIGH) Serial.print("Y ");
  if (digitalRead(greenPin) == HIGH) Serial.print("G ");
  Serial.println();
}
