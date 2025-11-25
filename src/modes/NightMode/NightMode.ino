/***************************************************
 * Global Pin constants for pedestrian and vehicle lights
 ***************************************************/
const int PEDESTRIAN_STRAIGHT_RED_1   = 7;   
const int PEDESTRIAN_STRAIGHT_GREEN_1 = 8;   
const int PEDESTRIAN_LEFT_RED_1    = 9;   
const int PEDESTRIAN_LEFT_GREEN_1  = 10;  
const int VEHICLE_GREEN_1        = 11;  
const int VEHICLE_YELLOW_1         = 12;  
const int VEHICLE_RED_1          = 13;  
const int PEDESTRIAN_STRAIGHT_RED_2   = 20;  
const int PEDESTRIAN_STRAIGHT_GREEN_2 = 19;  
const int PEDESTRIAN_LEFT_RED_2    = 18;  
const int PEDESTRIAN_LEFT_GREEN_2  = 17;  
const int VEHICLE_GREEN_2        = 16;  
const int VEHICLE_YELLOW_2         = 15;  
const int VEHICLE_RED_2          = 14;  
const int PEDESTRIAN_STRAIGHT_RED_3   = 22;  
const int PEDESTRIAN_STRAIGHT_GREEN_3 = 24;  
const int PEDESTRIAN_LEFT_RED_3    = 26;  
const int PEDESTRIAN_LEFT_GREEN_3  = 28;  
const int VEHICLE_GREEN_3        = 30;  
const int VEHICLE_YELLOW_3         = 32;  
const int VEHICLE_RED_3          = 34;  
const int PEDESTRIAN_STRAIGHT_RED_4   = 36;  
const int PEDESTRIAN_STRAIGHT_GREEN_4 = 38;  
const int PEDESTRIAN_LEFT_RED_4    = 40;  
const int PEDESTRIAN_LEFT_GREEN_4  = 42;  
const int VEHICLE_GREEN_4        = 44;  
const int VEHICLE_YELLOW_4         = 46;  
const int VEHICLE_RED_4          = 48;  
/***************************************************
 * Global Pin constants for buttons
 * Pin 3 (first button)
 * Pin 2 (second button)
 ***************************************************/
const int buttonPin1 = 3; 
const int buttonPin2 = 2; 
/***************************************************
 * Global volatile variable for button state
 * (modified in ISR)
 ***************************************************/
volatile int buttonState = 0; 
// 0 = no button pressed
// 1 = button at Pin 3 pressed
// 2 = button at Pin 2 pressed
/***************************************************
 * Pin definitions for ultrasonic sensors
 * Here: 4 sensors, corresponding to traffic lights 1-4
 ***************************************************/
#define TRIGA1 35
#define ECHOA1 37
#define TRIGA2 31
#define ECHOA2 33
#define TRIGA3 43
#define ECHOA3 45
#define TRIGA4 39
#define ECHOA4 41
/***************************************************
 * Interrupt Service Routines (ISR)
 ***************************************************/
void isrButton1() {
  // Button at Pin 3 (FALLING edge) => set global variable to 1
  buttonState = 1;
}
void isrButton2() {
  // Button at Pin 2 (FALLING edge) => set global variable to 2
  buttonState = 2;
}
/***************************************************
 * SETUP FUNCTION
 ***************************************************/
void setup() {
  // Serial interface for debugging
  Serial.begin(9600);
  // Buttons with internal pull-up (pressed = LOW)
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode(buttonPin2, INPUT_PULLUP);
  // Enable interrupts (FALLING edge) for both buttons
  attachInterrupt(digitalPinToInterrupt(buttonPin1), isrButton1, FALLING);
  attachInterrupt(digitalPinToInterrupt(buttonPin2), isrButton2, FALLING);
  // PinModes for Traffic Light 1
  pinMode(PEDESTRIAN_STRAIGHT_RED_1,   OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_1, OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_1,    OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_1,  OUTPUT);
  pinMode(VEHICLE_GREEN_1,        OUTPUT);
  pinMode(VEHICLE_YELLOW_1,         OUTPUT);
  pinMode(VEHICLE_RED_1,          OUTPUT);
  // PinModes for Traffic Light 2
  pinMode(PEDESTRIAN_STRAIGHT_RED_2,   OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_2, OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_2,    OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_2,  OUTPUT);
  pinMode(VEHICLE_GREEN_2,        OUTPUT);
  pinMode(VEHICLE_YELLOW_2,         OUTPUT);
  pinMode(VEHICLE_RED_2,          OUTPUT);
  // PinModes for Traffic Light 3
  pinMode(PEDESTRIAN_STRAIGHT_RED_3,   OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_3, OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_3,    OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_3,  OUTPUT);
  pinMode(VEHICLE_GREEN_3,        OUTPUT);
  pinMode(VEHICLE_YELLOW_3,         OUTPUT);
  pinMode(VEHICLE_RED_3,          OUTPUT);
  // PinModes for Traffic Light 4
  pinMode(PEDESTRIAN_STRAIGHT_RED_4,   OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_4, OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_4,    OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_4,  OUTPUT);
  pinMode(VEHICLE_GREEN_4,        OUTPUT);
  pinMode(VEHICLE_YELLOW_4,         OUTPUT);
  pinMode(VEHICLE_RED_4,          OUTPUT);
  // PinModes for ultrasonic sensors
  pinMode(TRIGA1, OUTPUT);
  pinMode(ECHOA1, INPUT);
  pinMode(TRIGA2, OUTPUT);
  pinMode(ECHOA2, INPUT);
  pinMode(TRIGA3, OUTPUT);
  pinMode(ECHOA3, INPUT);
  pinMode(TRIGA4, OUTPUT);
  pinMode(ECHOA4, INPUT);
  // Define initial traffic light states (e.g., red on)
  digitalWrite(VEHICLE_RED_1, HIGH);
  digitalWrite(VEHICLE_RED_2, HIGH);
  digitalWrite(VEHICLE_RED_3, HIGH);
  digitalWrite(VEHICLE_RED_4, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);
}
/***************************************************
 * Set all lights to defined default state
 ***************************************************/
void setDefaultLightStates() {
  // Turn yellow off
  digitalWrite(VEHICLE_YELLOW_1, LOW);
  digitalWrite(VEHICLE_YELLOW_2, LOW);
  digitalWrite(VEHICLE_YELLOW_3, LOW);
  digitalWrite(VEHICLE_YELLOW_4, LOW);
  // Turn vehicle red off
  digitalWrite(VEHICLE_RED_1, LOW);
  digitalWrite(VEHICLE_RED_2, LOW);
  digitalWrite(VEHICLE_RED_3, LOW);
  digitalWrite(VEHICLE_RED_4, LOW);
  // Turn vehicle green off
  digitalWrite(VEHICLE_GREEN_1, LOW);
  digitalWrite(VEHICLE_GREEN_2, LOW);
  digitalWrite(VEHICLE_GREEN_3, LOW);
  digitalWrite(VEHICLE_GREEN_4, LOW);
  // Pedestrian straight to red
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);
  // Turn pedestrian straight green off
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, LOW);
  // Pedestrian left to red
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);
  // Turn pedestrian left green off
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, LOW);
}
/***************************************************
 * status(number):
 * Switches traffic light states based on the number (1-4), based on your original code.
 ***************************************************/
boolean status(int number) {
  switch (number) {
    case 1:
      if (!digitalRead(VEHICLE_GREEN_1)) {
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_RED_2, HIGH);
        digitalWrite(VEHICLE_RED_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);
        digitalWrite(VEHICLE_YELLOW_1, LOW);
        digitalWrite(VEHICLE_GREEN_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, LOW);
        digitalWrite(VEHICLE_GREEN_4, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);
      }
      break;
    case 2:
      if (!digitalRead(VEHICLE_GREEN_2)) {
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_RED_1, HIGH);
        digitalWrite(VEHICLE_RED_3, HIGH);
        digitalWrite(VEHICLE_RED_4, HIGH);
        delay(1500);
        digitalWrite(VEHICLE_YELLOW_3, LOW);
        digitalWrite(VEHICLE_GREEN_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, LOW);
        digitalWrite(VEHICLE_GREEN_2, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);
      }
      break;
    case 3:
      if (!digitalRead(VEHICLE_GREEN_2)) {  // Note: Original checks VEHICLE_GREEN_2 (may be intentional)
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_RED_1, HIGH);
        digitalWrite(VEHICLE_RED_4, HIGH);
        delay(1500);
        digitalWrite(VEHICLE_YELLOW_2, LOW);
        digitalWrite(VEHICLE_GREEN_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, LOW);
        digitalWrite(VEHICLE_GREEN_3, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);
      }
      break;
    case 4:
      if (!digitalRead(VEHICLE_GREEN_1)) {
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        digitalWrite(VEHICLE_RED_2, HIGH);
        digitalWrite(VEHICLE_RED_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        delay(1500);
        digitalWrite(VEHICLE_YELLOW_1, LOW);
        digitalWrite(VEHICLE_GREEN_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, LOW);
        digitalWrite(VEHICLE_GREEN_4, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);
      }
      break;
  }
  return true; // Placeholder
}
/***************************************************
 * Measures distance using ultrasonic sensor
 * and calls 'status(number)' if:
 *   - distance == 0 or distance >= 500
 *   - or button pressed (buttonState > 0)
 ***************************************************/
long measureDistance(int trigPin, int echoPin, int number) {
  // Trigger ultrasonic signal
  digitalWrite(trigPin, LOW);
  delay(5);
  digitalWrite(trigPin, HIGH);
  delay(10);
  digitalWrite(trigPin, LOW);
  // Measure echo time
  long duration = pulseIn(echoPin, HIGH);
  // Calculate distance (in cm)
  long distance = (duration / 2) * 0.03432;
  // Check if distance is 0, >= 500, or button pressed
  if (distance == 0 || distance >= 500 || buttonState > 0) {
    // If a button is pressed, prioritize button state
    if (buttonState > 0) {
      number = buttonState;
    }
    status(number);
    buttonState = 0;
  }
  return distance;
}
/***************************************************
 * LOOP FUNCTION
 ***************************************************/
void loop() {
  // For each traffic light, determine sensor value and switch status if needed
  Serial.println("");
  Serial.print("Traffic Light 1: ");
  Serial.print(measureDistance(TRIGA1, ECHOA1, 1));
  Serial.print("  ");
  Serial.print("Traffic Light 2: ");
  Serial.print(measureDistance(TRIGA2, ECHOA2, 2));
  Serial.print("  ");
  Serial.print("Traffic Light 3: ");
  Serial.print(measureDistance(TRIGA3, ECHOA3, 3));
  Serial.print("  ");
  Serial.print("Traffic Light 4: ");
  Serial.print(measureDistance(TRIGA4, ECHOA4, 4));
  Serial.print("  ");
  // Output button state (global via ISR)
  Serial.print("Button State: ");
  Serial.println(buttonState);
  // Short pause
  delay(1000);
}