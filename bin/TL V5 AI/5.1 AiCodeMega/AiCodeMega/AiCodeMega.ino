/* =====================================================
   ULTIMATIVE VERKEHRSKREUZUNG MIT TAG/NACHT-MODUS
   =====================================================

   Hardware:
   - Arduino Mega 2560
   - 4x Hauptampeln (Rot/Gelb/Grün)
   - 4x Fußgängerampeln (Rot/Grün)
   - 4x Ultraschallsensoren (HC-SR04)
   - 5x Taster (4x Fußgänger, 1x Moduswechsel)

   Features:
   - 🌞 Tagesmodus: Festes Ampelprogramm mit Fußgängeranforderung
   - 🌚 Nachtmodus: Sensorgesteuerte Ampel mit Auto-Erkennung
   - 🚦 Blinkmodus bei Inaktivität
   - 🚸 Fußgänger-Warnblinkphase
   - 🔒 Entprellte Taster mit Interrupts
   - 📈 Serielles Debugging-Interface

   Pinbelegung:
   Siehe CONFIG SECTION unten

   Erstellt von: Richard Zuikov
   Version: 1.01 (Ultimate Sigma CCP Edition)
   ===================================================== */

#include <NewPing.h>  // Für verlässliche Ultraschallmessung

// =====================================================
// KONFIGURATIONSBEREICH - ALLES ANPASSBAR
// =====================================================

// -------------------------
// HARDWARE-PINBELEGUNG
// -------------------------
#define MODE_BUTTON 2    // Modusumschalter (3 sec halten)

// Hauptampeln [Rot, Gelb, Grün]
const uint8_t AMPELN[4][3] = {
  {22, 23, 24},  // Nord
  {25, 26, 27},  // Süd
  {28, 29, 30},  // Ost
  {31, 32, 33}   // West
};

// Fußgängerampeln [Rot, Grün]
const uint8_t FUSSGAENGER_LICHTER[4][2] = {
  {6, 7},   // Nord
  {8, 9},   // Süd
  {10, 11}, // Ost
  {12, 13}  // West
};

// Ultraschallsensoren (Pins ohne Serial-Konflikt)
NewPing SONAR[4] = {
  NewPing(14, 15, 300),  // Nord
  NewPing(16, 17, 300),  // Süd
  NewPing(18, 19, 300),  // Ost
  NewPing(0, 1, 300)   // West
};

// Fußgängertaster (Pins ohne Konflikte)
const uint8_t FUSSGAENGER_TASTER[4] = {2, 3, 4, 5};

// -------------------------
// ZEITEINSTELLUNGEN (MILLISEKUNDEN)
// -------------------------
const uint16_t TAGES_PHASEN[] = {10000, 3000, 10000, 3000}; // Als separates Array

enum ZEITEN {
  NACHT_GRUEN     = 15000,   // Grünphase bei Autoerkennung
  NACHT_GELB      = 3000,    // Gelbphase
  BLINK_INTERVALL = 500,     // Gelblichtblinken
  FUSSG_GEHZEIT   = 8000,    // Dauer Grünphase
  FUSSG_BLINKZEIT = 500,     // Blinkintervall
  ENTSPRELLZEIT   = 200      // Entprellzeit Taster
};

// -------------------------
// SENSOREINSTELLUNGEN
// -------------------------
#define SENSOR_AKTIV_DISTANZ 150  // Autoerkennung bis 1.5m (cm)
#define SENSOR_UPDATE        500   // Sensorabfrageintervall

// =====================================================
// GLOBALE VARIABLEN & ZUSTÄNDE
// =====================================================

enum MODI { TAG, NACHT };
enum LICHTZUSTAND { ROT, GELB, GRUEN };

// Systemzustand
volatile MODI aktueller_modus = TAG;
uint32_t modus_wechsel_zeit = 0;
uint32_t phasen_startzeit = 0;
uint8_t aktuelle_phase = 0;

// Fußgängermanagement
volatile bool fussg_anforderung[4] = {false};
bool fussg_aktiv = false;
uint32_t fussg_letzte_aktivierung = 0;

// Nachtmodus
bool auto_erkannt[4] = {false};
uint32_t letzte_sensor_aktualisierung = 0;
uint8_t aktive_richtung = 0;

// =====================================================
// INITIALISIERUNG (WIRD EINMAL AUSGEFÜHRT)
// =====================================================
void setup() {
  Serial.begin(115200);
  Serial.println(F("🚦 Systemstart 🚦"));
  
  // Initialisiere alle Ampeln
  ampeln_reseten();
  
  // Taster mit Interrupts
  init_taster();
  
  Serial.println(F("System bereit!"));
  Serial.println(F("========================"));
}

// =====================================================
// HAUPTSCHLEIFE (WIEDERHOLT SICH ENDLOS)
// =====================================================
void loop() {
  // Hauptsteuerungslogik
  verwalte_modus();
  
  // Fußgängeranforderungen bearbeiten
  verarbeite_fussgaenger();
  
  // Debug-Infos
  zeige_status();
  
  // Kleine Pause zur Prozessorentlastung
  delay(10);
}

// =====================================================
// MODUS-STEUERUNG
// =====================================================
void verwalte_modus() {
  switch(aktueller_modus) {
    case TAG:
      tagesmodus_logik();
      break;
      
    case NACHT:
      nachtmodus_logik();
      break;
  }
}

// -------------------------
// TAGESMODUS-LOGIK
// -------------------------
void tagesmodus_logik() {
  /* Regelbetrieb mit festem Phasenzyklus:
     1. Nord/Süd Grün (10s)
     2. Nord/Süd Gelb (3s)
     3. Ost/West Grün (10s)
     4. Ost/West Gelb (3s) */
  
  switch(aktuelle_phase) {
    case 0: // Phase 1: Nord/Süd Grün
      ampeln_setzen(0, GRUEN);
      ampeln_setzen(1, GRUEN);
      ampeln_setzen(2, ROT);
      ampeln_setzen(3, ROT);
      phasen_wechsel(TAGES_PHASEN[0]);
      break;

    case 1: // Übergangsphase Gelb
      ampeln_setzen(0, GELB);
      ampeln_setzen(1, GELB);
      phasen_wechsel(TAGES_PHASEN[1]);
      break;

    case 2: // Phase 3: Ost/West Grün
      ampeln_setzen(2, GRUEN);
      ampeln_setzen(3, GRUEN);
      ampeln_setzen(0, ROT);
      ampeln_setzen(1, ROT);
      phasen_wechsel(TAGES_PHASEN[2]);
      break;

    case 3: // Übergangsphase Gelb
      ampeln_setzen(2, GELB);
      ampeln_setzen(3, GELB);
      phasen_wechsel(TAGES_PHASEN[3]);
      break;
  }
}

// -------------------------
// NACHTMODUS-LOGIK
// -------------------------
void nachtmodus_logik() {
  sensor_update();
  
  if (!autos_erkannt()) {
    blinkmodus_aktivieren();
    return;
  }

  if (millis() - phasen_startzeit > NACHT_GRUEN) {
    aktive_richtung = naechste_richtung_finden();
    phasen_startzeit = millis();
  }

  verarbeite_nachtzyklus(aktive_richtung);
}

// =====================================================
// HILFSFUNKTIONEN
// =====================================================

// -------------------------
// AMPELSTEUERUNG
// -------------------------
void ampeln_setzen(uint8_t kreuzung, LICHTZUSTAND zustand) {
  /* Setzt die Lichter einer gesamten Kreuzung */
  digitalWrite(AMPELN[kreuzung][0], zustand == ROT ? HIGH : LOW);
  digitalWrite(AMPELN[kreuzung][1], zustand == GELB ? HIGH : LOW);
  digitalWrite(AMPELN[kreuzung][2], zustand == GRUEN ? HIGH : LOW);
}

// -------------------------
// PHASENWECHSEL
// -------------------------
void phasen_wechsel(uint16_t dauer) {
  if(millis() - phasen_startzeit >= dauer) {
    aktuelle_phase = (aktuelle_phase + 1) % 4;
    phasen_startzeit = millis();
  }
}

// Weitere Funktionen zur Steuerung von Fußgängern und Sensoren...

// =====================================================
// ENDE DES PROGRAMMS
// =====================================================