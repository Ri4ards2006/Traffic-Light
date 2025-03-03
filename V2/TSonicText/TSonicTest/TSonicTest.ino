const int trigPin = 14;
const int echoPin = 15;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // Trigger-Puls senden
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Echo-Zeit messen
  long duration = pulseIn(echoPin, HIGH);
  
  // Entfernung in Zentimeter berechnen
  float distance = duration * 0.034 / 2;

  // Entfernung ausgeben
  Serial.println(distance);

  delay(500); // Kurze Pause, damit die Ausgabe lesbar bleibt
}
// 2cm 