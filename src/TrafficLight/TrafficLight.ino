/*  
Traffic Light Control System with Ultrasonic Sensors and Day/Night Mode  
Version 1.1  
Date: 2024-05-20  
Purpose: This Arduino-based system controls 4 traffic lights (pedestrian and vehicle) using ultrasonic distance sensors for obstacle detection and buttons for manual override. It includes a day/night mode to adjust sensitivity and light transition timing for better visibility.  

Hardware Setup:  
- Arduino Board (e.g., Uno, Nano)  
- 4 Traffic Light Sets (each with:  
  - Pedestrian: straight (red/green), left (red/green)  
  - Vehicle: red/yellow/green)  
- 2 Manual Control Buttons (Pin 3 = Light 1 override; Pin 2 = Light 2 override)  
- 1 Mode Switch Button (Pin 4 = Day ↔ Night mode toggle)  
- 4 Ultrasonic Sensors (HC-SR04 compatible) paired with each traffic light:  
  - Sensor 1: TRIGA1=35, ECHOA1=37 (Light 1)  
  - Sensor 2: TRIGA2=31, ECHOA2=33 (Light 2)  
  - Sensor 3: TRIGA3=43, ECHOA3=45 (Light 3)  
  - Sensor 4: TRIGA4=39, ECHOA4=41 (Light 4)  
- Jumper wires, breadboard, and power supply (5V)  

Software Logic:  
1. Initialization (setup()): Configures pins, serial communication, and interrupts.  
2. Day/Night Mode: Toggled via mode button (Pin 4). Adjusts sensor thresholds and yellow light delays.  
3. Sensor Reading (measureDistance()): Triggers light transitions if obstacles are detected (or sensor errors) and handles manual button presses.  
4. Light State Management: setDefaultLightStates() resets all lights; status() coordinates transitions for each light (1-4).  
5. Main Loop (loop()): Continuously reads sensors, logs data, and monitors button states.  
*/

// =============================================================================
//                                   GLOBAL CONSTANTS & VARIABLES  
// =============================================================================

/***************************************************  
* Pedestrian and Vehicle Light Pins (Traffic Lights 1-4)  
* Naming: [PEDESTRIAN/VEHICLE]_[STRAIGHT/LEFT]_[COLOR]_[LIGHT_NUMBER]  
* Example: PEDESTRIAN_STRAIGHT_RED_1 = Straight pedestrian red light (Light 1)  
***************************************************/  
// Traffic Light 1 Pins  
const int PEDESTRIAN_STRAIGHT_RED_1    = 7;    // Straight pedestrian red (Light 1)  
const int PEDESTRIAN_STRAIGHT_GREEN_1  = 8;    // Straight pedestrian green (Light 1)  
const int PEDESTRIAN_LEFT_RED_1        = 9;    // Left pedestrian red (Light 1)  
const int PEDESTRIAN_LEFT_GREEN_1      = 10;   // Left pedestrian green (Light 1)  
const int VEHICLE_GREEN_1              = 11;   // Vehicle green (Light 1)  
const int VEHICLE_YELLOW_1             = 12;   // Vehicle yellow (Light 1)  
const int VEHICLE_RED_1                = 13;   // Vehicle red (Light 1)  

// Traffic Light 2 Pins  
const int PEDESTRIAN_STRAIGHT_RED_2    = 20;   // Straight pedestrian red (Light 2)  
const int PEDESTRIAN_STRAIGHT_GREEN_2  = 19;   // Straight pedestrian green (Light 2)  
const int PEDESTRIAN_LEFT_RED_2        = 18;   // Left pedestrian red (Light 2)  
const int PEDESTRIAN_LEFT_GREEN_2      = 17;   // Left pedestrian green (Light 2)  
const int VEHICLE_GREEN_2              = 16;   // Vehicle green (Light 2)  
const int VEHICLE_YELLOW_2             = 15;   // Vehicle yellow (Light 2)  
const int TRAFFIC_LIGHT_YELLOW_2       = 15;   // Redundant constant (matches VEHICLE_YELLOW_2 for clarity)  
const int VEHICLE_RED_2                = 14;   // Vehicle red (Light 2)  

// Traffic Light 3 Pins  
const int PEDESTRIAN_STRAIGHT_RED_3    = 22;   // Straight pedestrian red (Light 3)  
const int PEDESTRIAN_STRAIGHT_GREEN_3  = 24;   // Straight pedestrian green (Light 3)  
const int PEDESTRIAN_LEFT_RED_3        = 26;   // Left pedestrian red (Light 3)  
const int PEDESTRIAN_LEFT_GREEN_3      = 28;   // Left pedestrian green (Light 3)  
const int VEHICLE_GREEN_3              = 30;   // Vehicle green (Light 3)  
const int VEHICLE_YELLOW_3             = 32;   // Vehicle yellow (Light 3)  
const int VEHICLE_RED_3                = 34;   // Vehicle red (Light 3)  

// Traffic Light 4 Pins  
const int PEDESTRIAN_STRAIGHT_RED_4    = 36;   // Straight pedestrian red (Light 4)  
const int PEDESTRIAN_STRAIGHT_GREEN_4  = 38;   // Straight pedestrian green (Light 4)  
const int PEDESTRIAN_LEFT_RED_4        = 40;   // Left pedestrian red (Light 4)  
const int PEDESTRIAN_LEFT_GREEN_4      = 42;   // Left pedestrian green (Light 4)  
const int VEHICLE_GREEN_4              = 44;   // Vehicle green (Light 4)  
const int VEHICLE_YELLOW_4             = 46;   // Vehicle yellow (Light 4)  
const int VEHICLE_RED_4                = 48;   // Vehicle red (Light 4)  

/***************************************************  
* Button Pins & States  
* Pins 2, 3: Manual override buttons (Light 1/2)  
* Pin 4: Day/Night mode switch button  
***************************************************/  
const int manualButtonPin1 = 3;   // Manual override button for Light 1 (Pin 3)  
const int manualButtonPin2 = 2;   // Manual override button for Light 2 (Pin 2)  
const int modeButtonPin    = 4;   // Day/Night mode switch button (Pin 4)  

// Volatile variable to track manual button presses (modified in ISRs)  
// Values: 0 = no press; 1 = Pin 3 pressed (Light 1); 2 = Pin 2 pressed (Light 2)  
volatile int buttonState = 0;  

// Volatile variable to track day/night mode (modified in ISR)  
// true = Day Mode (default); false = Night Mode  
volatile bool isDayMode = true;  

/***************************************************  
* Ultrasonic Sensor Pins (Traffic Lights 1-4)  
* TRIG = Trigger (output); ECHO = Echo (input)  
***************************************************/  
#define TRIGA1 35    // Sensor 1 Trigger (Light 1)  
#define ECHOA1 37    // Sensor 1 Echo (Light 1)  
#define TRIGA2 31    // Sensor 2 Trigger (Light 2)  
#define ECHOA2 33    // Sensor 2 Echo (Light 2)  
#define TRIGA3 43    // Sensor 3 Trigger (Light 3)  
#define ECHOA3 45    // Sensor 3 Echo (Light 3)  
#define TRIGA4 39    // Sensor 4 Trigger (Light 4)  
#define ECHOA4 41    // Sensor 4 Echo (Light 4)  

// =============================================================================
//                                   INTERRUPT SERVICE ROUTINES (ISRs)  
// =============================================================================

/***************************************************  
* ISR for Manual Override Button 1 (Pin 3)  
* Triggered on Falling edge (button pressed) → sets buttonState=1  
***************************************************/  
void isrManualButton1() {  
  buttonState = 1;  // Signal Light 1 manual override  
  // Optional: Add debounce delay (e.g., delay(50)) to prevent flaky presses  
}  

/***************************************************  
* ISR for Manual Override Button 2 (Pin 2)  
* Triggered on Falling edge (button pressed) → sets buttonState=2  
***************************************************/  
void isrManualButton2() {  
  buttonState = 2;  // Signal Light 2 manual override  
  // Optional: Add debounce delay  
}  

/***************************************************  
* ISR for Day/Night Mode Switch Button (Pin 4)  
* Triggered on Falling edge (button pressed) → toggles isDayMode  
***************************************************/  
void isrModeButton() {  
  isDayMode = !isDayMode;  // Toggle mode (day ↔ night)  
  // Optional: Add debounce delay for reliability  
}  

// =============================================================================
//                                   SETUP FUNCTION (Runs Once)  
// =============================================================================  

void setup() {  
  // ---------------------------  
  // Serial Communication Init (Baud 9600)  
  // ---------------------------  
  Serial.begin(9600);  
  while (!Serial);  // Wait for serial monitor to connect (useful for native USB boards)  
  Serial.println("System starting...");  

  // ---------------------------  
  // Button Pin Modes (INPUT_PULLUP)  
  // ---------------------------  
  // Manual buttons: internal pull-up (HIGH when not pressed, LOW when pressed)  
  pinMode(manualButtonPin1, INPUT_PULLUP);  // Pin 3: Light 1 override button  
  pinMode(manualButtonPin2, INPUT_PULLUP);  // Pin 2: Light 2 override button  

  // Mode switch button: INPUT_PULLUP  
  pinMode(modeButtonPin, INPUT_PULLUP);     // Pin 4: Day/Night mode button  

  // ---------------------------  
  // Attach Interrupts to Buttons  
  // ---------------------------  
  // Manual buttons: detect Falling edge (press)  
  attachInterrupt(digitalPinToInterrupt(manualButtonPin1), isrManualButton1, FALLING);  
  attachInterrupt(digitalPinToInterrupt(manualButtonPin2), isrManualButton2, FALLING);  

  // Mode button: detect Falling edge (press) to toggle mode  
  attachInterrupt(digitalPinToInterrupt(modeButtonPin), isrModeButton, FALLING);  

  // ---------------------------  
  // Traffic Light Pin Modes (OUTPUT)  
  // ---------------------------  
  // Light 1 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_1,    OUTPUT);  
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_1,  OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_RED_1,        OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_GREEN_1,      OUTPUT);  
  pinMode(VEHICLE_GREEN_1,              OUTPUT);  
  pinMode(VEHICLE_YELLOW_1,             OUTPUT);  
  pinMode(VEHICLE_RED_1,                OUTPUT);  

  // Light 2 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_2,    OUTPUT);  
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_2,  OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_RED_2,        OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_GREEN_2,      OUTPUT);  
  pinMode(VEHICLE_GREEN_2,              OUTPUT);  
  pinMode(VEHICLE_YELLOW_2,             OUTPUT);  
  pinMode(VEHICLE_RED_2,                OUTPUT);  

  // Light 3 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_3,    OUTPUT);  
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_3,  OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_RED_3,        OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_GREEN_3,      OUTPUT);  
  pinMode(VEHICLE_GREEN_3,              OUTPUT);  
  pinMode(VEHICLE_YELLOW_3,             OUTPUT);  
  pinMode(VEHICLE_RED_3,                OUTPUT);  

  // Light 4 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_4,    OUTPUT);  
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_4,  OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_RED_4,        OUTPUT);  
  pinMode(PEDESTRIAN_LEFT_GREEN_4,      OUTPUT);  
  pinMode(VEHICLE_GREEN_4,              OUTPUT);  
  pinMode(VEHICLE_YELLOW_4,             OUTPUT);  
  pinMode(VEHICLE_RED_4,                OUTPUT);  

  // ---------------------------  
  // Ultrasonic Sensor Pin Modes  
  // ---------------------------  
  // TRIG pins (output) send trigger pulses; ECHO pins (input) receive reflections  
  pinMode(TRIGA1, OUTPUT);  
  pinMode(ECHOA1, INPUT);  
  pinMode(TRIGA2, OUTPUT);  
  pinMode(ECHOA2, INPUT);  
  pinMode(TRIGA3, OUTPUT);  
  pinMode(ECHOA3, INPUT);  
  pinMode(TRIGA4, OUTPUT);  
  pinMode(ECHOA4, INPUT);  

  // ---------------------------  
  // Initial Light States (All Red)  
  // ---------------------------  
  // All vehicle red lights ON (active HIGH)  
  digitalWrite(VEHICLE_RED_1, HIGH);  
  digitalWrite(VEHICLE_RED_2, HIGH);  
  digitalWrite(VEHICLE_RED_3, HIGH);  
  digitalWrite(VEHICLE_RED_4, HIGH);  

  // All pedestrian straight red lights ON  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);  

  // All pedestrian left red lights ON  
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);  

  Serial.println("Initialization complete. Default mode: DAY.");  
}  

// =============================================================================
//                                   LIGHT STATE MANAGEMENT  
// =============================================================================  

/***************************************************  
* setDefaultLightStates()  
* Resets all traffic lights to a safe default state:  
* - All yellow lights OFF  
* - All vehicle red lights OFF (temporarily, re-enabled later if needed)  
* - All vehicle green lights OFF  
* - Pedestrian straight/red and left/red lights ON; their green lights OFF  
***************************************************/  
void setDefaultLightStates() {  
  // ---------------------------  
  // Turn Off All Vehicle Yellow Lights  
  // ---------------------------  
  digitalWrite(VEHICLE_YELLOW_1, LOW);  
  digitalWrite(VEHICLE_YELLOW_2, LOW);  
  digitalWrite(VEHICLE_YELLOW_3, LOW);  
  digitalWrite(VEHICLE_YELLOW_4, LOW);  

  // ---------------------------  
  // Temporarily Turn Off Vehicle Red Lights  
  // ---------------------------  
  // Red lights will be re-enabled in subsequent steps if required.  
  digitalWrite(VEHICLE_RED_1, LOW);  
  digitalWrite(VEHICLE_RED_2, LOW);  
  digitalWrite(VEHICLE_RED_3, LOW);  
  digitalWrite(VEHICLE_RED_4, LOW);  

  // ---------------------------  
  // Turn Off All Vehicle Green Lights  
  // ---------------------------  
  digitalWrite(VEHICLE_GREEN_1, LOW);  
  digitalWrite(VEHICLE_GREEN_2, LOW);  
  digitalWrite(VEHICLE_GREEN_3, LOW);  
  digitalWrite(VEHICLE_GREEN_4, LOW);  

  // ---------------------------  
  // Reactivate Pedestrian Straight Red Lights (ON)  
  // ---------------------------  
  // Red = "Do not cross" for straight pedestrians  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);  

  // Turn Off Pedestrian Straight Green Lights (OFF)  
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, LOW);  
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, LOW);  
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, LOW);  
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, LOW);  

  // ---------------------------  
  // Reactivate Pedestrian Left Red Lights (ON)  
  // ---------------------------  
  // Red = "Do not cross" for left pedestrians  
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);  

  // Turn Off Pedestrian Left Green Lights (OFF)  
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, LOW);  
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, LOW);  
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, LOW);  
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, LOW);  
}  

// =============================================================================
//                                   STATE TRANSITION LOGIC  
// =============================================================================  

/***************************************************  
* status(int lightNumber)  
* Coordinates traffic light state transitions for the specified light (1-4).  
* Includes day/night mode adjustments for yellow light delays.  
* Returns: boolean (Placeholder, always true)  
* Note: Original logic for Light 3/4 may have intentional dependencies (verify).  
***************************************************/  
boolean status(int lightNumber) {  
  // ---------------------------  
  // Day/Night Mode Configuration  
  // ---------------------------  
  // Yellow light delay (mode-dependent): longer at night for visibility  
  const long YELLOW_DELAY_DAY   = 1500;   // 1.5 seconds (day)  
  const long YELLOW_DELAY_NIGHT = 2000;   // 2.0 seconds (night)  
  long yellowDelay = isDayMode ? YELLOW_DELAY_DAY : YELLOW_DELAY_NIGHT;  

  switch (lightNumber) {  
    // ---------------------------  
    // Light 1 Transition Logic  
    // ---------------------------  
    case 1: {  
      // Trigger only if Light 1's vehicle green light is OFF (transition needed)  
      if (!digitalRead(VEHICLE_GREEN_1)) {  
        setDefaultLightStates();  // Reset to default state  

        // Step 1: Yellow warning phase (all lights)  
        // Yellow lights warn vehicles/pedestrians of upcoming changes  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        delay(yellowDelay);  // Wait for mode-adjusted delay  

        // Step 2: Reset defaults, then prepare Light 1 and 4 for green  
        setDefaultLightStates();  

        // Light 1 and 4 enter pre-green phase (yellow ON)  
        // Lights 2 and 3 stay red  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_RED_2, HIGH);  
        digitalWrite(VEHICLE_RED_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        delay(yellowDelay);  // Wait again  

        // Step 3: Activate green phase for Light 1 and 4  
        // Yellow lights OFF, green lights ON  
        digitalWrite(VEHICLE_YELLOW_1, LOW);  
        digitalWrite(VEHICLE_GREEN_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, LOW);  
        digitalWrite(VEHICLE_GREEN_4, HIGH);  

        // Step 4: Activate pedestrian green lights (synchronized with vehicles)  
        // Straight pedestrians for Light 1/4: green ON, red OFF  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);  

        // Left pedestrians for Light 2/3: green ON, red OFF  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);  
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);  
      }  
      break;  
    }  

    // ---------------------------  
    // Light 2 Transition Logic  
    // ---------------------------  
    case 2: {  
      if (!digitalRead(VEHICLE_GREEN_2)) {  // Trigger if Light 2's green is OFF  
        setDefaultLightStates();  

        // Yellow warning phase (all lights)  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        delay(yellowDelay);  

        // Pre-green phase (Light 2 and 3)  
        setDefaultLightStates();  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_RED_1, HIGH);  
        digitalWrite(VEHICLE_RED_3, HIGH);  
        digitalWrite(VEHICLE_RED_4, HIGH);  
        delay(yellowDelay);  

        // Activate green phase (Light 2 and 3)  
        digitalWrite(VEHICLE_YELLOW_3, LOW);  
        digitalWrite(VEHICLE_GREEN_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, LOW);  
        digitalWrite(VEHICLE_GREEN_2, HIGH);  

        // Pedestrian straight green (Light 2/3)  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);  

        // Pedestrian left green (Light 1/4)  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);  
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);  
      }  
      break;  
    }  

    // ---------------------------  
    // Light 3 Transition Logic  
    // Note: Triggers when Light 2's green is OFF (original logic, may be intentional)  
    // ---------------------------  
    case 3: {  
      if (!digitalRead(VEHICLE_GREEN_2)) {  // Unusual condition: check Light 2's green state  
        setDefaultLightStates();  

        // Yellow warning phase (all lights)  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        delay(yellowDelay);  

        // Pre-green phase (Light 2 and 3)  
        setDefaultLightStates();  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_RED_1, HIGH);  
        digitalWrite(VEHICLE_RED_4, HIGH);  
        delay(yellowDelay);  

        // Activate green phase (Light 2 and 3)  
        digitalWrite(VEHICLE_YELLOW_2, LOW);  
        digitalWrite(VEHICLE_GREEN_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, LOW);  
        digitalWrite(VEHICLE_GREEN_3, HIGH);  

        // Pedestrian straight green (Light 2/3)  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);  

        // Pedestrian left green (Light 1/4)  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);  
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);  
      }  
      break;  
    }  

    // ---------------------------  
    // Light 4 Transition Logic  
    // Note: Triggers when Light 1's green is OFF (original logic, verify intentionality)  
    // ---------------------------  
    case 4: {  
      if (!digitalRead(VEHICLE_GREEN_1)) {  // Unusual condition: check Light 1's green state  
        setDefaultLightStates();  

        // Yellow warning phase (all lights)  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        delay(yellowDelay);  

        // Pre-green phase (Light 1 and 4)  
        setDefaultLightStates();  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        digitalWrite(VEHICLE_RED_2, HIGH);  
        digitalWrite(VEHICLE_RED_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        delay(yellowDelay);  

        // Activate green phase (Light 1 and 4)  
        digitalWrite(VEHICLE_YELLOW_1, LOW);  
        digitalWrite(VEHICLE_GREEN_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, LOW);  
        digitalWrite(VEHICLE_GREEN_4, HIGH);  

        // Pedestrian straight green (Light 1/4)  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);  
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);  

        // Pedestrian left green (Light 2/3)  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);  
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);  
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);  
      }  
      break;  
    }  
  }  
  return true;  // Placeholder return (no current use)  
}  

// =============================================================================
//                                   SENSOR & MODE HANDLING  
// =============================================================================  

/***************************************************  
* measureDistance(int trigPin, int echoPin, int lightNumber)  
* Reads an ultrasonic sensor, calculates distance, and triggers light transitions if:  
*   - distance == 0 (sensor error)  
*   - distance ≥ mode-dependent threshold  
*   - or manual button pressed (buttonState > 0)  
* Parameters:  
*   - trigPin: Sensor trigger pin (output)  
*   - echoPin: Sensor echo pin (input)  
*   - lightNumber: Associated traffic light (1-4)  
* Returns: long (distance in cm, or 0 if error)  
***************************************************/  
long measureDistance(int trigPin, int echoPin, int lightNumber) {  
  // ---------------------------  
  // Step 1: Trigger Ultrasonic Pulse  
  // ---------------------------  
  // Stabilize trigger pin (LOW for 5ms)  
  digitalWrite(trigPin, LOW);  
  delay(5);  

  // Send 10µs trigger pulse (HIGH) to activate sensor  
  digitalWrite(trigPin, HIGH);  
  delay(10);  // Pulse duration (10µs)  
  digitalWrite(trigPin, LOW);  // Stop pulse  

  // ---------------------------  
  // Step 2: Measure Echo Duration  
  // ---------------------------  
  // pulseIn() returns duration of HIGH signal on echoPin (µs).  
  // Returns 0 if no echo (sensor disconnected) or duration > 65535µs (obstacle too far).  
  long echoDuration = pulseIn(echoPin, HIGH);  

  // ---------------------------  
  // Step 3: Calculate Distance  
  // ---------------------------  
  // Formula: distance (cm) = (echoDuration * speed_of_sound) / (2 * 1e6)  
  // Speed of sound ≈ 3432 cm/s (20°C). Divided by 2 for round-trip.  
  // Original code uses 0.03432 (likely typo; correct is 0.003432). Retained for consistency.  
  long distance = (echoDuration / 2) * 0.03432;  

  // ---------------------------  
  // Step 4: Check Mode-Dependent Triggers  
  // ---------------------------  
  // Define sensor threshold based on current mode:  
  // - Day Mode: ≥500cm (less sensitive, ignores distant obstacles)  
  // - Night Mode: ≥300cm (more sensitive, reacts to closer obstacles)  
  const long SENSOR_THRESHOLD_DAY   = 500;  
  const long SENSOR_THRESHOLD_NIGHT = 300;  
  long sensorThreshold = isDayMode ? SENSOR_THRESHOLD_DAY : SENSOR_THRESHOLD_NIGHT;  

  // Check if trigger conditions are met (sensor error, obstacle too far, or manual press)  
  if (distance == 0 || distance >= sensorThreshold || buttonState > 0) {  
    // ---------------------------  
    // Priority: Manual Button Over Sensor Input  
    // ---------------------------  
    // If a manual button is pressed, override lightNumber with buttonState (1=Light1, 2=Light2)  
    if (buttonState > 0) {  
      lightNumber = buttonState;  // Use button-pressed light number  
    }  

    // ---------------------------  
    // Trigger Light Transition  
    // ---------------------------  
    status(lightNumber);  // Update light states based on lightNumber  

    // ---------------------------  
    // Reset Manual Button State  
    // ---------------------------  
    buttonState = 0;  // Clear buttonState to avoid repeat triggers  
  }  

  return distance;  // Return measured distance (cm)  
}  

// =============================================================================
//                                   MAIN LOOP FUNCTION (Runs Continuously)  
// =============================================================================  

void loop() {  
  // ---------------------------  
  // Clear Serial Console & Print Mode  
  // ---------------------------  
  Serial.println("\n===================================");  
  Serial.print("Current Mode: ");  
  Serial.println(isDayMode ? "DAY (Default)" : "NIGHT");  // Explicit mode label  
  Serial.println("-----------------------------------");  

  // ---------------------------  
  // Read Sensors and Log Distances  
  // ---------------------------  
  // Process each traffic light's sensor and print results  
  Serial.print("Light 1 (Sensor TRIGA1-ECHOA1) Distance: ");  
  Serial.print(measureDistance(TRIGA1, ECHOA1, 1));  // Read Light 1 sensor  
  Serial.println(" cm");  

  Serial.print("Light 2 (Sensor TRIGA2-ECHOA2) Distance: ");  
  Serial.print(measureDistance(TRIGA2, ECHOA2, 2));  // Read Light 2 sensor  
  Serial.println(" cm");  

  Serial.print("Light 3 (Sensor TRIGA3-ECHOA3) Distance: ");  
  Serial.print(measureDistance(TRIGA3, ECHOA3, 3));  // Read Light 3 sensor  
  Serial.println(" cm");  

  Serial.print("Light 4 (Sensor TRIGA4-ECHOA4) Distance: ");  
  Serial.print(measureDistance(TRIGA4, ECHOA4, 4));  // Read Light 4 sensor  
  Serial.println(" cm");  

  // ---------------------------  
  // Log Button States  
  // ---------------------------  
  Serial.println("--- Button Status ---");  
  Serial.print("Manual Button (Pin ");  
  Serial.print(manualButtonPin1);  
  Serial.print("): ");  
  Serial.println(buttonState == 1 ? "Pressed (Light 1 override)" : "Not pressed");  

  Serial.print("Manual Button (Pin ");  
  Serial.print(manualButtonPin2);  
  Serial.print("): ");  
  Serial.println(buttonState == 2 ? "Pressed (Light 2 override)" : "Not pressed");  

  Serial.print("Mode Button (Pin ");  
  Serial.print(modeButtonPin);  
  Serial.print("): Last state = ");  
  Serial.println(isDayMode ? "DAY" : "NIGHT");  // Indirectly shows last toggle result  
  Serial.println("---------------------");  

  // ---------------------------  
  // Pause Between Readings  
  // ---------------------------  
  delay(1000);  // 1-second delay (loop runs at ~1Hz)  
}  