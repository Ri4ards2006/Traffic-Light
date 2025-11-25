// =============================================================================
//                                   GLOBAL VARIABLES & Constants
// =============================================================================

// ---------------------------
// Pedestrian and Vehicle Light Pins (Traffic Lights 1-4)
// ---------------------------
// Naming Convention: [PEDESTRIAN/VEHICLE]_[STRAIGHT/LEFT]_[RED/GREEN/YELLOW]_[LIGHT NUMBER]
// Example: PEDESTRIAN_STRAIGHT_RED_1 = Red light for straight pedestrian direction at Traffic Light 1
const int PEDESTRIAN_STRAIGHT_RED_1   = 7;    // Pin 7: Straight pedestrian red (Light 1)
const int PEDESTRIAN_STRAIGHT_GREEN_1 = 8;   // Pin 8: Straight pedestrian green (Light 1)
const int PEDESTRIAN_LEFT_RED_1      = 9;    // Pin 9: Left pedestrian red (Light 1)
const int PEDESTRIAN_LEFT_GREEN_1    = 10;   // Pin 10: Left pedestrian green (Light 1)
const int VEHICLE_GREEN_1            = 11;   // Pin 11: Vehicle green (Light 1)
const int VEHICLE_YELLOW_1           = 12;   // Pin 12: Vehicle yellow (Light 1)
const int VEHICLE_RED_1              = 13;   // Pin 13: Vehicle red (Light 1)

// Traffic Light 2 Pins
const int PEDESTRIAN_STRAIGHT_RED_2   = 20;   // Pin 20: Straight pedestrian red (Light 2)
const int PEDESTRIAN_STRAIGHT_GREEN_2 = 19;   // Pin 19: Straight pedestrian green (Light 2)
const int PEDESTRIAN_LEFT_RED_2       = 18;   // Pin 18: Left pedestrian red (Light 2)
const int PEDESTRIAN_LEFT_GREEN_2     = 17;   // Pin 17: Left pedestrian green (Light 2)
const int VEHICLE_GREEN_2             = 16;   // Pin 16: Vehicle green (Light 2)
const int VEHICLE_YELLOW_2            = 15;   // Pin 15: Vehicle yellow (Light 2)
const int VEHICLE_RED_2               = 14;   // Pin 14: Vehicle red (Light 2)

// Traffic Light 3 Pins
const int PEDESTRIAN_STRAIGHT_RED_3   = 22;   // Pin 22: Straight pedestrian red (Light 3)
const int PEDESTRIAN_STRAIGHT_GREEN_3 = 24;   // Pin 24: Straight pedestrian green (Light 3)
const int PEDESTRIAN_LEFT_RED_3       = 26;   // Pin 26: Left pedestrian red (Light 3)
const int PEDESTRIAN_LEFT_GREEN_3     = 28;   // Pin 28: Left pedestrian green (Light 3)
const int VEHICLE_GREEN_3             = 30;   // Pin 30: Vehicle green (Light 3)
const int VEHICLE_YELLOW_3            = 32;   // Pin 32: Vehicle yellow (Light 3)
const int VEHICLE_RED_3               = 34;   // Pin 34: Vehicle red (Light 3)

// Traffic Light 4 Pins
const int PEDESTRIAN_STRAIGHT_RED_4   = 36;   // Pin 36: Straight pedestrian red (Light 4)
const int PEDESTRIAN_STRAIGHT_GREEN_4 = 38;   // Pin 38: Straight pedestrian green (Light 4)
const int PEDESTRIAN_LEFT_RED_4       = 40;   // Pin 40: Left pedestrian red (Light 4)
const int PEDESTRIAN_LEFT_GREEN_4     = 42;   // Pin 42: Left pedestrian green (Light 4)
const int VEHICLE_GREEN_4             = 44;   // Pin 44: Vehicle green (Light 4)
const int VEHICLE_YELLOW_4            = 46;   // Pin 46: Vehicle yellow (Light 4)
const int VEHICLE_RED_4               = 48;   // Pin 48: Vehicle red (Light 4)

// ---------------------------
// Button Pins
// ---------------------------
const int buttonPin1 = 3;  // First button connected to Pin 3
const int buttonPin2 = 2;  // Second button connected to Pin 2

// ---------------------------
// Button State (Volatile)
// ---------------------------
// Volatile keyword ensures the compiler does not cache this variable; critical for ISR (Interrupt) updates.
// Used to track button presses across interrupts and the main loop.
volatile int buttonState = 0; 
// State Values:
//   0 = No button pressed
//   1 = buttonPin1 (Pin 3) pressed
//   2 = buttonPin2 (Pin 2) pressed

// ---------------------------
// Ultrasonic Sensor Pins (Traffic Lights 1-4)
// ---------------------------
// Naming Convention: TRIGA[X] = Trigger pin for Sensor X; ECHOA[X] = Echo pin for Sensor X
// Each sensor corresponds to a traffic light (1-4) to detect obstacles nearby.
#define TRIGA1 35    // Trigger pin for Sensor 1 (Light 1)
#define ECHOA1 37    // Echo pin for Sensor 1 (Light 1)
#define TRIGA2 31    // Trigger pin for Sensor 2 (Light 2)
#define ECHOA2 33    // Echo pin for Sensor 2 (Light 2)
#define TRIGA3 43    // Trigger pin for Sensor 3 (Light 3)
#define ECHOA3 45    // Echo pin for Sensor 3 (Light 3)
#define TRIGA4 39    // Trigger pin for Sensor 4 (Light 4)
#define ECHOA4 41    // Echo pin for Sensor 4 (Light 4)

// =============================================================================
//                                   INTERRUPT SERVICE ROUTINES (ISRs)
// =============================================================================

// ---------------------------
// ISR for buttonPin1 (Pin 3)
// ---------------------------
// This function runs whenever buttonPin1 (Pin 3) is pressed (FALLING edge interrupt).
// Updates buttonState to 1 to indicate a press.
void isrButton1() {
  // When the button is pressed, the pin voltage drops from HIGH to LOW (FALLING edge).
  // Update the global buttonState variable to signal a press of buttonPin1.
  buttonState = 1;
}

// ---------------------------
// ISR for buttonPin2 (Pin 2)
// ---------------------------
// This function runs whenever buttonPin2 (Pin 2) is pressed (FALLING edge interrupt).
// Updates buttonState to 2 to indicate a press.
void isrButton2() {
  // When the button is pressed, the pin voltage drops from HIGH to LOW (FALLING edge).
  // Update the global buttonState variable to signal a press of buttonPin2.
  buttonState = 2;
}

// =============================================================================
//                                   SETUP FUNCTION (Runs Once at Startup)
// =============================================================================

void setup() {
  // ---------------------------
  // Serial Port Initialization (Debugging)
  // ---------------------------
  // Enable serial communication at 9600 baud rate to log sensor data and button states.
  Serial.begin(9600);

  // ---------------------------
  // Button Pin Configuration
  // ---------------------------
  // Set button pins to INPUT_PULLUP mode:
  // - Internal pull-up resistor activates, so pins are HIGH when not pressed.
  // - When pressed, the pin is connected to GND, reading LOW.
  pinMode(buttonPin1, INPUT_PULLUP);  // Pin 3: buttonPin1
  pinMode(buttonPin2, INPUT_PULLUP);  // Pin 2: buttonPin2

  // ---------------------------
  // Interrupt Configuration for Buttons
  // ---------------------------
  // Attach interrupts to detect button presses:
  // - digitalPinToInterrupt() converts pin number to interrupt number (board-specific).
  // - FALLING edge triggers the ISR when the button is pressed (pin voltage drops from HIGH to LOW).
  attachInterrupt(digitalPinToInterrupt(buttonPin1), isrButton1, FALLING);  // Interrupt for buttonPin1
  attachInterrupt(digitalPinToInterrupt(buttonPin2), isrButton2, FALLING);  // Interrupt for buttonPin2

  // ---------------------------
  // Traffic Light Pin Configuration (All OUTPUT)
  // ---------------------------
  // Configure all traffic light pins as OUTPUT to control their state (HIGH = on, LOW = off).
  // Traffic Light 1 Pins:
  pinMode(PEDESTRIAN_STRAIGHT_RED_1,   OUTPUT);   // Pedestrian straight red (Light 1)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_1, OUTPUT);   // Pedestrian straight green (Light 1)
  pinMode(PEDESTRIAN_LEFT_RED_1,       OUTPUT);   // Pedestrian left red (Light 1)
  pinMode(PEDESTRIAN_LEFT_GREEN_1,     OUTPUT);   // Pedestrian left green (Light 1)
  pinMode(VEHICLE_GREEN_1,             OUTPUT);   // Vehicle green (Light 1)
  pinMode(VEHICLE_YELLOW_1,            OUTPUT);   // Vehicle yellow (Light 1)
  pinMode(VEHICLE_RED_1,               OUTPUT);   // Vehicle red (Light 1)

  // Traffic Light 2 Pins:
  pinMode(PEDESTRIAN_STRAIGHT_RED_2,   OUTPUT);   // Pedestrian straight red (Light 2)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_2, OUTPUT);   // Pedestrian straight green (Light 2)
  pinMode(PEDESTRIAN_LEFT_RED_2,       OUTPUT);   // Pedestrian left red (Light 2)
  pinMode(PEDESTRIAN_LEFT_GREEN_2,     OUTPUT);   // Pedestrian left green (Light 2)
  pinMode(VEHICLE_GREEN_2,             OUTPUT);   // Vehicle green (Light 2)
  pinMode(VEHICLE_YELLOW_2,            OUTPUT);   // Vehicle yellow (Light 2)
  pinMode(VEHICLE_RED_2,               OUTPUT);   // Vehicle red (Light 2)

  // Traffic Light 3 Pins:
  pinMode(PEDESTRIAN_STRAIGHT_RED_3,   OUTPUT);   // Pedestrian straight red (Light 3)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_3, OUTPUT);   // Pedestrian straight green (Light 3)
  pinMode(PEDESTRIAN_LEFT_RED_3,       OUTPUT);   // Pedestrian left red (Light 3)
  pinMode(PEDESTRIAN_LEFT_GREEN_3,     OUTPUT);   // Pedestrian left green (Light 3)
  pinMode(VEHICLE_GREEN_3,             OUTPUT);   // Vehicle green (Light 3)
  pinMode(VEHICLE_YELLOW_3,            OUTPUT);   // Vehicle yellow (Light 3)
  pinMode(VEHICLE_RED_3,               OUTPUT);   // Vehicle red (Light 3)

  // Traffic Light 4 Pins:
  pinMode(PEDESTRIAN_STRAIGHT_RED_4,   OUTPUT);   // Pedestrian straight red (Light 4)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_4, OUTPUT);   // Pedestrian straight green (Light 4)
  pinMode(PEDESTRIAN_LEFT_RED_4,       OUTPUT);   // Pedestrian left red (Light 4)
  pinMode(PEDESTRIAN_LEFT_GREEN_4,     OUTPUT);   // Pedestrian left green (Light 4)
  pinMode(VEHICLE_GREEN_4,             OUTPUT);   // Vehicle green (Light 4)
  pinMode(VEHICLE_YELLOW_4,            OUTPUT);   // Vehicle yellow (Light 4)
  pinMode(VEHICLE_RED_4,               OUTPUT);   // Vehicle red (Light 4)

  // ---------------------------
  // Ultrasonic Sensor Pin Configuration
  // ---------------------------
  // TRIG pins (output) trigger the ultrasonic pulse.
  // ECHO pins (input) receive the reflected pulse.
  pinMode(TRIGA1, OUTPUT);  // Sensor 1 Trigger
  pinMode(ECHOA1, INPUT);   // Sensor 1 Echo
  pinMode(TRIGA2, OUTPUT);  // Sensor 2 Trigger
  pinMode(ECHOA2, INPUT);   // Sensor 2 Echo
  pinMode(TRIGA3, OUTPUT);  // Sensor 3 Trigger
  pinMode(ECHOA3, INPUT);   // Sensor 3 Echo
  pinMode(TRIGA4, OUTPUT);  // Sensor 4 Trigger
  pinMode(ECHOA4, INPUT);   // Sensor 4 Echo

  // ---------------------------
  // Initial Light States (All Red)
  // ---------------------------
  // At startup, ensure all vehicle and pedestrian lights are in a safe "red" state.
  // Vehicle red lights (active HIGH):
  digitalWrite(VEHICLE_RED_1, HIGH);   // Light 1: Vehicle red ON
  digitalWrite(VEHICLE_RED_2, HIGH);   // Light 2: Vehicle red ON
  digitalWrite(VEHICLE_RED_3, HIGH);   // Light 3: Vehicle red ON
  digitalWrite(VEHICLE_RED_4, HIGH);   // Light 4: Vehicle red ON

  // Pedestrian straight red lights (active HIGH):
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);   // Light 1: Pedestrian straight red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);   // Light 2: Pedestrian straight red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);   // Light 3: Pedestrian straight red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);   // Light 4: Pedestrian straight red ON

  // Pedestrian left red lights (active HIGH):
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);   // Light 1: Pedestrian left red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);   // Light 2: Pedestrian left red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);   // Light 3: Pedestrian left red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);   // Light 4: Pedestrian left red ON
}

// =============================================================================
//                                   setDefaultLightStates()
// =============================================================================

// ---------------------------
// Function: setDefaultLightStates()
// Purpose: Reset all traffic lights to a default "safe" state.
// ---------------------------
// This function turns off all non-red lights (yellow, green) and ensures red lights are active.
// Used to reset the system before transitioning to new states.
void setDefaultLightStates() {
  // ---------------------------
  // Turn Off All Vehicle Yellow Lights
  // ---------------------------
  // Yellow lights are only active during state transitions (1.5s delays). Reset to off.
  digitalWrite(VEHICLE_YELLOW_1, LOW);  // Light 1: Vehicle yellow OFF
  digitalWrite(VEHICLE_YELLOW_2, LOW);  // Light 2: Vehicle yellow OFF
  digitalWrite(VEHICLE_YELLOW_3, LOW);  // Light 3: Vehicle yellow OFF
  digitalWrite(VEHICLE_YELLOW_4, LOW);  // Light 4: Vehicle yellow OFF

  // ---------------------------
  // Turn Off All Vehicle Red Lights (Temporary)
  // ---------------------------
  // Red lights are kept on via other logic. This step ensures they’re off briefly during reset.
  digitalWrite(VEHICLE_RED_1, LOW);   // Light 1: Vehicle red OFF (temporary)
  digitalWrite(VEHICLE_RED_2, LOW);   // Light 2: Vehicle red OFF (temporary)
  digitalWrite(VEHICLE_RED_3, LOW);   // Light 3: Vehicle red OFF (temporary)
  digitalWrite(VEHICLE_RED_4, LOW);   // Light 4: Vehicle red OFF (temporary)

  // ---------------------------
  // Turn Off All Vehicle Green Lights
  // ---------------------------
  // Green lights are only active during "go" phases. Reset to off.
  digitalWrite(VEHICLE_GREEN_1, LOW);   // Light 1: Vehicle green OFF
  digitalWrite(VEHICLE_GREEN_2, LOW);   // Light 2: Vehicle green OFF
  digitalWrite(VEHICLE_GREEN_3, LOW);   // Light 3: Vehicle green OFF
  digitalWrite(VEHICLE_GREEN_4, LOW);   // Light 4: Vehicle green OFF

  // ---------------------------
  // Set Pedestrian Straight Lights to Red (ON)
  // ---------------------------
  // Red light indicates "do not cross" for straight pedestrians.
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);   // Light 1: Pedestrian straight red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);   // Light 2: Pedestrian straight red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);   // Light 3: Pedestrian straight red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);   // Light 4: Pedestrian straight red ON

  // ---------------------------
  // Turn Off Pedestrian Straight Green Lights
  // ---------------------------
  // Green light indicates "cross now". Reset to off.
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, LOW);   // Light 1: Pedestrian straight green OFF
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, LOW);   // Light 2: Pedestrian straight green OFF
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, LOW);   // Light 3: Pedestrian straight green OFF
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, LOW);   // Light 4: Pedestrian straight green OFF

  // ---------------------------
  // Set Pedestrian Left Lights to Red (ON)
  // ---------------------------
  // Red light indicates "do not cross" for left pedestrians.
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);   // Light 1: Pedestrian left red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);   // Light 2: Pedestrian left red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);   // Light 3: Pedestrian left red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);   // Light 4: Pedestrian left red ON

  // ---------------------------
  // Turn Off Pedestrian Left Green Lights
  // ---------------------------
  // Green light indicates "cross now". Reset to off.
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, LOW);   // Light 1: Pedestrian left green OFF
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, LOW);   // Light 2: Pedestrian left green OFF
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, LOW);   // Light 3: Pedestrian left green OFF
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, LOW);   // Light 4: Pedestrian left green OFF
}

// =============================================================================
//                                   status(int number)
// =============================================================================

// ---------------------------
// Function: status(int number)
// Purpose: Coordinate traffic light state transitions for a specific light (1-4).
// Parameters:
//   - number: Traffic light identifier (1 = Light 1, ..., 4 = Light 4)
// Returns: boolean (Always returns true; placeholder for potential future logic)
// Note: This function contains complex state transitions. Logic may need validation for real-world use.
// ---------------------------
// This function is triggered when a sensor detects an obstacle (or a button is pressed).
// It follows a sequence to transition lights from red → yellow → green, with pedestrian lights synchronized.
boolean status(int number) {
  switch (number) {
    // ---------------------------
    // Case 1: Traffic Light 1 Triggered
    // ---------------------------
    case 1: {
      // Check if vehicle green light for Light 1 is OFF (indicates it’s time to transition)
      if (!digitalRead(VEHICLE_GREEN_1)) {  // digitalRead() returns HIGH (1) if light is ON, LOW (0) if OFF
        // Step 1: Reset all lights to default (red on, green/yellow off)
        setDefaultLightStates();

        // Step 2: Activate yellow lights for all traffic lights (1.5s delay)
        // Yellow lights warn vehicles/pedestrians of impending state change.
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  // Light 1: Yellow ON
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  // Light 2: Yellow ON
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  // Light 3: Yellow ON
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  // Light 4: Yellow ON
        delay(1500);  // Pause for 1.5 seconds to let yellow light flash

        // Step 3: Reset defaults again, then set new pre-green states
        setDefaultLightStates();

        // Step 4: Activate yellow for Light 1 and Light 4; set red for Lights 2 and 3
        // This may prepare for a coordinated green phase (Lights 1 and 4)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  // Light 1: Yellow ON (pre-green)
        digitalWrite(VEHICLE_RED_2, HIGH);      // Light 2: Red ON (stay red)
        digitalWrite(VEHICLE_RED_3, HIGH);      // Light 3: Red ON (stay red)
        digitalWrite(VEHICLE_YELLOW_4, HIGH);   // Light 4: Yellow ON (pre-green)
        delay(1500);  // Pause for 1.5 seconds

        // Step 5: Deactivate yellow, activate green for Lights 1 and 4
        // Also activate pedestrian lights for synchronized crossing.
        digitalWrite(VEHICLE_YELLOW_1, LOW);    // Light 1: Yellow OFF
        digitalWrite(VEHICLE_GREEN_1, HIGH);   // Light 1: Green ON (vehicles can go)
        digitalWrite(VEHICLE_YELLOW_4, LOW);   // Light 4: Yellow OFF
        digitalWrite(VEHICLE_GREEN_4, HIGH);   // Light 4: Green ON (vehicles can go)

        // Pedestrian Lights (Straight):
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);  // Light 1: Straight pedestrians can cross
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);  // Light 4: Straight pedestrians can cross
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);     // Light 1: Straight pedestrian red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);     // Light 4: Straight pedestrian red OFF

        // Pedestrian Lights (Left):
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);   // Light 2: Left pedestrians can cross?
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);   // Light 3: Left pedestrians can cross?
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);      // Light 2: Left pedestrian red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);      // Light 3: Left pedestrian red OFF
      }
      break;
    }

    // ---------------------------
    // Case 2: Traffic Light 2 Triggered
    // ---------------------------
    case 2: {
      // Check if vehicle green light for Light 2 is OFF
      if (!digitalRead(VEHICLE_GREEN_2)) {
        // Step 1: Reset all lights to default
        setDefaultLightStates();

        // Step 2: Yellow lights for all (1.5s delay)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);

        // Step 3: Reset defaults, then set yellow for Light 2 and prepare others
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_2, HIGH);    // Light 2: Yellow pre-green
        digitalWrite(VEHICLE_RED_1, HIGH);       // Light 1: Red (stay red)
        digitalWrite(VEHICLE_RED_3, HIGH);       // Light 3: Red (stay red)
        digitalWrite(VEHICLE_RED_4, HIGH);       // Light 4: Red (stay red)
        delay(1500);

        // Step 4: Activate green for Light 2 and Light 3
        digitalWrite(VEHICLE_YELLOW_3, LOW);    // Light 3: Yellow OFF
        digitalWrite(VEHICLE_GREEN_3, HIGH);    // Light 3: Green ON
        digitalWrite(VEHICLE_YELLOW_2, LOW);    // Light 2: Yellow OFF
        digitalWrite(VEHICLE_GREEN_2, HIGH);    // Light 2: Green ON

        // Pedestrian Lights (Straight):
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);  // Light 3: Straight pedestrians cross
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);  // Light 2: Straight pedestrians cross
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);     // Light 2: Straight red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);     // Light 3: Straight red OFF

        // Pedestrian Lights (Left):
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);   // Light 1: Left pedestrians cross?
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);   // Light 4: Left pedestrians cross?
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);      // Light 1: Left red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);      // Light 4: Left red OFF
      }
      break;
    }

    // ---------------------------
    // Case 3: Traffic Light 3 Triggered (Note: Checks Light 2’s green state!)
    // ---------------------------
    case 3: {
      // Note: This condition checks VEHICLE_GREEN_2 (Light 2) instead of VEHICLE_GREEN_3 (Light 3).
      // Verify if this is intentional (e.g., coordinated with Light 2).
      if (!digitalRead(VEHICLE_GREEN_2)) {
        // Step 1: Reset all lights to default
        setDefaultLightStates();

        // Step 2: Yellow lights for all (1.5s delay)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);

        // Step 3: Reset defaults, then set yellow for Light 2 and Light 3
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_3, HIGH);   // Light 3: Yellow pre-green
        digitalWrite(VEHICLE_YELLOW_2, HIGH);   // Light 2: Yellow pre-green
        digitalWrite(VEHICLE_RED_1, HIGH);      // Light 1: Red (stay red)
        digitalWrite(VEHICLE_RED_4, HIGH);      // Light 4: Red (stay red)
        delay(1500);

        // Step 4: Activate green for Light 2 and Light 3
        digitalWrite(VEHICLE_YELLOW_2, LOW);   // Light 2: Yellow OFF
        digitalWrite(VEHICLE_GREEN_2, HIGH);   // Light 2: Green ON
        digitalWrite(VEHICLE_YELLOW_3, LOW);   // Light 3: Yellow OFF
        digitalWrite(VEHICLE_GREEN_3, HIGH);   // Light 3: Green ON

        // Pedestrian Lights (Straight):
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);  // Light 3: Straight pedestrians cross
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);  // Light 2: Straight pedestrians cross
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);     // Light 2: Straight red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);     // Light 3: Straight red OFF

        // Pedestrian Lights (Left):
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);   // Light 1: Left pedestrians cross?
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);   // Light 4: Left pedestrians cross?
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);      // Light 1: Left red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);      // Light 4: Left red OFF
      }
      break;
    }

    // ---------------------------
    // Case 4: Traffic Light 4 Triggered
    // ---------------------------
    case 4: {
      // Check if vehicle green light for Light 4 is OFF
      if (!digitalRead(VEHICLE_GREEN_1)) {  // Note: Checks Light 1’s green state, not Light 4’s. Verify intentionality.
        // Step 1: Reset all lights to default
        setDefaultLightStates();

        // Step 2: Yellow lights for all (1.5s delay)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);
        digitalWrite(VEHICLE_YELLOW_2, HIGH);
        digitalWrite(VEHICLE_YELLOW_3, HIGH);
        digitalWrite(VEHICLE_YELLOW_4, HIGH);
        delay(1500);

        // Step 3: Reset defaults, then set yellow for Light 4 and prepare others
        setDefaultLightStates();
        digitalWrite(VEHICLE_YELLOW_4, HIGH);   // Light 4: Yellow pre-green
        digitalWrite(VEHICLE_RED_2, HIGH);      // Light 2: Red (stay red)
        digitalWrite(VEHICLE_RED_3, HIGH);      // Light 3: Red (stay red)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);   // Light 1: Yellow pre-green
        delay(1500);

        // Step 4: Activate green for Light 1 and Light 4
        digitalWrite(VEHICLE_YELLOW_1, LOW);   // Light 1: Yellow OFF
        digitalWrite(VEHICLE_GREEN_1, HIGH);   // Light 1: Green ON
        digitalWrite(VEHICLE_YELLOW_4, LOW);   // Light 4: Yellow OFF
        digitalWrite(VEHICLE_GREEN_4, HIGH);   // Light 4: Green ON

        // Pedestrian Lights (Straight):
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);  // Light 1: Straight pedestrians cross
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);  // Light 4: Straight pedestrians cross
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);     // Light 1: Straight red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);     // Light 4: Straight red OFF

        // Pedestrian Lights (Left):
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);   // Light 3: Left pedestrians cross?
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);   // Light 2: Left pedestrians cross?
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);      // Light 3: Left red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);      // Light 2: Left red OFF
      }
      break;
    }
  }
  return true;  // Placeholder return; no current use of boolean result.
}

// =============================================================================
//                                   measureDistance(int trigPin, int echoPin, int number)
// =============================================================================

// ---------------------------
// Function: measureDistance(int trigPin, int echoPin, int number)
// Purpose: Read an ultrasonic sensor, check for triggers (obstacles or button presses), and update traffic light states.
// Parameters:
//   - trigPin: Trigger pin of the ultrasonic sensor (output pin to send pulse)
//   - echoPin: Echo pin of the ultrasonic sensor (input pin to receive pulse)
//   - number: Traffic light identifier (1-4) associated with this sensor
// Returns: long (Distance in centimeters, or -1 if pulseIn() fails)
// ---------------------------
long measureDistance(int trigPin, int echoPin, int number) {
  // ---------------------------
  // Step 1: Trigger Ultrasonic Pulse
  // ---------------------------
  // Ultrasonic sensors require a 10µs trigger pulse to send sound waves.
  // First, pull trigger LOW to stabilize (some sensors need this).
  digitalWrite(trigPin, LOW);
  delay(5);  // Short delay to ensure stability (5ms)

  // Send trigger pulse (HIGH for 10µs)
  digitalWrite(trigPin, HIGH);
  delay(10);  // Pulse duration (10µs); adjust if needed for your sensor.
  digitalWrite(trigPin, LOW);  // Stop the trigger pulse

  // ---------------------------
  // Step 2: Measure Echo Pulse Duration
  // ---------------------------
  // The sensor sends a HIGH pulse on echoPin for the duration it takes sound to
  // travel to the obstacle and back. pulseIn() measures this duration in microseconds.
  // Returns 0 if no echo received (sensor error) or duration > 65535µs (obstacle too far).
  long duration = pulseIn(echoPin, HIGH);  // Units: microseconds

  // ---------------------------
  // Step 3: Calculate Distance
  // ---------------------------
  // Sound travels at ~3432 cm/s (speed of sound in air at 20°C).
  // Time for signal to go and return: duration (µs) → convert to seconds: duration / 1e6.
  // Distance = (time * speed) / 2 (since sound travels to obstacle and back).
  // Formula: (duration / 2) * (3432 cm/s) → simplified to (duration * 0.03432) cm.
  long distance = (duration / 2) * 0.03432;  // Units: centimeters

  // ---------------------------
  // Step 4: Check for Triggers (Update Lights if Needed)
  // ---------------------------
  // Conditions to trigger light state change:
  // - distance == 0: No echo received (sensor disconnected or error)
  // - distance >= 500cm: Obstacle too far (sensor limit or no obstacle)
  // - buttonState > 0: A button was pressed (manual override)
  if (distance == 0 || distance >= 500 || buttonState > 0) {
    // ---------------------------
    // Priority: Button Press Over Sensor Input
    // ---------------------------
    // If a button is pressed, override the sensor number with the button’s state.
    // Example: If buttonState=2 (buttonPin2 pressed), use number=2 instead of the sensor’s original number.
    if (buttonState > 0) {
      number = buttonState;  // Override with button-pressed light number
    }

    // ---------------------------
    // Activate State Transition Logic
    // ---------------------------
    // Call status() with the (possibly overridden) light number to update states.
    status(number);

    // ---------------------------
    // Reset Button State
    // ---------------------------
    // Clear buttonState to avoid repeated triggers until the next press.
    buttonState = 0;
  }

  // ---------------------------
  // Return Measured Distance
  // ---------------------------
  return distance;
}

// =============================================================================
//                                   LOOP FUNCTION (Runs Continuously)
// =============================================================================

void loop() {
  // ---------------------------
  // Clear Serial Console
  // ---------------------------
  // Print a blank line to separate updates in the serial monitor.
  Serial.println("");

  // ---------------------------
  // Read Sensors and Update Lights (Traffic Light 1-4)
  // ---------------------------
  // For each traffic light, read its associated ultrasonic sensor and process input.
  // measureDistance() returns the distance and may trigger light state changes.

  // Traffic Light 1:
  Serial.print("Traffic Light 1: ");
  Serial.print(measureDistance(TRIGA1, ECHOA1, 1));  // Read Sensor 1 (Light 1)
  Serial.print("  ");  // Add spacing for readability

  // Traffic Light 2:
  Serial.print("Traffic Light 2: ");
  Serial.print(measureDistance(TRIGA2, ECHOA2, 2));  // Read Sensor 2 (Light 2)
  Serial.print("  ");

  // Traffic Light 3:
  Serial.print("Traffic Light 3: ");
  Serial.print(measureDistance(TRIGA3, ECHOA3, 3));  // Read Sensor 3 (Light 3)
  Serial.print("  ");

  // Traffic Light 4:
  Serial.print("Traffic Light 4: ");
  Serial.print(measureDistance(TRIGA4, ECHOA4, 4));  // Read Sensor 4 (Light 4)
  Serial.print("  ");

  // ---------------------------
  // Log Button State
  // ---------------------------
  Serial.print("Button State: ");
  Serial.println(buttonState);  // Print current buttonState (0, 1, or 2)

  // ---------------------------
  // Pause Before Next Loop Iteration
  // ---------------------------
  delay(1000);  // Wait 1 second before repeating (1Hz loop rate)
}