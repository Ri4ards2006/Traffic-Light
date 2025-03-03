#define TRIG1 14
#define ECHO1 15
#define TRIG2 16
#define ECHO2 17
#define TRIG3 18
#define ECHO3 19
#define TRIG4 0
#define ECHO4 1

void setup() {
  Serial.begin(9600);

  // Pin-Modus für alle Sensoren konfigurieren
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
  // Sensoren nacheinander prüfen
  checkSensor(TRIG1, ECHO1, 1);
  delay(50); // Kurze Pause, um Signalüberlagerung zu vermeiden

  checkSensor(TRIG2, ECHO2, 2);
  delay(50);

  checkSensor(TRIG3, ECHO3, 3);
  delay(50);

  checkSensor(TRIG4, ECHO4, 4);
  delay(100); // Etwas längere Pause nach allen Sensoren
}

void checkSensor(int trigPin, int echoPin, int sensorNumber) {
  // Trigger-Signal senden
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15); // Trigger-Puls verlängert
  digitalWrite(trigPin, LOW);

  // Echo-Signal messen
  long duration = pulseIn(echoPin, HIGH, 20000); // Timeout 20ms
  if (duration == 0) {
    Serial.print("Sensor ");
    Serial.print(sensorNumber);
    Serial.println(": Kein Echo-Signal empfangen!");
  } else {
    float distance = (duration * 0.034) / 2; // Umrechnen in cm
    Serial.print("Sensor ");
    Serial.print(sensorNumber);
    Serial.print(": Objekt in ");
    Serial.print(distance);
    Serial.println(" cm Entfernung erkannt!");
  }
}
