const int schalterPin = A0; // Pin, an dem die parallel geschalteten Schalter angeschlossen sind

void setup() {
  Serial.begin(9600); // Serielle Kommunikation starten (für Debugging)
  
  // Schalter-Pin als Eingang mit internem Pull-Up-Widerstand definieren
  pinMode(schalterPin, INPUT_PULLUP);
}

void loop() {
  // Status des Schalters auslesen
  int schalterStatus = digitalRead(schalterPin);

  if (schalterStatus == LOW) {
    // Schalter geschlossen
    Serial.println("Schalter gedrückt!");
  } else {
    // Schalter geöffnet
    Serial.println("Schalter offen!");
  }

  delay(200); // Kurze Pause, um zu verhindern, dass der serielle Monitor zu schnell überfüllt wird
}
