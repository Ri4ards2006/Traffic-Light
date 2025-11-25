#define TRIG1 14
#define ECHO1 15
#define TRIG2 16
#define ECHO2 17
#define TRIG3 18
#define ECHO3 19
#define TRIG4 1
#define ECHO4 0

void setup() {
  Serial.begin(9600);

  // Pins initialisieren
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);
  pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT);
  pinMode(ECHO4, INPUT);
}

void loop() {
  // Sensoren prüfen
  checkSensor(TRIG1, ECHO1, 1);
  checkSensor(TRIG2, ECHO2, 2);
  checkSensor(TRIG3, ECHO3, 3);
  checkSensor(TRIG4, ECHO4, 4);

  delay(2000); // Kurze Pause zwischen den Messungen
}

void checkSensor(int trigPin, int echoPin, int sensorNumber) {
  // Ultraschall-Impuls auslösen
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Echo-Dauer messen
  long duration = pulseIn(echoPin, HIGH);
  float distance = (duration * 0.034) / 2; // Umwandlung in cm

  // Status prüfen
  bool isActive = (distance <= 10.0 && distance > 0.0); // Aktiv, wenn <= 10 cm
  Serial.print("Sensor ");
  Serial.print(sensorNumber);
  Serial.print(": ");
  
  if (isActive) {
    Serial.println("🟢 An");
  } else {
    Serial.println("🔴 Nicht an");
  }
}
