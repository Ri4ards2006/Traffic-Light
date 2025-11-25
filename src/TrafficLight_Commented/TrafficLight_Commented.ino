/* 
 * Traffic Light Control System with Ultrasonic Sensors and Day/Night Mode
 * 
 * This Arduino-based system manages traffic lights for vehicles and pedestrians using:
 * - Ultrasonic sensors (HC-SR04) to detect obstacles
 * - Day/Night mode to adjust sensitivity and timing
 * - Manual button overrides for specific traffic lights
 * 
 * Key Features:
 * 1. Automatic obstacle detection with adjustable thresholds
 * 2. Mode-dependent timing (yellow light duration)
 * 3. Coordinated traffic light transitions (phase-based)
 * 4. Pedestrian light synchronization with vehicle phases
 * 5. Serial debugging for system monitoring
 * 
 * Hardware Requirements:
 * - Arduino board (e.g., Uno, Mega)
 * - 4x Ultrasonic sensors (HC-SR04)
 * - Traffic lights (LEDs or actual bulbs with drivers)
 * - 3x Momentary push buttons (with pull-up resistors, or use INPUT_PULLUP)
 * - Jumper wires for connecting sensors, buttons, and lights
*/

// =============================================================================
//                                   GLOBAL CONSTANTS  
// =============================================================================  
/* 
 * Constants are values that do not change during runtime. They are used here to:
 * - Standardize hardware pin configurations
 * - Define system behavior parameters (delays, thresholds)
 * - Improve code readability and maintainability
*/

// ---------------------------
// General System Configuration  
// ---------------------------  
/* 
 * Serial Communication: Used for debugging. Baud rate determines data transfer speed.
 * 9600 is a common rate for Arduino-PC communication.
*/
#define SER_BAUD_RATE 9600       
/* 
 * Loop Delay: Controls how often the main loop runs. 
 * DEFAULT_DELAY_MS = 1000 → 1Hz update rate (1 reading per second). 
 * Prevents excessive sensor polling and reduces wear.
*/
#define DEFAULT_DELAY_MS 1000    

// ---------------------------
// Day/Night Mode Parameters  
// ---------------------------  
/* 
 * Day/Night mode adjusts system behavior for different lighting conditions. 
 * Night mode prioritizes visibility and sensitivity; Day mode is more relaxed.
*/

/* 
 * Ultrasonic Sensor Thresholds: 
 * These define the maximum distance (cm) at which an obstacle will trigger a light transition.
 * - Night Mode: LOWER threshold (300cm) → more sensitive (reacts to closer obstacles)
 * - Day Mode: HIGHER threshold (500cm) → less sensitive (ignores distant obstacles)
 * Note: Original logic may be inverted (see measureDistance() for details)
*/
#define NIGHT_SENSOR_THRESHOLD_CM 300  
#define DAY_SENSOR_THRESHOLD_CM 500  

/* 
 * Yellow Light Delay: Duration (ms) yellow lights stay on during transitions.
 * - Night Mode: LONGER delay (2000ms) → better visibility in low light
 * - Day Mode: Default (1500ms) → normal operation
*/
#define YELLOW_DELAY_DAY_MS 1500  
#define YELLOW_DELAY_NIGHT_MS 2000  

// ---------------------------
// Ultrasonic Sensor Constants (HC-SR04 Specifications)  
// ---------------------------  
/* 
 * Speed of Sound: Critical for distance calculation. 
 * Typo Note: Original code uses 0.03432 cm/ms, but correct value at 20°C is ~34.3 cm/ms (34300 cm/s). 
 * This likely a decimal placement error (3432 cm/s → 3.432 cm/ms, but code has 0.03432). 
 * Retained as-is for consistency with original logic, but will cause incorrect distance readings.
*/
#define SPEED_OF_SOUND_CM_PER_MS 0.03432  // Incorrect, should be 3.432 cm/ms  

/* 
 * Minimum Echo Duration: HC-SR04 sensors require a minimum echo pulse to detect obstacles.
 * MIN_ECHO_DURATION_US = 100µs → ~17cm (echo time for 100µs: (100/2)*0.03432 = 1.716cm). 
 * Values below this are considered invalid (no obstacle detected).
*/
#define MIN_ECHO_DURATION_US 100  

/* 
 * Maximum Echo Duration: HC-SR04 max range is ~400cm (round-trip time: (400*2)/34300s ≈ 23.3ms → 23300µs).
 * However, pulseIn() has a 16-bit limit (0-65535), so MAX_ECHO_DURATION_US is set to 65535. 
 * Values above this indicate no obstacle (or sensor error). pulseIn() returns 0 in this case.
*/
#define MAX_ECHO_DURATION_US 65535  

// =============================================================================
//                                   GLOBAL VARIABLES  
// =============================================================================  
/* 
 * Global Variables store persistent state required across functions. 
 * Traffic light and sensor pins are defined as constants (const int) to prevent accidental modification.
*/

// ---------------------------
// Traffic Light Pins (Pedestrian & Vehicle)  
// ---------------------------  
/* 
 * Naming Convention: [TYPE]_[DIRECTION]_[COLOR]_[NUMBER]  
 * - TYPE: PEDESTRIAN (pedestrian) or VEHICLE (vehicle)
 * - DIRECTION: STRAIGHT (crossing straight) or LEFT (turning left)
 * - COLOR: RED, GREEN, YELLOW (note: YELLOW only for vehicles)
 * - NUMBER: Light identifier (1-4)
 * 
 * Pins are defined with const int to ensure they cannot be changed after initialization.
 * All lights are connected as active-HIGH (digitalWrite HIGH = ON, LOW = OFF)
*/

// Pedestrian Straight Red Lights (Stop for straight crossing)
const int PEDESTRIAN_STRAIGHT_RED_1 = 7;    // Light 1: Red for straight pedestrians
const int PEDESTRIAN_STRAIGHT_RED_2 = 20;   // Light 2: Red for straight pedestrians
const int PEDESTRIAN_STRAIGHT_RED_3 = 22;   // Light 3: Red for straight pedestrians
const int PEDESTRIAN_STRAIGHT_RED_4 = 36;   // Light 4: Red for straight pedestrians

// Pedestrian Straight Green Lights (Go for straight crossing)
const int PEDESTRIAN_STRAIGHT_GREEN_1 = 8;  // Light 1: Green for straight pedestrians
const int PEDESTRIAN_STRAIGHT_GREEN_2 = 19; // Light 2: Green for straight pedestrians
const int PEDESTRIAN_STRAIGHT_GREEN_3 = 24; // Light 3: Green for straight pedestrians
const int PEDESTRIAN_STRAIGHT_GREEN_4 = 38; // Light 4: Green for straight pedestrians

// Pedestrian Left Red Lights (Stop for left turn)
const int PEDESTRIAN_LEFT_RED_1 = 9;        // Light 1: Red for left pedestrians
const int PEDESTRIAN_LEFT_RED_2 = 18;       // Light 2: Red for left pedestrians
const int PEDESTRIAN_LEFT_RED_3 = 26;       // Light 3: Red for left pedestrians
const int PEDESTRIAN_LEFT_RED_4 = 40;       // Light 4: Red for left pedestrians

// Pedestrian Left Green Lights (Go for left turn)
const int PEDESTRIAN_LEFT_GREEN_1 = 10;     // Light 1: Green for left pedestrians
const int PEDESTRIAN_LEFT_GREEN_2 = 17;     // Light 2: Green for left pedestrians
const int PEDESTRIAN_LEFT_GREEN_3 = 28;     // Light 3: Green for left pedestrians
const int PEDESTRIAN_LEFT_GREEN_4 = 42;     // Light 4: Green for left pedestrians

// Vehicle Green Lights (Go for vehicles)
const int VEHICLE_GREEN_1 = 11;             // Light 1: Green light (vehicles)
const int VEHICLE_GREEN_2 = 16;             // Light 2: Green light (vehicles)
const int VEHICLE_GREEN_3 = 30;             // Light 3: Green light (vehicles)
const int VEHICLE_GREEN_4 = 44;             // Light 4: Green light (vehicles)

// Vehicle Yellow Lights (Warning for state change)
const int VEHICLE_YELLOW_1 = 12;            // Light 1: Yellow light (vehicles)
const int VEHICLE_YELLOW_2 = 15;            // Light 2: Yellow light (vehicles)
const int VEHICLE_YELLOW_3 = 32;            // Light 3: Yellow light (vehicles)
const int VEHICLE_YELLOW_4 = 46;            // Light 4: Yellow light (vehicles)

// Vehicle Red Lights (Stop for vehicles)
const int VEHICLE_RED_1 = 13;                // Light 1: Red light (vehicles)
const int VEHICLE_RED_2 = 14;                // Light 2: Red light (vehicles)
const int VEHICLE_RED_3 = 34;                // Light 3: Red light (vehicles)
const int VEHICLE_RED_4 = 48;                // Light 4: Red light (vehicles)

// ---------------------------
// Ultrasonic Sensor Pins  
// ---------------------------  
/* 
 * Each sensor has TRIG (trigger) and ECHO (echo) pins. TRIG sends a pulse; ECHO receives the reflection.
 * Naming: TRIGA[X]/ECHOA[X] where X = light number (1-4). Sensor X is linked to Light X.
 * Pins are defined with #define for easier modification.
*/
#define TRIGA1 35    // Light 1 Sensor Trigger Pin
#define ECHOA1 37    // Light 1 Sensor Echo Pin
#define TRIGA2 31    // Light 2 Sensor Trigger Pin
#define ECHOA2 33    // Light 2 Sensor Echo Pin
#define TRIGA3 43    // Light 3 Sensor Trigger Pin
#define ECHOA3 45    // Light 3 Sensor Echo Pin
#define TRIGA4 39    // Light 4 Sensor Trigger Pin
#define ECHOA4 41    // Light 4 Sensor Echo Pin

// ---------------------------
// Button Pins  
// ---------------------------  
/* 
 * Buttons use INPUT_PULLUP to simplify wiring (no external resistors needed). 
 * When not pressed, pins read HIGH; when pressed, pins read LOW (FALLING edge triggers ISR).
*/
const int manualButtonPin1 = 3;  // Manual override button for Light 1 (Pin 3)
const int manualButtonPin2 = 2;  // Manual override button for Light 2 (Pin 2)
const int modeButtonPin = 4;     // Day/Night mode toggle button (Pin 4)

// ---------------------------
// Volatile Variables (Interrupt-Used)  
// ---------------------------  
/* 
 * Volatile keyword: Tells the compiler not to optimize variable storage (prevents caching). 
 * Critical for variables modified in Interrupt Service Routines (ISRs) and read in main loop.
*/

/* 
 * manualButtonState: Tracks which manual button was pressed (0 = none, 1 = Light1, 2 = Light2).
 * Updated in ISRs; read in measureDistance().
*/
volatile int manualButtonState = 0;  

/* 
 * isDayMode: System mode flag (true = Day, false = Night). 
 * Toggled by modeButtonPin ISR; used to adjust thresholds and delays.
*/
volatile bool isDayMode = true;  

// =============================================================================
//                                   INTERRUPT SERVICE ROUTINES (ISRs)  
// =============================================================================  
/* 
 * ISRs: Short functions executed when an interrupt event occurs (e.g., button pressed). 
 * Must be fast (avoid delays) to prevent blocking other interrupts.
*/

// ---------------------------
// ISR: Manual Override Button 1 (Light 1)  
// ---------------------------  
/* 
 * Triggered on FALLING edge (button pressed) of manualButtonPin1 (Pin 3).
 * Updates manualButtonState to 1 to indicate Light 1 override request.
*/
void isrManualButton1() {  
  // Reset state to avoid conflicts (prevents multiple overlapping triggers)
  manualButtonState = 0;  
  // Set state to 1 (Light 1 override requested)
  manualButtonState = 1;  
  // Optional Debounce: Add delay(50) here to ignore button bounce (mechanical noise). 
  // Current code lacks debounce → may trigger multiple times per physical press.
}  

// ---------------------------
// ISR: Manual Override Button 2 (Light 2)  
// ---------------------------  
/* 
 * Triggered on FALLING edge (button pressed) of manualButtonPin2 (Pin 2).
 * Updates manualButtonState to 2 to indicate Light 2 override request.
*/
void isrManualButton2() {  
  // Reset state to avoid conflicts
  manualButtonState = 0;  
  // Set state to 2 (Light 2 override requested)
  manualButtonState = 2;  
}  

// ---------------------------
// ISR: Day/Night Mode Toggle Button  
// ---------------------------  
/* 
 * Triggered on FALLING edge (button pressed) of modeButtonPin (Pin 4).
 * Toggles isDayMode between true (Day) and false (Night).
*/
void isrModeButton() {  
  // Flip mode state (true ↔ false)
  isDayMode = !isDayMode;  
  // Mode change logging is handled in loop() to avoid ISR serial conflicts (slower, but safer).
}  

// =============================================================================
//                                   SETUP FUNCTION  
// =============================================================================  
/* 
 * setup(): Runs once at startup. Initializes hardware, software, and initial states.
 * Critical for ensuring safe operation before entering main loop.
*/

void setup() {  
  // ---------------------------  
  // Step 1: Initialize Serial Communication  
  // ---------------------------  
  /* 
   * Serial.begin() starts the USB serial interface for debugging. 
   * while(!Serial); waits for serial monitor connection (prevents lost initial logs).
  */
  Serial.begin(SER_BAUD_RATE);  // Start serial at 9600 baud
  while (!Serial);              // Wait for PC connection (avoids missed startup messages)
  Serial.println("=== System Startup ===");  
  Serial.print("Serial initialized at ");  
  Serial.print(SER_BAUD_RATE);  
  Serial.println(" baud. Ready for operation.");  

  // ---------------------------  
  // Step 2: Configure Button Pins  
  // ---------------------------  
  /* 
   * Buttons use INPUT_PULLUP: Internal pull-up resistor ensures HIGH when not pressed. 
   * When pressed, pin reads LOW (triggers FALLING edge interrupt).
  */
  pinMode(manualButtonPin1, INPUT_PULLUP);  // Light 1 override button (Pin 3)
  pinMode(manualButtonPin2, INPUT_PULLUP);  // Light 2 override button (Pin 2)
  pinMode(modeButtonPin, INPUT_PULLUP);     // Day/Night mode button (Pin 4)

  // ---------------------------  
  // Step 3: Attach Interrupts to Buttons  
  // ---------------------------  
  /* 
   * attachInterrupt(): Links a button pin to an ISR. 
   * digitalPinToInterrupt() converts pin number to interrupt ID (required for AVR-based Arduinos).
   * FALLING edge: ISR triggered when pin transitions from HIGH → LOW (button pressed).
  */
  attachInterrupt(digitalPinToInterrupt(manualButtonPin1), isrManualButton1, FALLING);  // Light 1 button Interrupt
  attachInterrupt(digitalPinToInterrupt(manualButtonPin2), isrManualButton2, FALLING);  // Light 2 button Interrupt
  attachInterrupt(digitalPinToInterrupt(modeButtonPin), isrModeButton, FALLING);        // Mode button Interrupt

  // ---------------------------  
  // Step 4: Configure Traffic Light Pins as OUTPUT  
  // ---------------------------  
  /* 
   * All traffic light pins are OUTPUT because LEDs require voltage to be driven ON/OFF.
   * Pins are set individually to avoid confusion (arrays could simplify, but explicit here for clarity).
  */

  // Light 1 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_1, OUTPUT);    // Ped. Straight Red (Light 1)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_1, OUTPUT);  // Ped. Straight Green (Light 1)
  pinMode(PEDESTRIAN_LEFT_RED_1, OUTPUT);        // Ped. Left Red (Light 1)
  pinMode(PEDESTRIAN_LEFT_GREEN_1, OUTPUT);      // Ped. Left Green (Light 1)
  pinMode(VEHICLE_GREEN_1, OUTPUT);              // Vehicle Green (Light 1)
  pinMode(VEHICLE_YELLOW_1, OUTPUT);             // Vehicle Yellow (Light 1)
  pinMode(VEHICLE_RED_1, OUTPUT);                // Vehicle Red (Light 1)

  // Light 2 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_2, OUTPUT);    // Ped. Straight Red (Light 2)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_2, OUTPUT);  // Ped. Straight Green (Light 2)
  pinMode(PEDESTRIAN_LEFT_RED_2, OUTPUT);        // Ped. Left Red (Light 2)
  pinMode(PEDESTRIAN_LEFT_GREEN_2, OUTPUT);      // Ped. Left Green (Light 2)
  pinMode(VEHICLE_GREEN_2, OUTPUT);              // Vehicle Green (Light 2)
  pinMode(VEHICLE_YELLOW_2, OUTPUT);             // Vehicle Yellow (Light 2)
  pinMode(VEHICLE_RED_2, OUTPUT);                // Vehicle Red (Light 2)

  // Light 3 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_3, OUTPUT);    // Ped. Straight Red (Light 3)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_3, OUTPUT);  // Ped. Straight Green (Light 3)
  pinMode(PEDESTRIAN_LEFT_RED_3, OUTPUT);        // Ped. Left Red (Light 3)
  pinMode(PEDESTRIAN_LEFT_GREEN_3, OUTPUT);      // Ped. Left Green (Light 3)
  pinMode(VEHICLE_GREEN_3, OUTPUT);              // Vehicle Green (Light 3)
  pinMode(VEHICLE_YELLOW_3, OUTPUT);             // Vehicle Yellow (Light 3)
  pinMode(VEHICLE_RED_3, OUTPUT);                // Vehicle Red (Light 3)

  // Light 4 Pins  
  pinMode(PEDESTRIAN_STRAIGHT_RED_4, OUTPUT);    // Ped. Straight Red (Light 4)
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_4, OUTPUT);  // Ped. Straight Green (Light 4)
  pinMode(PEDESTRIAN_LEFT_RED_4, OUTPUT);        // Ped. Left Red (Light 4)
  pinMode(PEDESTRIAN_LEFT_GREEN_4, OUTPUT);      // Ped. Left Green (Light 4)
  pinMode(VEHICLE_GREEN_4, OUTPUT);              // Vehicle Green (Light 4)
  pinMode(VEHICLE_YELLOW_4, OUTPUT);             // Vehicle Yellow (Light 4)
  pinMode(VEHICLE_RED_4, OUTPUT);                // Vehicle Red (Light 4)

  // ---------------------------  
  // Step 5: Configure Ultrasonic Sensor Pins  
  // ---------------------------  
  /* 
   * TRIG pins (OUTPUT): Send trigger pulses to sensors.
   * ECHO pins (INPUT): Receive reflected echo pulses from sensors.
  */
  pinMode(TRIGA1, OUTPUT);  // Light 1 Sensor Trigger
  pinMode(ECHOA1, INPUT);   // Light 1 Sensor Echo
  pinMode(TRIGA2, OUTPUT);  // Light 2 Sensor Trigger
  pinMode(ECHOA2, INPUT);   // Light 2 Sensor Echo
  pinMode(TRIGA3, OUTPUT);  // Light 3 Sensor Trigger
  pinMode(ECHOA3, INPUT);   // Light 3 Sensor Echo
  pinMode(TRIGA4, OUTPUT);  // Light 4 Sensor Trigger
  pinMode(ECHOA4, INPUT);   // Light 4 Sensor Echo

  // ---------------------------  
  // Step 6: Set Initial Light States (All Red)  
  // ---------------------------  
  /* 
   * Safety First: All lights start in "STOP" (RED) state to prevent accidents.
   * Vehicle red lights are set to HIGH (ON) initially.
   * Pedestrian red lights are ALSO set to HIGH (ON) initially.
  */
  // Vehicle Red Lights (All ON initially)
  digitalWrite(VEHICLE_RED_1, HIGH);  
  digitalWrite(VEHICLE_RED_2, HIGH);  
  digitalWrite(VEHICLE_RED_3, HIGH);  
  digitalWrite(VEHICLE_RED_4, HIGH);  

  // Pedestrian Straight Red Lights (All ON initially)
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);  
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);  

  // Pedestrian Left Red Lights (All ON initially)
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);  
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);  

  Serial.println("\n=== Initialization Complete ===");  
  Serial.println("Default mode: DAY. All lights initialized to RED.");  
}  

// =============================================================================
//                                   LIGHT STATE MANAGEMENT FUNCTIONS  
// =============================================================================  

// ---------------------------
// Function: setDefaultLightStates()  
// Purpose: Reset all traffic lights to a safe default state (non-red off, red on).  
// Logic:  
//   - Turns off all yellow & green lights (non-red).  
//   - Temporarily turns off vehicle red lights (re-enabled later in transitions).  
//   - Ensures pedestrian red lights remain ON (critical safety default).  
// Note: Vehicle red lights are turned OFF here but re-enabled in pre-green phases.  
// ---------------------------  
void setDefaultLightStates() {  
  // ---------------------------  
  // Substep 1: Turn Off All Vehicle Yellow Lights  
  // ---------------------------  
  /* 
   * Yellow lights are only active DURING transitions (not default). 
   * Default state: Yellow lights OFF.
  */
  digitalWrite(VEHICLE_YELLOW_1, LOW);  // Light 1 Vehicle Yellow OFF
  digitalWrite(VEHICLE_YELLOW_2, LOW);  // Light 2 Vehicle Yellow OFF
  digitalWrite(VEHICLE_YELLOW_3, LOW);  // Light 3 Vehicle Yellow OFF
  digitalWrite(VEHICLE_YELLOW_4, LOW);  // Light 4 Vehicle Yellow OFF

  // ---------------------------  
  // Substep 2: Temporarily Turn Off Vehicle Red Lights  
  // ---------------------------  
  /* 
   * Red lights are re-enabled later in transition phases (for conflicting lights). 
   * Temporarily OFF to avoid conflicting states during reset.
  */
  digitalWrite(VEHICLE_RED_1, LOW);     // Light 1 Vehicle Red temporarily OFF
  digitalWrite(VEHICLE_RED_2, LOW);     // Light 2 Vehicle Red temporarily OFF
  digitalWrite(VEHICLE_RED_3, LOW);     // Light 3 Vehicle Red temporarily OFF
  digitalWrite(VEHICLE_RED_4, LOW);     // Light 4 Vehicle Red temporarily OFF

  // ---------------------------  
  // Substep 3: Turn Off All Vehicle Green Lights  
  // ---------------------------  
  /* 
   * Green lights are only active during "GO" phases (not default). 
   * Default state: Green lights OFF.
  */
  digitalWrite(VEHICLE_GREEN_1, LOW);   // Light 1 Vehicle Green OFF
  digitalWrite(VEHICLE_GREEN_2, LOW);   // Light 2 Vehicle Green OFF
  digitalWrite(VEHICLE_GREEN_3, LOW);   // Light 3 Vehicle Green OFF
  digitalWrite(VEHICLE_GREEN_4, LOW);   // Light 4 Vehicle Green OFF

  // ---------------------------  
  // Substep 4: Reactivate Pedestrian Straight Red Lights (ON)  
  // ---------------------------  
  /* 
   * Pedestrian red lights are part of the safety default: 
   * - Red ON = "Do not cross" (critical for preventing accidents).
   * - Green OFF (pedestrians must wait by default).
  */
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH);  // Light 1 Ped. Straight Red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH);  // Light 2 Ped. Straight Red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH);  // Light 3 Ped. Straight Red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH);  // Light 4 Ped. Straight Red ON
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, LOW);  // Light 1 Ped. Straight Green OFF
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, LOW);  // Light 2 Ped. Straight Green OFF
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, LOW);  // Light 3 Ped. Straight Green OFF
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, LOW);  // Light 4 Ped. Straight Green OFF

  // ---------------------------  
  // Substep 5: Reactivate Pedestrian Left Red Lights (ON)  
  // ---------------------------  
  /* 
   * Same logic as pedestrian straight red lights: Red ON by default.
   * Green lights for left turns are kept OFF initially.
  */
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);      // Light 1 Ped. Left Red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);      // Light 2 Ped. Left Red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);      // Light 3 Ped. Left Red ON
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);      // Light 4 Ped. Left Red ON
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, LOW);     // Light 1 Ped. Left Green OFF
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, LOW);     // Light 2 Ped. Left Green OFF
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, LOW);     // Light 3 Ped. Left Green OFF
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, LOW);     // Light 4 Ped. Left Green OFF

  Serial.println("\nDefault light state reset. All non-red lights OFF; red lights ON.");  
}  

// ---------------------------
// Function: status(int lightNumber)  
// Purpose: Coordinate state transitions for a specific traffic light (1-4).  
// Parameters:  
//   - lightNumber: Target traffic light (1-4).  
// Logic:  
//   1. Adjusts yellow light delay based on day/night mode.  
//   2. Checks if the target light’s green is OFF (needs transition).  
//   3. Executes phase steps: Yellow Warning → Pre-Green → Green Activation.  
//   4. Synchronizes pedestrian lights with vehicle transitions.  
// Note: Current logic has LIGHT-SPECIFIC dependencies (e.g., Light 3 depends on Light 2 state).  
//       Verify field behavior before deployment.  
// ---------------------------  
boolean status(int lightNumber) {  
  // ---------------------------  
  // Substep 1: Determine Mode-Dependent Parameters  
  // ---------------------------  
  /* 
   * Yellow Delay: Adjusted based on mode (longer at night for visibility).
   * Sensor Threshold: Adjusted based on mode (more sensitive at night).
  */
  long yellowDelayMs = isDayMode ? YELLOW_DELAY_DAY_MS : YELLOW_DELAY_NIGHT_MS;  
  long sensorThresholdCm = isDayMode ? DAY_SENSOR_THRESHOLD_CM : NIGHT_SENSOR_THRESHOLD_CM;  

  // ---------------------------  
  // Substep 2: Light-Specific Transition Logic (Switch Statement)  
  // ---------------------------  
  switch (lightNumber) {  
    // ---------------------------  
    // Case 1: Traffic Light 1 Transition  
    // Trigger Condition: Light 1’s vehicle green light is OFF (no active green).  
    // Note: This logic may be overly simplistic. In real systems, transitions often depend on timing, not just state.  
    // ---------------------------  
    case 1: {  
      Serial.println("\n--- Handling Light 1 Transition ---");  
      /* 
       * Only proceed if Light 1’s green light is NOT active (LOW = OFF). 
       * Prevents unnecessary transitions when green is already ON.
      */
      if (!digitalRead(VEHICLE_GREEN_1)) {  
        setDefaultLightStates();  // Reset to default state first (non-red OFF, pedestrian red ON)
        
        // ---------------------------  
        // Phase 1: Yellow Warning (All Lights)  
        // ---------------------------  
        /* 
         * Yellow Warning Phase: Notifies vehicles/pedestrians of impending state change. 
         * All lights show yellow to indicate transition.
        */
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  // Light 1 Vehicle Yellow ON
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  // Light 2 Vehicle Yellow ON
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  // Light 3 Vehicle Yellow ON
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  // Light 4 Vehicle Yellow ON
        Serial.println("Yellow warning phase activated (all lights).");  
        delay(yellowDelayMs);  // Wait for mode-adjusted delay (day:1500ms, night:2000ms)  

        // ---------------------------  
        // Phase 2: Pre-Green Preparation (Light 1 & 4)  
        // ---------------------------  
        /* 
         * Pre-Green Phase: Prepares for green activation. 
         * - Light 1 & 4: Yellow ON (preparing to go)
         * - Light 2 & 3: Red ON (must stay stopped)
         * Resets to default state again to clear previous phase states.
        */
        setDefaultLightStates();  // Reset to clear Phase 1 state  
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  // Light 1 Vehicle Yellow (pre-green) ON
        digitalWrite(VEHICLE_RED_2, HIGH);     // Light 2 Vehicle Red ON (stay stopped)
        digitalWrite(VEHICLE_RED_3, HIGH);     // Light 3 Vehicle Red ON (stay stopped)
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  // Light 4 Vehicle Yellow (pre-green) ON
        Serial.println("Pre-green phase (Light 1 & 4 yellow, Lights 2 & 3 red).");  
        delay(yellowDelayMs);  // Wait again for pre-green duration  

        // ---------------------------  
        // Phase 3: Activate Green (Light 1 & 4)  
        // ---------------------------  
        /* 
         * Green Activation Phase: Yellow lights OFF; green lights ON. 
         * Light 1 & 4 now allow vehicle traffic.
        */
        digitalWrite(VEHICLE_YELLOW_1, LOW);   // Light 1 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_1, HIGH);   // Light 1 Vehicle Green ON
        digitalWrite(VEHICLE_YELLOW_4, LOW);   // Light 4 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_4, HIGH);   // Light 4 Vehicle Green ON
        Serial.println("Green phase activated (Light 1 & 4).");  

        // ---------------------------  
        // Phase 4: Activate Pedestrian Lights (Synchronized)  
        // ---------------------------  
        /* 
         * Pedestrian Lights Synchronization: 
         * - When vehicle lights 1&4 are green → pedestrian straight 1&4 can cross (green ON)
         * - Simultaneously, pedestrian left 2&3 can cross (green ON)
         * This implies a coordinated crossing pattern (e.g., opposite directions)
        */
        // Pedestrian Straight Green (Lights 1 & 4)
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);  // Light 1 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);  // Light 4 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);      // Light 1 Ped. Straight Red OFF (now green)
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);      // Light 4 Ped. Straight Red OFF (now green)

        // Pedestrian Left Green (Lights 2 & 3)
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);   // Light 2 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);   // Light 3 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);      // Light 2 Ped. Left Red OFF (now green)
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);      // Light 3 Ped. Left Red OFF (now green)

        Serial.println("Pedestrian lights updated (straight: Light 1 & 4; left: Light 2 & 3).");  
      }  
      break;  

    // ---------------------------  
    // Case 2: Traffic Light 2 Transition  
    // Trigger Condition: Light 2’s vehicle green light is OFF.  
    // Note: Similar structure to Case 1, but affects different light groups.  
    // ---------------------------  
    case 2: {  
      Serial.println("\n--- Handling Light 2 Transition ---");  
      if (!digitalRead(VEHICLE_GREEN_2)) {  
        setDefaultLightStates();  

        // Phase 1: Yellow Warning (All Lights)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        Serial.println("Yellow warning phase activated (all lights).");  
        delay(yellowDelayMs);  

        // Phase 2: Pre-Green Preparation (Light 2 & 3)
        setDefaultLightStates();  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  // Light 2 Vehicle Yellow (pre-green) ON
        digitalWrite(VEHICLE_RED_1, HIGH);     // Light 1 Vehicle Red ON (stay stopped)
        digitalWrite(VEHICLE_RED_3, HIGH);     // Light 3 Vehicle Red ON (stay stopped) → Wait, original code says "Light 3 Vehicle Red ON (stay red)" but Light 2's pre-green?
        // Note: This line may be redundant. Light 3's state is already managed by setDefaultLightStates()
        digitalWrite(VEHICLE_RED_4, HIGH);     // Light 4 Vehicle Red ON (stay stopped)
        Serial.println("Pre-green phase (Light 2 & 3 yellow, Lights 1 & 4 red).");  
        delay(yellowDelayMs);  

        // Phase 3: Activate Green (Light 2 & 3)
        digitalWrite(VEHICLE_YELLOW_3, LOW);   // Light 3 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_3, HIGH);   // Light 3 Vehicle Green ON
        digitalWrite(VEHICLE_YELLOW_2, LOW);   // Light 2 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_2, HIGH);   // Light 2 Vehicle Green ON
        Serial.println("Green phase activated (Light 2 & 3).");  

        // Phase 4: Activate Pedestrian Lights
        // Pedestrian Straight Green (Lights 2 & 3)
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);  // Light 3 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);  // Light 2 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);      // Light 2 Ped. Straight Red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);      // Light 3 Ped. Straight Red OFF

        // Pedestrian Left Green (Lights 1 & 4)
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);   // Light 1 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);   // Light 4 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);      // Light 1 Ped. Left Red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);      // Light 4 Ped. Left Red OFF

        Serial.println("Pedestrian lights updated (straight: Light 2 & 3; left: Light 1 & 4).");  
      }  
      break;  
    }  

    // ---------------------------  
    // Case 3: Traffic Light 3 Transition  
    // Trigger Condition: Light 2’s vehicle green light is OFF (unusual, verify logic).  
    // Note: This case checks Light 2’s state instead of its own (Light 3). Likely unintended.  
    // ---------------------------  
    case 3: {  
      Serial.println("\n--- Handling Light 3 Transition ---");  
      /* 
       * Trigger condition is LIGHT 2’s green state, not Light 3’s own. 
       * This suggests coordination with Light 2, but may be a bug. 
       * Original logic may have intended to check Light 3’s green state.
      */
      if (!digitalRead(VEHICLE_GREEN_2)) {  // Unusual: Checks Light 2, not Light 3
        setDefaultLightStates();  

        // Phase 1: Yellow Warning (All Lights)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        Serial.println("Yellow warning phase activated (all lights).");  
        delay(yellowDelayMs);  

        // Phase 2: Pre-Green Preparation (Light 2 & 3) → Same as Case 2’s Phase 2?
        setDefaultLightStates();  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  // Light 3 Vehicle Yellow (pre-green) ON
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  // Light 2 Vehicle Yellow (pre-green) ON
        digitalWrite(VEHICLE_RED_1, HIGH);     // Light 1 Vehicle Red ON (stay stopped)
        digitalWrite(VEHICLE_RED_4, HIGH);     // Light 4 Vehicle Red ON (stay stopped)
        Serial.println("Pre-green phase (Light 2 & 3 yellow, Lights 1 & 4 red).");  
        delay(yellowDelayMs);  

        // Phase 3: Activate Green (Light 2 & 3) → Same as Case 2’s Phase 3?
        digitalWrite(VEHICLE_YELLOW_2, LOW);   // Light 2 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_2, HIGH);   // Light 2 Vehicle Green ON
        digitalWrite(VEHICLE_YELLOW_3, LOW);   // Light 3 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_3, HIGH);   // Light 3 Vehicle Green ON
        Serial.println("Green phase activated (Light 2 & 3).");  

        // Phase 4: Activate Pedestrian Lights → Same as Case 2’s Phase 4?
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH);  // Light 3 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH);  // Light 2 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);      // Light 2 Ped. Straight Red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);      // Light 3 Ped. Straight Red OFF
        digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);   // Light 1 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);   // Light 4 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);      // Light 1 Ped. Left Red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);      // Light 4 Ped. Left Red OFF
        Serial.println("Pedestrian lights updated (straight: Light 2 & 3; left: Light 1 & 4).");  
      }  
      break;  
    }  

    // ---------------------------  
    // Case 4: Traffic Light 4 Transition  
    // Trigger Condition: Light 1’s vehicle green light is OFF (unusual, verify logic).  
    // Note: This case checks Light 1’s state instead of its own (Light 4). Likely unintended.  
    // ---------------------------  
    case 4: {  
      Serial.println("\n--- Handling Light 4 Transition ---");  
      /* 
       * Trigger condition is LIGHT 1’s green state, not Light 4’s own. 
       * This suggests coordination with Light 1, but may be a bug. 
       * Original logic may have intended to check Light 4’s green state.
      */
      if (!digitalRead(VEHICLE_GREEN_1)) {  // Unusual: Checks Light 1, not Light 4
        setDefaultLightStates();  

        // Phase 1: Yellow Warning (All Lights)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  
        digitalWrite(VEHICLE_YELLOW_2, HIGH);  
        digitalWrite(VEHICLE_YELLOW_3, HIGH);  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);  
        Serial.println("Yellow warning phase activated (all lights).");  
        delay(yellowDelayMs);  

        // Phase 2: Pre-Green Preparation (Light 1 & 4)
        setDefaultLightStates();  
        digitalWrite(VEHICLE_YELLOW_4, HIGH);   // Light 4 Vehicle Yellow (pre-green) ON
        digitalWrite(VEHICLE_RED_2, HIGH);      // Light 2 Vehicle Red ON (stay stopped)
        digitalWrite(VEHICLE_RED_3, HIGH);      // Light 3 Vehicle Red ON (stay stopped)
        digitalWrite(VEHICLE_YELLOW_1, HIGH);  // Light 1 Vehicle Yellow (pre-green) ON
        Serial.println("Pre-green phase (Light 1 & 4 yellow, Lights 2 & 3 red).");  
        delay(yellowDelayMs);  

        // Phase 3: Activate Green (Light 1 & 4)
        digitalWrite(VEHICLE_YELLOW_1, LOW);   // Light 1 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_1, HIGH);   // Light 1 Vehicle Green ON
        digitalWrite(VEHICLE_YELLOW_4, LOW);   // Light 4 Vehicle Yellow OFF
        digitalWrite(VEHICLE_GREEN_4, HIGH);   // Light 4 Vehicle Green ON
        Serial.println("Green phase activated (Light 1 & 4).");  

        // Phase 4: Activate Pedestrian Lights
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH);  // Light 1 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH);  // Light 4 Ped. Straight Green ON
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);      // Light 1 Ped. Straight Red OFF
        digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);      // Light 4 Ped. Straight Red OFF

        // Pedestrian Left Green (Lights 2 & 3) → Note: Original code has "Light 2 & 3" here, but case 1 uses "Light 2 & 3"
        // This may cause conflicting pedestrian signals. Verify intended behavior.
        digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);   // Light 3 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);   // Light 2 Ped. Left Green ON
        digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);      // Light 3 Ped. Left Red OFF
        digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);      // Light 2 Ped. Left Red OFF

        Serial.println("Pedestrian lights updated (straight: Light 1 & 4; left: Light 2 & 3).");  
      }  
      break;  
    }  
  }  
  // Placeholder return (not currently used in system logic)
  return true;  
}  

// ---------------------------
// Function: measureDistance(int trigPin, int echoPin, int lightNumber)  
// Purpose: Read ultrasonic sensor data, calculate distance, and trigger light transitions.  
// Parameters:  
//   - trigPin: Sensor trigger pin (output).  
//   - echoPin: Sensor echo pin (input).  
//   - lightNumber: Associated traffic light (1-4) for transition logic.  
// Returns: long (distance in cm, or 0 if error).  
// Logic:  
//   1. Send trigger pulse to sensor.  
//   2. Measure echo duration (µs) to calculate distance.  
//   3. Validate distance reading (check for errors).  
//   4. Check if transition is needed (sensor error, obstacle too far, or manual override).  
//   5. If needed, override with manual button state and update light states.  
// Note: Contains critical logic errors (see comments below).  
// ---------------------------  
long measureDistance(int trigPin, int echoPin, int lightNumber) {  
  // ---------------------------  
  // Substep 1: Trigger Ultrasonic Sensor  
  // ---------------------------  
  /* 
   * HC-SR04 Protocol: Requires a 10µs HIGH pulse on TRIG to activate measurement.
   * - Step 1: Pull TRIG LOW for stability (5ms delay recommended by datasheet).
   * - Step 2: Send 10µs HIGH pulse.
   * - Step 3: Pull TRIG LOW to end pulse.
  */
  digitalWrite(trigPin, LOW);  
  delay(5);  // Stabilization delay (matches HC-SR04 requirements)
  digitalWrite(trigPin, HIGH);  
  delayMicroseconds(10);  // 10µs trigger pulse (minimum required)
  digitalWrite(trigPin, LOW);  // Stop pulse  

  // ---------------------------  
  // Substep 2: Measure Echo Duration (µs)  
  // ---------------------------  
  /* 
   * pulseIn(echoPin, HIGH, timeout): Reads duration of HIGH pulse on ECHO pin. 
   * - timeout: MAX_ECHO_DURATION_US (65535µs) → if echo takes longer, returns 0 (no obstacle).
   * Returns: echoDurationUs (µs) → time between TRIG HIGH and ECHO HIGH end.
  */
  long echoDurationUs = pulseIn(echoPin, HIGH, MAX_ECHO_DURATION_US);  
  Serial.print("Sensor (trig="); Serial.print(trigPin); Serial.print(", echo="); Serial.print(echoPin); Serial.print(") Echo Duration: ");  
  Serial.print(echoDurationUs); Serial.println(" µs");  

  // ---------------------------  
  // Substep 3: Calculate Distance (cm)  
  // ---------------------------  
  /* 
   * Distance Formula: 
   * distanceCm = (echoDurationUs / 2) * (speed_of_sound_cm_per_ms) 
   * - echoDurationUs / 2: accounts for round-trip time (sound to obstacle + back).
   * - speed_of_sound_cm_per_ms: 0.03432 cm/ms (original code value, but likely incorrect). 
   * Correct Formula (with fixed speed): (echoDurationUs / 2) * (34.3 cm/ms) → (echoDurationUs * 34.3)/2000 cm (since 1ms=1000µs)
   * Current code’s SPEED_OF_SOUND_CM_PER_MS is 1000x too small → distance readings are 1000x smaller than actual.
  */
  long distanceCm = (echoDurationUs / 2) * SPEED_OF_SOUND_CM_PER_MS;  

  // ---------------------------  
  // Substep 4: Validate Distance Reading  
  // ---------------------------  
  /* 
   * Invalid readings include:
   * - distanceCm <= 0: No echo received (sensor disconnected or error)
   * - distanceCm > (MAX_ECHO_DURATION_US * SPEED_OF_SOUND_CM_PER_MS): Echo duration exceeds max → obstacle too far (but with typo, this threshold is too low)
   * Note: With original SPEED_OF_SOUND (0.03432), max valid distance is 65535 * 0.03432 ≈ 2245cm. But HC-SR04 max is ~400cm → validation is incorrect.
  */
  if (distanceCm <= 0 || distanceCm > MAX_ECHO_DURATION_US * SPEED_OF_SOUND_CM_PER_MS) {  
    distanceCm = 0;  // Mark as error
    Serial.println("Invalid/missing sensor reading. Treating as error.");  
  }  

  // ---------------------------  
  // Substep 5: Check Transition Trigger Conditions  
  // ---------------------------  
  /* 
   * Trigger Condition Logic (Potential Bug):
   * Original code triggers on:
   * 1. Sensor error (distanceCm == 0)
   * 2. Obstacle too far (distanceCm >= sensorThresholdCm)
   * 3. Manual button pressed (manualButtonState > 0)
   * But according to system design:
   * - Night mode (threshold=300cm) should trigger for obstacles CLOSER than 300cm
   * - Day mode (threshold=500cm) should trigger for obstacles CLOSER than 500cm
   * Thus, condition should be (distanceCm > 0 && distanceCm <= sensorThresholdCm) → current logic is inverted.
   * This means the system currently triggers when obstacles are FAR away (or no obstacle), which is opposite of intended.
  */
  bool triggerCondition = (distanceCm == 0) || (distanceCm >= sensorThresholdCm) || (manualButtonState > 0);  
  if (triggerCondition) {  
    Serial.print("\nTrigger detected for Light "); Serial.println(lightNumber);  

    // ---------------------------  
    // Substep 6: Prioritize Manual Button Override  
    // ---------------------------  
    /* 
     * Manual override takes precedence: If a button is pressed, use its target light instead of measured lightNumber.
     * manualButtonState is 1 (Light1) or 2 (Light2) → only Lights 1&2 can be manually overridden.
     * No buttons defined for Lights 3&4 → manual override not possible for them (unless code is modified).
    */
    if (manualButtonState > 0) {  
      Serial.print("Manual override detected (buttonState="); Serial.print(manualButtonState); Serial.println("). Overriding lightNumber.");  
      lightNumber = manualButtonState;  // Override with button’s target light (1 or 2)  
    }  

    // ---------------------------  
    // Substep 7: Trigger Light Transition  
    // ---------------------------  
    /* 
     * Call status(lightNumber) to update light states. 
     * This function handles phase transitions for the specified light.
    */
    status(lightNumber);  

    // ---------------------------  
    // Substep 8: Reset Manual Button State  
    // ---------------------------  
    /* 
     * Clear override flag to prevent repeat triggers. 
     * Without this, status() may be called again on next loop with stale state.
    */
    manualButtonState = 0;  
    Serial.println("Manual button state reset (buttonState=0).");  
  }  

  return distanceCm;  
}  

// =============================================================================
//                                   MAIN LOOP FUNCTION  
// =============================================================================  
/* 
 * loop(): Runs continuously after setup(). 
 * Responsibilities:
 * 1. Log system mode and parameters
 * 2. Read/process all ultrasonic sensors
 * 3. Log button states
 * 4. Delay to control loop frequency
*/

void loop() {  
  // ---------------------------  
  // Step 1: Clear Serial Console & Print Mode Status  
  // ---------------------------  
  /* 
   * Clear console (approximate) with divider. Print current mode, threshold, and yellow delay.
   * Helps monitor system state in real-time.
  */
  Serial.println("\n===================================");  
  Serial.print("Current Mode: ");  
  Serial.print(isDayMode ? "DAY" : "NIGHT");  
  Serial.print(" | Threshold: ");  
  Serial.print(isDayMode ? DAY_SENSOR_THRESHOLD_CM : NIGHT_SENSOR_THRESHOLD_CM);  
  Serial.println(" cm | Yellow Delay: ");  
  Serial.print(isDayMode ? YELLOW_DELAY_DAY_MS : YELLOW_DELAY_NIGHT_MS);  
  Serial.println(" ms");  
  Serial.println("-----------------------------------");  

  // ---------------------------  
  // Step 2: Read & Process Sensors for All Traffic Lights  
  // ---------------------------  
  /* 
   * Process each light’s sensor in sequence. 
   * measureDistance() returns distance and may trigger light transitions.
  */

  // Light 1 Sensor (TRIGA1-ECHOA1)
  Serial.print("Light 1 Sensor (TRIGA1-ECHOA1) Distance: ");  
  long light1Distance = measureDistance(TRIGA1, ECHOA1, 1);  
  Serial.print(light1Distance); Serial.println(" cm");  

  // Light 2 Sensor (TRIGA2-ECHOA2)
  Serial.print("Light 2 Sensor (TRIGA2-ECHOA2) Distance: ");  
  long light2Distance = measureDistance(TRIGA2, ECHOA2, 2);  
  Serial.print(light2Distance); Serial.println(" cm");  

  // Light 3 Sensor (TRIGA3-ECHOA3)
  Serial.print("Light 3 Sensor (TRIGA3-ECHOA3) Distance: ");  
  long light3Distance = measureDistance(TRIGA3, ECHOA3, 3);  
  Serial.print(light3Distance); Serial.println(" cm");  

  // Light 4 Sensor (TRIGA4-ECHOA4)
  Serial.print("Light 4 Sensor (TRIGA4-ECHOA4) Distance: ");  
  long light4Distance = measureDistance(TRIGA4, ECHOA4, 4);  
  Serial.print(light4Distance); Serial.println(" cm");  

  // ---------------------------  
  // Step 3: Log Button States  
  // ---------------------------  
  /* 
   * Button states are logged to monitor manual override and mode toggle status.
   * digitalRead() returns HIGH (released) or LOW (pressed) for INPUT_PULLUP pins.
  */
  Serial.println("\n--- Button Status ---");  
  Serial.print("Manual Button 1 (Pin "); Serial.print(manualButtonPin1); Serial.print("): ");  
  Serial.print(digitalRead(manualButtonPin1) ? "RELEASED (HIGH)" : "PRESSED (LOW)");  
  Serial.print(" | State: "); Serial.println(manualButtonState);  

  Serial.print("Manual Button 2 (Pin "); Serial.print(manualButtonPin2); Serial.print("): ");  
  Serial.print(digitalRead(manualButtonPin2) ? "RELEASED (HIGH)" : "PRESSED (LOW)");  
  Serial.print(" | State: "); Serial.println(manualButtonState);  

  Serial.print("Mode Button (Pin "); Serial.print(modeButtonPin); Serial.print("): ");  
  Serial.print(digitalRead(modeButtonPin) ? "RELEASED (HIGH)" : "PRESSED (LOW)");  
  Serial.print(" | Current Mode: "); Serial.println(isDayMode ? "DAY" : "NIGHT");  
  Serial.println("-----------------------------------");  

  // ---------------------------  
  // Step 4: Pause for Next Loop Iteration  
  // ---------------------------  
  /* 
   * Delay to control loop frequency. 
   * DEFAULT_DELAY_MS = 1000 → loop runs ~1Hz (once per second).
   * Note: If transitions are triggered (with delay() in measureDistance/status()), 
   * total loop time will exceed 1000ms → variable update rate. Consider non-blocking delays (millis()) for stability.
  */
  Serial.print("Loop delay ("); Serial.print(DEFAULT_DELAY_MS); Serial.println(" ms)...");  
  delay(DEFAULT_DELAY_MS);  // Wait 1 second before next iteration  
}  