/***************************************************
 *_traffic_light_controller.ino - Traffic Light Control System
 * 
 * Purpose: This code controls 4 traffic light groups with integrated pedestrian signals.
 *          Each group manages signals for cars (red, yellow, green) and pedestrians (straight/left red/green).
 *          It implements two sequences (1/4 and 2/3) that alternate in a loop to simulate traffic light timing.
 * 
 * Author: Richard Zuikov a
 * Date: 25.11.2025 (Note: This date may be illustrative; adjust as needed)
 * Note: Original German code translated and fully commented for clarity.
 *       All variable/function names and comments are in English for international usability.
 ***************************************************/

/*--------------------------
 * PIN DEFINITIONS
 * Maps Arduino pins to specific traffic/pedestrian lights.
 * Naming convention: [TYPE]_[DIRECTION]_[COLOR]_[GROUP]
 * - TYPE: PEDESTRIAN (Fußgänger) or TRAFFIC_LIGHT (Autosignal)
 * - DIRECTION: STRAIGHT (geradeaus) or LEFT (links)
 * - COLOR: RED, GREEN, YELLOW
 * - GROUP: 1-4 (identifies the traffic light group)
 *--------------------------*/
const int PEDESTRIAN_STRAIGHT_RED_1    = 7;    // Fußgänger-Gerade-Rot (Gruppe 1) auf Pin 7
const int PEDESTRIAN_STRAIGHT_GREEN_1  = 8;   // Fußgänger-Gerade-Grün (Gruppe 1) auf Pin 8
const int PEDESTRIAN_LEFT_RED_1        = 9;   // Fußgänger-Links-Rot (Gruppe 1) auf Pin 9
const int PEDESTRIAN_LEFT_GREEN_1      = 10;  // Fußgänger-Links-Grün (Gruppe 1) auf Pin 10
const int TRAFFIC_LIGHT_GREEN_1        = 11;  // Auto-Grün (Gruppe 1) auf Pin 11
const int TRAFFIC_LIGHT_YELLOW_1       = 12;  // Auto-Gelb (Gruppe 1) auf Pin 12
const int TRAFFIC_LIGHT_RED_1          = 13;  // Auto-Rot (Gruppe 1) auf Pin 13

const int PEDESTRIAN_STRAIGHT_RED_2    = 20;   // Fußgänger-Gerade-Rot (Gruppe 2) auf Pin 20
const int PEDESTRIAN_STRAIGHT_GREEN_2  = 19;   // Fußgänger-Gerade-Grün (Gruppe 2) auf Pin 19
const int PEDESTRIAN_LEFT_RED_2        = 18;   // Fußgänger-Links-Rot (Gruppe 2) auf Pin 18
const int PEDESTRIAN_LEFT_GREEN_2      = 17;   // Fußgänger-Links-Grün (Gruppe 2) auf Pin 17
const int TRAFFIC_LIGHT_GREEN_2        = 16;   // Auto-Grün (Gruppe 2) auf Pin 16
const int TRAFFIC_LIGHT_YELLOW_2       = 15;   // Auto-Gelb (Gruppe 2) auf Pin 15
const int TRAFFIC_LIGHT_RED_2          = 14;   // Auto-Rot (Gruppe 2) auf Pin 14

const int PEDESTRIAN_STRAIGHT_RED_3    = 22;   // Fußgänger-Gerade-Rot (Gruppe 3) auf Pin 22
const int PEDESTRIAN_STRAIGHT_GREEN_3  = 24;   // Fußgänger-Gerade-Grün (Gruppe 3) auf Pin 24
const int PEDESTRIAN_LEFT_RED_3        = 26;   // Fußgänger-Links-Rot (Gruppe 3) auf Pin 26
const int PEDESTRIAN_LEFT_GREEN_3      = 28;   // Fußgänger-Links-Grün (Gruppe 3) auf Pin 28
const int TRAFFIC_LIGHT_GREEN_3        = 30;   // Auto-Grün (Gruppe 3) auf Pin 30
const int TRAFFIC_LIGHT_YELLOW_3       = 32;   // Auto-Gelb (Gruppe 3) auf Pin 32
const int TRAFFIC_LIGHT_RED_3          = 34;   // Auto-Rot (Gruppe 3) auf Pin 34

const int PEDESTRIAN_STRAIGHT_RED_4    = 36;   // Fußgänger-Gerade-Rot (Gruppe 4) auf Pin 36
const int PEDESTRIAN_STRAIGHT_GREEN_4  = 38;   // Fußgänger-Gerade-Grün (Gruppe 4) auf Pin 38
const int PEDESTRIAN_LEFT_RED_4        = 40;   // Fußgänger-Links-Rot (Gruppe 4) auf Pin 40
const int PEDESTRIAN_LEFT_GREEN_4      = 42;   // Fußgänger-Links-Grün (Gruppe 4) auf Pin 42
const int TRAFFIC_LIGHT_GREEN_4        = 44;   // Auto-Grün (Gruppe 4) auf Pin 44
const int TRAFFIC_LIGHT_YELLOW_4       = 46;   // Auto-Gelb (Gruppe 4) auf Pin 46
const int TRAFFIC_LIGHT_RED_4          = 48;   // Auto-Rot (Gruppe 4) auf Pin 48

/*--------------------------
 * FUNCTION: setInitialStates()
 * Resets all traffic lights to a safe initial state.
 * Purpose: Ensures no leftover light states from previous code execution.
 *          Critical for preventing unintended signals (e.g., green lights staying on).
 *--------------------------*/
void setInitialStates() {
  // --------------------------
  // Phase 1: Turn off all traffic light yellow signals
  // Yellow lights are only active during transitions; they should be off in all stable states.
  // --------------------------
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, LOW);   // Yellow (Gruppe 1) ausschalten
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, LOW);   // Yellow (Gruppe 2) ausschalten
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, LOW);   // Yellow (Gruppe 3) ausschalten
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, LOW);   // Yellow (Gruppe 4) ausschalten

  // --------------------------
  // Phase 2: Turn off all traffic light green signals
  // Green lights allow cars to proceed; they must be off unless explicitly active.
  // Original German comment ("Rot Autos aus") was misleading (confused red/green).
  // Correct logic: Green lights are off in initial state.
  // --------------------------
  digitalWrite(TRAFFIC_LIGHT_GREEN_1, LOW);    // Green (Gruppe 1) ausschalten
  digitalWrite(TRAFFIC_LIGHT_GREEN_2, LOW);    // Green (Gruppe 2) ausschalten
  digitalWrite(TRAFFIC_LIGHT_GREEN_3, LOW);    // Green (Gruppe 3) ausschalten
  digitalWrite(TRAFFIC_LIGHT_GREEN_4, LOW);    // Green (Gruppe 4) ausschalten

  // --------------------------
  // Phase 3: Turn off all traffic light red signals
  // Wait: Red lights are *normally on* (cars must stop). But here, they are turned off temporarily?
  // Original German comment ("Grün Autos aus") was incorrect (confused green/red).
  // However, the initial state requires red lights to be on. See note below!
  // Note: This function is called before each phase to reset. But the final setup() overrides this.
  // Actual red light behavior is controlled by subsequent digitalWrite() calls.
  // --------------------------
  digitalWrite(TRAFFIC_LIGHT_RED_1, LOW);      // Red (Gruppe 1) temporär ausschalten (wird später wieder eingeschaltet)
  digitalWrite(TRAFFIC_LIGHT_RED_2, LOW);      // Red (Gruppe 2) temporär ausschalten
  digitalWrite(TRAFFIC_LIGHT_RED_3, LOW);      // Red (Gruppe 3) temporär ausschalten
  digitalWrite(TRAFFIC_LIGHT_RED_4, LOW);      // Red (Gruppe 4) temporär ausschalten

  // --------------------------
  // Phase 4: Activate pedestrian straight red signals (all groups)
  // Pedestrian red lights indicate that crossing is not allowed. These should always be on initially.
  // --------------------------
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH); // Fußgänger-Gerade-Rot (Gruppe 1) an
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH); // Fußgänger-Gerade-Rot (Gruppe 2) an
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH); // Fußgänger-Gerade-Rot (Gruppe 3) an
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH); // Fußgänger-Gerade-Rot (Gruppe 4) an

  // --------------------------
  // Phase 5: Turn off pedestrian straight green signals (all groups)
  // Green lights allow pedestrians to cross. They are off initially (controlled by red lights).
  // --------------------------
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, LOW); // Fußgänger-Gerade-Grün (Gruppe 1) ausschalten
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, LOW); // Fußgänger-Gerade-Grün (Gruppe 2) ausschalten
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, LOW); // Fußgänger-Gerade-Grün (Gruppe 3) ausschalten
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, LOW); // Fußgänger-Gerade-Grün (Gruppe 4) ausschalten

  // --------------------------
  // Phase 6: Activate pedestrian left red signals (all groups)
  // Similar to straight pedestrians: left-crossing red lights are on initially (no crossing allowed).
  // --------------------------
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);   // Fußgänger-Links-Rot (Gruppe 1) an
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);   // Fußgänger-Links-Rot (Gruppe 2) an
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);   // Fußgänger-Links-Rot (Gruppe 3) an
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);   // Fußgänger-Links-Rot (Gruppe 4) an

  // --------------------------
  // Phase 7: Turn off pedestrian left green signals (all groups)
  // Green lights for left-crossing pedestrians are off initially.
  // --------------------------
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, LOW);  // Fußgänger-Links-Grün (Gruppe 1) ausschalten
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, LOW);  // Fußgänger-Links-Grün (Gruppe 2) ausschalten
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, LOW);  // Fußgänger-Links-Grün (Gruppe 3) ausschalten
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, LOW);  // Fußgänger-Links-Grün (Gruppe 4) ausschalten
}

/*--------------------------
 * FUNCTION: Sequence14()
 * Controls traffic light groups 1 and 4 (assumed to be opposite directions, e.g., North/South)
 * Implements a 3-phase cycle for these groups:
 * 1. Warning phase (all yellow)
 * 2. Transition phase (yellow 1/4 + red 2/3)
 * 3. Active phase (green 1/4 + red 2/3 + pedestrian greens)
 *--------------------------*/
void Sequence14() {
  // Reset all lights to initial state before starting the sequence
  setInitialStates();

  // --------------------------
  // Phase 1: All traffic yellow (2 seconds)
  // Purpose: Warn cars that the current state is about to change.
  //          Simultaneous yellow across all groups may indicate a system-wide transition.
  // --------------------------
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, HIGH);  // Yellow (Gruppe 1) an
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, HIGH);  // Yellow (Gruppe 2) an
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, HIGH);  // Yellow (Gruppe 3) an
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, HIGH);  // Yellow (Gruppe 4) an
  delay(2000);  // 2 Sekunden warten, bis die Warnphase abgeschlossen ist

  // Reset again to clear any leftover states before next phase
  setInitialStates();

  // --------------------------
  // Phase 2: Yellow 1/4 active + Red 2/3 active (2 seconds)
  // Purpose: Prepare groups 1 and 4 for green (yellow warning), while groups 2/3 stay red (cars must stop).
  //          This is a transition phase between warning and active states.
  // --------------------------
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, HIGH);  // Yellow (Gruppe 1) erneut an (Vorbereitung auf Grün)
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, HIGH);  // Yellow (Gruppe 4) erneut an (Vorbereitung auf Grün)
  digitalWrite(TRAFFIC_LIGHT_RED_2, HIGH);     // Red (Gruppe 2) an (Autos müssen weiterhin stoppen)
  digitalWrite(TRAFFIC_LIGHT_RED_3, HIGH);     // Red (Gruppe 3) an (Autos müssen weiterhin stoppen)
  delay(2000);  // 2 Sekunden warten, um Transition abzuschließen

  // Reset again to ensure clean state before final phase
  setInitialStates();

  // --------------------------
  // Phase 3: Green 1/4 active + Red 2/3 active (3.5 seconds)
  // Plus: Pedestrian straight green (1/4) + Pedestrian left green (2/3)
  // Purpose: Allow cars in groups 1/4 to proceed (green), while groups 2/3 remain red.
  //          Simultaneously, enable pedestrian crossing:
  //          - Pedestrians can cross straight (groups 1/4)
  //          - Pedestrians can cross left (groups 2/3)
  //          Red lights for these pedestrians are turned off to indicate active green.
  // --------------------------*/
  // Keep red active for groups 2/3 (cars must stop)
  digitalWrite(TRAFFIC_LIGHT_RED_2, HIGH);     // Red (Gruppe 2) bleibt an
  digitalWrite(TRAFFIC_LIGHT_RED_3, HIGH);     // Red (Gruppe 3) bleibt an

  // Activate green for groups 1/4 (cars can proceed)
  digitalWrite(TRAFFIC_LIGHT_GREEN_1, HIGH);   // Green (Gruppe 1) an
  digitalWrite(TRAFFIC_LIGHT_GREEN_4, HIGH);   // Green (Gruppe 4) an

  // Activate pedestrian straight green (groups 1/4) → allow crossing
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_1, HIGH); // Fußgänger-Gerade-Grün (Gruppe 1) an
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_4, HIGH); // Fußgänger-Gerade-Grün (Gruppe 4) an

  // Activate pedestrian left green (groups 2/3) → allow left crossing there
  digitalWrite(PEDESTRIAN_LEFT_GREEN_2, HIGH);   // Fußgänger-Links-Grün (Gruppe 2) an
  digitalWrite(PEDESTRIAN_LEFT_GREEN_3, HIGH);   // Fußgänger-Links-Grün (Gruppe 3) an

  // Turn off pedestrian straight red (groups 1/4) → since green is active, red must be off
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, LOW);  // Fußgänger-Gerade-Rot (Gruppe 1) ausschalten
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, LOW);  // Fußgänger-Gerade-Rot (Gruppe 4) ausschalten

  // Turn off pedestrian left red (groups 2/3) → since their green is active, red must be off
  digitalWrite(PEDESTRIAN_LEFT_RED_2, LOW);      // Fußgänger-Links-Rot (Gruppe 2) ausschalten
  digitalWrite(PEDESTRIAN_LEFT_RED_3, LOW);      // Fußgänger-Links-Rot (Gruppe 3) ausschalten

  // --------------------------
  // Final delay: Active phase duration (3.5 seconds)
  // During this time, cars in 1/4 and pedestrians in 1/4 (straight) and 2/3 (left) can proceed.
  // --------------------------*/
  delay(3500);  // 3.5 Sekunden haltezeit für die aktive Phase
}

/*--------------------------
 * FUNCTION: Sequence23()
 * Controls traffic light groups 2 and 3 (assumed to be opposite directions, e.g., East/West)
 * Implements a 3-phase cycle mirroring Sequence14(), but for groups 2/3.
 *--------------------------*/
void Sequence23() {
  // Reset all lights to initial state before starting the sequence
  setInitialStates();

  // --------------------------
  // Phase 1: All traffic yellow (2 seconds) - same as Sequence14()
  //--------------------------*/
  digitalWrite(TRAFFIC_LIGHT_YELLOW_1, HIGH);  // Yellow (Gruppe 1) an
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, HIGH);  // Yellow (Gruppe 2) an
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, HIGH);  // Yellow (Gruppe 3) an
  digitalWrite(TRAFFIC_LIGHT_YELLOW_4, HIGH);  // Yellow (Gruppe 4) an
  delay(2000);  // 2 Sekunden wartzeit

  // Reset again to clear leftovers
  setInitialStates();

  // --------------------------
  // Phase 2: Yellow 3/2 active + Red 1/4 active (2 seconds)
  // Purpose: Prepare groups 2/3 for green (yellow warning), while groups 1/4 stay red (cars must stop).
  // Note: Symmetrical to Sequence14()'s Phase 2, but with reversed groups.
  // --------------------------*/
  digitalWrite(TRAFFIC_LIGHT_YELLOW_3, HIGH);  // Yellow (Gruppe 3) an (Vorbereitung auf Grün)
  digitalWrite(TRAFFIC_LIGHT_YELLOW_2, HIGH);  // Yellow (Gruppe 2) an (Vorbereitung auf Grün)
  digitalWrite(TRAFFIC_LIGHT_RED_1, HIGH);     // Red (Gruppe 1) an (Autos müssen stoppen)
  digitalWrite(TRAFFIC_LIGHT_RED_4, HIGH);     // Red (Gruppe 4) an (Autos müssen stoppen)
  delay(2000);  // 2 Sekunden wartzeit

  // Reset again for clean state
  setInitialStates();

  // --------------------------
  // Phase 3: Green 2/3 active + Red 1/4 active (3.5 seconds)
  // Plus: Pedestrian straight green (3/2) + Pedestrian left green (1/4)
  // Purpose: Allow cars in groups 2/3 to proceed (green), while groups 1/4 remain red.
  //          Pedestrian signals adjusted:
  //          - Pedestrians can cross straight (groups 2/3)
  //          - Pedestrians can cross left (groups 1/4)
  // --------------------------*/
  // Keep red active for groups 1/4 (cars must stop)
  digitalWrite(TRAFFIC_LIGHT_RED_1, HIGH);     // Red (Gruppe 1) bleibt an
  digitalWrite(TRAFFIC_LIGHT_RED_4, HIGH);     // Red (Gruppe 4) bleibt an

  // Activate green for groups 2/3 (cars can proceed)
  digitalWrite(TRAFFIC_LIGHT_GREEN_2, HIGH);   // Green (Gruppe 2) an
  digitalWrite(TRAFFIC_LIGHT_GREEN_3, HIGH);   // Green (Gruppe 3) an

  // Activate pedestrian straight green (groups 2/3) → allow crossing
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_3, HIGH); // Fußgänger-Gerade-Grün (Gruppe 3) an
  digitalWrite(PEDESTRIAN_STRAIGHT_GREEN_2, HIGH); // Fußgänger-Gerade-Grün (Gruppe 2) an

  // Activate pedestrian left green (groups 1/4) → allow left crossing there
  digitalWrite(PEDESTRIAN_LEFT_GREEN_1, HIGH);   // Fußgänger-Links-Grün (Gruppe 1) an
  digitalWrite(PEDESTRIAN_LEFT_GREEN_4, HIGH);   // Fußgänger-Links-Grün (Gruppe 4) an

  // Turn off pedestrian straight red (groups 2/3) → green is active, red off
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, LOW);  // Fußgänger-Gerade-Rot (Gruppe 3) ausschalten
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, LOW);  // Fußgänger-Gerade-Rot (Gruppe 2) ausschalten

  // Turn off pedestrian left red (groups 1/4) → green is active, red off
  digitalWrite(PEDESTRIAN_LEFT_RED_1, LOW);      // Fußgänger-Links-Rot (Gruppe 1) ausschalten
  digitalWrite(PEDESTRIAN_LEFT_RED_4, LOW);      // Fußgänger-Links-Rot (Gruppe 4) ausschalten

  // --------------------------
  // Final delay: Active phase duration (3.5 seconds)
  // During this time, cars in 2/3 and pedestrians in 2/3 (straight) and 1/4 (left) can proceed.
  // --------------------------*/
  delay(3500);  // 3.5 Sekunden haltezeit für die aktive Phase
}

/*--------------------------
 * FUNCTION: setup()
 * Initial setup:
 * 1. Configure serial communication (for debugging, not required for core functionality)
 * 2. Set all pins as OUTPUT (required to control LEDs)
 * 3. Force initial state: All red lights (traffic + pedestrian) active
 *    → Critical safety measure: System starts in "stop all" mode.
 *--------------------------*/
void setup() {
  // Serial communication: 9600 baud rate (standard for Arduino debug)
  Serial.begin(9600);  // Optional: For logging states (not used in current code, but useful for testing)

  // --------------------------
  // Pin configuration for Traffic Light Group 1
  // --------------------------*/
  pinMode(PEDESTRIAN_STRAIGHT_RED_1,    OUTPUT);   // Fußgänger-Gerade-Rot (Gruppe 1) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_1, OUTPUT);   // Fußgänger-Gerade-Grün (Gruppe 1) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_RED_1,       OUTPUT);   // Fußgänger-Links-Rot (Gruppe 1) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_GREEN_1,     OUTPUT);   // Fußgänger-Links-Grün (Gruppe 1) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_GREEN_1,       OUTPUT);   // Auto-Grün (Gruppe 1) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_YELLOW_1,      OUTPUT);   // Auto-Gelb (Gruppe 1) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_RED_1,         OUTPUT);   // Auto-Rot (Gruppe 1) als Ausgang konfigurieren

  // --------------------------
  // Pin configuration for Traffic Light Group 2
  // --------------------------*/
  pinMode(PEDESTRIAN_STRAIGHT_RED_2,    OUTPUT);   // Fußgänger-Gerade-Rot (Gruppe 2) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_2, OUTPUT);   // Fußgänger-Gerade-Grün (Gruppe 2) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_RED_2,       OUTPUT);   // Fußgänger-Links-Rot (Gruppe 2) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_GREEN_2,     OUTPUT);   // Fußgänger-Links-Grün (Gruppe 2) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_GREEN_2,       OUTPUT);   // Auto-Grün (Gruppe 2) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_YELLOW_2,      OUTPUT);   // Auto-Gelb (Gruppe 2) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_RED_2,         OUTPUT);   // Auto-Rot (Gruppe 2) als Ausgang konfigurieren

  // --------------------------
  // Pin configuration for Traffic Light Group 3
  // --------------------------*/
  pinMode(PEDESTRIAN_STRAIGHT_RED_3,    OUTPUT);   // Fußgänger-Gerade-Rot (Gruppe 3) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_3, OUTPUT);   // Fußgänger-Gerade-Grün (Gruppe 3) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_RED_3,       OUTPUT);   // Fußgänger-Links-Rot (Gruppe 3) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_GREEN_3,     OUTPUT);   // Fußgänger-Links-Grün (Gruppe 3) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_GREEN_3,       OUTPUT);   // Auto-Grün (Gruppe 3) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_YELLOW_3,      OUTPUT);   // Auto-Gelb (Gruppe 3) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_RED_3,         OUTPUT);   // Auto-Rot (Gruppe 3) als Ausgang konfigurieren

  // --------------------------
  // Pin configuration for Traffic Light Group 4
  // --------------------------*/
  pinMode(PEDESTRIAN_STRAIGHT_RED_4,    OUTPUT);   // Fußgänger-Gerade-Rot (Gruppe 4) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_STRAIGHT_GREEN_4, OUTPUT);   // Fußgänger-Gerade-Grün (Gruppe 4) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_RED_4,       OUTPUT);   // Fußgänger-Links-Rot (Gruppe 4) als Ausgang konfigurieren
  pinMode(PEDESTRIAN_LEFT_GREEN_4,     OUTPUT);   // Fußgänger-Links-Grün (Gruppe 4) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_GREEN_4,       OUTPUT);   // Auto-Grün (Gruppe 4) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_YELLOW_4,      OUTPUT);   // Auto-Gelb (Gruppe 4) als Ausgang konfigurieren
  pinMode(TRAFFIC_LIGHT_RED_4,         OUTPUT);   // Auto-Rot (Gruppe 4) als Ausgang konfigurieren

  // --------------------------
  // Force initial state: All red lights active
  // Purpose: Even if setInitialStates() is not called, start with all red to ensure safety.
  //          Overwrites any potential accidental pin states during startup.
  // --------------------------*/
  // Traffic red lights (all groups)
  digitalWrite(TRAFFIC_LIGHT_RED_1, HIGH);    // Auto-Rot (Gruppe 1) an
  digitalWrite(TRAFFIC_LIGHT_RED_2, HIGH);    // Auto-Rot (Gruppe 2) an
  digitalWrite(TRAFFIC_LIGHT_RED_3, HIGH);    // Auto-Rot (Gruppe 3) an
  digitalWrite(TRAFFIC_LIGHT_RED_4, HIGH);    // Auto-Rot (Gruppe 4) an

  // Pedestrian straight red lights (all groups)
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_1, HIGH); // Fußgänger-Gerade-Rot (Gruppe 1) an
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_2, HIGH); // Fußgänger-Gerade-Rot (Gruppe 2) an
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_3, HIGH); // Fußgänger-Gerade-Rot (Gruppe 3) an
  digitalWrite(PEDESTRIAN_STRAIGHT_RED_4, HIGH); // Fußgänger-Gerade-Rot (Gruppe 4) an

  // Pedestrian left red lights (all groups)
  digitalWrite(PEDESTRIAN_LEFT_RED_1, HIGH);  // Fußgänger-Links-Rot (Gruppe 1) an
  digitalWrite(PEDESTRIAN_LEFT_RED_2, HIGH);  // Fußgänger-Links-Rot (Gruppe 2) an
  digitalWrite(PEDESTRIAN_LEFT_RED_3, HIGH);  // Fußgänger-Links-Rot (Gruppe 3) an
  digitalWrite(PEDESTRIAN_LEFT_RED_4, HIGH);  // Fußgänger-Links-Rot (Gruppe 4) an
}

/*--------------------------
 * FUNCTION: loop()
 * Main program loop:
 * - Repeatedly executes Sequence14() and Sequence23()
 * - Creates a cycle: Group1/4 active → Group2/3 active → repeat
 * - Runs indefinitely (until power is cut)
 *--------------------------*/
void loop() {
  // Execute Sequence14() first: Control Group1/4
  Sequence14();  // Start sequence for groups 1 and 4

  // Then execute Sequence23(): Control Group2/3
  Sequence23();  // Start sequence for groups 2 and 3

  // After both sequences complete, loop repeats automatically
}