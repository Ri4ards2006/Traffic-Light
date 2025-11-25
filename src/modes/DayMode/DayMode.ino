/***************************************************
 * Pin Constants for Pedestrian and Traffic Lights
 * Author: Richard Zuikov a
 * Date: 25.11.2025
 * Note: Original German code translated to English.
 ***************************************************/
const int PEDESTRIAN_STRAIGHT_RED_1    = 7;   
const int PEDESTRIAN_STRAIGHT_GREEN_1  = 8;   
const int PEDESTRIAN_LEFT_RED_1        = 9;   
const int PEDESTRIAN_LEFT_GREEN_1      = 10;  
const int TRAFFIC_LIGHT_GREEN_1        = 11;  
const int TRAFFIC_LIGHT_YELLOW_1       = 12;  
const int TRAFFIC_LIGHT_RED_1          = 13;  
const int PEDESTRIAN_STRAIGHT_RED_2    = 20;  
const int PEDESTRIAN_STRAIGHT_GREEN_2  = 19;  
const int PEDESTRIAN_LEFT_RED_2        = 18;  
const int PEDESTRIAN_LEFT_GREEN_2      = 17;  
const int TRAFFIC_LIGHT_GREEN_2        = 16;  
const int TRAFFIC_LIGHT_YELLOW_2       = 15;  
const int TRAFFIC_LIGHT_RED_2          = 14;  
const int PEDESTRIAN_STRAIGHT_RED_3    = 22;  
const int PEDESTRIAN_STRAIGHT_GREEN_3  = 24;  
const int PEDESTRIAN_LEFT_RED_3        = 26;  
const int PEDESTRIAN_LEFT_GREEN_3      = 28;  
const int TRAFFIC_LIGHT_GREEN_3        = 30;  
const int TRAFFIC_LIGHT_YELLOW_3       = 32;  
const int TRAFFIC_LIGHT_RED_3          = 34;  
const int PEDESTRIAN_STRAIGHT_RED_4    = 36;  
const int PEDESTRIAN_STRAIGHT_GREEN_4  = 38;  
const int PEDESTRIAN_LEFT_RED_4        = 40;  
const int PEDESTRIAN_LEFT_GREEN_4      = 42;  
const int TRAFFIC_LIGHT_GREEN_4        = 44;  
const int TRAFFIC_LIGHT_YELLOW_4       = 46;  
const int TRAFFIC_LIGHT_RED_4          = 48;  

/***************************************************
 * Sets all traffic lights to their initial state
 ***************************************************/
void setInitialStates() {
  // Turn off yellow lights
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, LOW);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, LOW);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, LOW);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, LOW);
  
  // Turn off green lights (original comment: "Rot Autos aus" was incorrect)
  digitalWrite(TRAFFIC_LIGHT_GREEN_1, LOW);
  digitalWrite(TRAFFIC_LIGHT_GREEN_2, LOW);
  digitalWrite(TRAFFIC_LIGHT_GREEN_3, LOW);
  digitalWrite(TRAFFIC_LIGHT_GREEN_4, LOW);
  
  // Turn off red lights (original comment: "Grün Autos aus" was incorrect)
  digitalWrite(TRAFFIC_LIGHT_RED_1, LOW);
  digitalWrite(TRAFFIC_LIGHT_RED_2, LOW);
  digitalWrite(TRAFFIC_LIGHT_RED_3, LOW);
  digitalWrite(TRAFFIC_LIGHT_RED_4, LOW);
  
  // Set pedestrian straight lights to red
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);
  
  // Turn off pedestrian straight green lights
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, LOW);
  
  // Set pedestrian left lights to red
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);
  
  // Turn off pedestrian left green lights
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, LOW);
}

/***************************************************
 * Sequence 1 and 4
 ***************************************************/
void Sequence14() {
  setInitialStates();
  
  // Activate yellow lights for all traffic signals
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, HIGH);
  delay(2000);
  
  setInitialStates();
  
  // Yellow 1 and 4 active, Red 2 and 3 active
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_3, HIGH);
  delay(2000);
  
  setInitialStates();
  
  // Red 2 and 3 active, Green 1 and 4 active
  // Pedestrian straight green active (1 and 4), Pedestrian left green active (2 and 3)
  // Turn off pedestrian straight red (1 and 4), Turn off pedestrian left red (2 and 3)
  digitalWrite(TRAFFIC_LIGHT_RED_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_3, HIGH);
  digitalWrite(TRAFFIC_LIGHT_GREEN_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_GREEN_4, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);
  digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);
  digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);
  
  // Final phase: 3.5 seconds
  delay(3500);
}

/***************************************************
 * Sequence 2 and 3
 ***************************************************/
void Sequence23() {
  setInitialStates();
  
  // Activate yellow lights for all traffic signals
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, HIGH);
  delay(2000);
  
  setInitialStates();
  
  // Yellow 3 and 2 active, Red 1 and 4 active
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, HIGH);
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_4, HIGH);
  delay(2000);
  
  setInitialStates();
  
  // Red 1 and 4 active, Green 2 and 3 active
  // Pedestrian straight green active (3 and 2), Pedestrian left green active (1 and 4)
  // Turn off pedestrian straight red (3 and 2), Turn off pedestrian left red (1 and 4)
  digitalWrite(TRAFFIC_LIGHT_RED_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_4, HIGH);
  digitalWrite(TRAFFIC_LIGHT_GREEN_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_GREEN_3, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);
  digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);
  digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);
  
  // Final phase: 3.5 seconds
  delay(3500);
}

/***************************************************
 * SETUP
 * Defines all pins as outputs and initializes traffic light states
 ***************************************************/
void setup() {
  Serial.begin(9600);
  
  // Traffic light 1 pins
  pinMode(PEDESTRIAN_STRAIGHT_RED_1,    OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_1,  OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_1,        OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_1,      OUTPUT);
  pinMode(TRAFFIC_LIGHT_GREEN_1,        OUTPUT);
  pinMode(TRAFFIC_LIGHT_YELLOW_1,       OUTPUT);
  pinMode(TRAFFIC_LIGHT_RED_1,          OUTPUT);
  
  // Traffic light 2 pins
  pinMode(PEDESTRIAN_STRAIGHT_RED_2,    OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_2,  OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_2,        OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_2,      OUTPUT);
  pinMode(TRAFFIC_LIGHT_GREEN_2,        OUTPUT);
  pinMode(TRAFFIC_LIGHT_YELLOW_2,       OUTPUT);
  pinMode(TRAFFIC_LIGHT_RED_2,          OUTPUT);
  
  // Traffic light 3 pins
  pinMode(PEDESTRIAN_STRAIGHT_RED_3,    OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_3,  OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_3,        OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_3,      OUTPUT);
  pinMode(TRAFFIC_LIGHT_GREEN_3,        OUTPUT);
  pinMode(TRAFFIC_LIGHT_YELLOW_3,       OUTPUT);
  pinMode(TRAFFIC_LIGHT_RED_3,          OUTPUT);
  
  // Traffic light 4 pins
  pinMode(PEDESTRIAN_STRAIGHT_RED_4,    OUTPUT);
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_4,  OUTPUT);
  pinMode(PEDESTRIAN_LEFT_RED_4,        OUTPUT);
  pinMode(PEDESTRIAN_LEFT_GREEN_4,      OUTPUT);
  pinMode(TRAFFIC_LIGHT_GREEN_4,        OUTPUT);
  pinMode(TRAFFIC_LIGHT_YELLOW_4,       OUTPUT);
  pinMode(TRAFFIC_LIGHT_RED_4,          OUTPUT);
  
  // Initial state: all traffic and pedestrian red lights active
  digitalWrite(TRAFFIC_LIGHT_RED_1, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_2, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_3, HIGH);
  digitalWrite(TRAFFIC_LIGHT_RED_4, HIGH);
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
 * LOOP
 * Executes sequences 1-4 and 2-3 repeatedly
 ***************************************************/
void loop() {
  Sequence14();  // Run sequence 1 and 4
  Sequence23();  // Run sequence 2 and 3
}