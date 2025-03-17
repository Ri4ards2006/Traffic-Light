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

#include <NewPing.h>  // Für zuverlässige Ultraschallmessung

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

// Ultraschallsensoren [Trigger, Echo]
NewPing SONAR[4] = {
  NewPing(14, 15, 300),  // Nord (max 300cm)
  NewPing(16, 17, 300),  // Süd
  NewPing(18, 19, 300),  // Ost
  NewPing(1, 0, 300)     // West
};

// Fußgängertaster
const uint8_t FUSSGAENGER_TASTER[4] = {28, 25, 27, 30};

// -------------------------
// ZEITEINSTELLUNGEN (MILLISEKUNDEN)
// -------------------------
enum ZEITEN {
  TAGES_PHASEN[] = {10000, 3000, 10000, 3000}, // Grün, Gelb, Grün, Gelb
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
  /* Drei Betriebszustände:
     1. Blinkmodus (keine Autos)
     2. Grünphase für erkannte Richtung
     3. Übergangsphase mit Gelb */
     
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
  digitalWrite(AMPELN[kreuzung][ROT], zustand == ROT ? HIGH : LOW);
  digitalWrite(AMPELN[kreuzung][GELB], zustand == GELB ? HIGH : LOW);
  digitalWrite(AMPELN[kreuzung][GRUEN], zustand == GRUEN ? HIGH : LOW);
}

void ampeln_reseten() {
  /* Setzt alle Ampeln auf Standardzustand */
  for(uint8_t i=0; i<4; i++) {
    ampeln_setzen(i, ROT);
    digitalWrite(FUSSGAENGER_LICHTER[i][ROT], HIGH);
    digitalWrite(FUSSGAENGER_LICHTER[i][GRUEN], LOW);
  }
}

// -------------------------
// PHASENVERWALTUNG
// -------------------------
void phasen_wechsel(uint16_t dauer) {
  /* Prüft ob Phasenzeit abgelaufen ist */
  if(millis() - phasen_startzeit >= dauer) {
    aktuelle_phase = (aktuelle_phase + 1) % 4;
    phasen_startzeit = millis();
    Serial.print(F("Neue Phase: "));
    Serial.println(aktuelle_phase);
  }
}

// -------------------------
// FUSSGÄNGERHANDLING
// -------------------------
void verarbeite_fussgaenger() {
  /* Bearbeitet alle Fußgängeranforderungen */
  static uint32_t letzte_aktivierung = 0;
  
  if(fussg_aktiv && (millis() - letzte_aktivierung > FUSSG_GEHZEIT + 5000)) {
    fussg_aktiv = false;
  }

  if(!fussg_aktiv && (millis() - letzte_aktivierung > 1000)) {
    for(uint8_t i=0; i<4; i++) {
      if(fussg_anforderung[i]) {
        fussgaenger_phase_starten(i);
        fussg_anforderung[i] = false;
        letzte_aktivierung = millis();
        fussg_aktiv = true;
      }
    }
  }
}

void fussgaenger_phase_starten(uint8_t richtung) {
  /* Vollständiger Fußgängerzyklus */
  // Vorbereitung
  ampeln_setzen(richtung, GELB);
  delay(2000);
  ampeln_setzen(richtung, ROT);
  
  // Grünphase
  digitalWrite(FUSSGAENGER_LICHTER[richtung][ROT], LOW);
  digitalWrite(FUSSGAENGER_LICHTER[richtung][GRUEN], HIGH);
  delay(FUSSG_GEHZEIT);
  
  // Warnblinken
  for(uint8_t i=0; i<5; i++) {
    digitalWrite(FUSSGAENGER_LICHTER[richtung][GRUEN], !digitalRead(FUSSGAENGER_LICHTER[richtung][GRUEN]));
    delay(FUSSG_BLINKZEIT);
  }
  
  // Zurücksetzen
  digitalWrite(FUSSGAENGER_LICHTER[richtung][GRUEN], LOW);
  digitalWrite(FUSSGAENGER_LICHTER[richtung][ROT], HIGH);
}

// -------------------------
// NACHTMODUS-FUNKTIONEN
// -------------------------
void sensor_update() {
  /* Aktualisiert Sensordaten mit Debouncing */
  if(millis() - letzte_sensor_aktualisierung > SENSOR_UPDATE) {
    for(uint8_t i=0; i<4; i++) {
      auto_erkannt[i] = (SONAR[i].ping_cm() > 0 && SONAR[i].ping_cm() < SENSOR_AKTIV_DISTANZ);
    }
    letzte_sensor_aktualisierung = millis();
  }
}

bool autos_erkannt() {
  for(uint8_t i=0; i<4; i++) {
    if(auto_erkannt[i]) return true;
  }
  return false;
}

uint8_t naechste_richtung_finden() {
  /* Findet nächste aktive Richtung mit Round-Robin */
  static uint8_t letzte_richtung = 0;
  
  for(uint8_t i=1; i<=4; i++) {
    uint8_t test_richtung = (letzte_richtung + i) % 4;
    if(auto_erkannt[test_richtung]) {
      letzte_richtung = test_richtung;
      return test_richtung;
    }
  }
  return 0;
}

void blinkmodus_aktivieren() {
  /* Blinkt alle gelben Lichter */
  static bool blinkzustand = false;
  static uint32_t letztes_blinken = 0;
  
  if(millis() - letztes_blinken > BLINK_INTERVALL) {
    blinkzustand = !blinkzustand;
    for(uint8_t i=0; i<4; i++) {
      digitalWrite(AMPELN[i][GELB], blinkzustand);
    }
    letztes_blinken = millis();
  }
}

void verarbeite_nachtzyklus(uint8_t richtung) {
  uint32_t vergangene_zeit = millis() - phasen_startzeit;
  
  if(vergangene_zeit < NACHT_GRUEN - NACHT_GELB) {
    ampeln_setzen(richtung, GRUEN);
  } else if(vergangene_zeit < NACHT_GRUEN) {
    ampeln_setzen(richtung, GELB);
  } else {
    ampeln_setzen(richtung, ROT);
  }
}

// -------------------------
// TASTERHANDLING
// -------------------------
void init_taster() {
  /* Initialisiert alle Taster mit Interrupts */
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(MODE_BUTTON), modus_taster_ISR, CHANGE);
  
  for(uint8_t i=0; i<4; i++) {
    pinMode(FUSSGAENGER_TASTER[i], INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(FUSSGAENGER_TASTER[i]), 
                   []() { fussgaenger_taster_ISR(i); }, FALLING);
  }
}

void modus_taster_ISR() {
  /* ISR für Modusumschaltung */
  static uint32_t gedrueckt_zeit = 0;
  
  if(digitalRead(MODE_BUTTON) == LOW) {
    gedrueckt_zeit = millis();
  } else {   
    if(millis() - gedrueckt_zeit > 3000) {
      aktueller_modus = (aktueller_modus == TAG) ? NACHT : TAG;
      Serial.print(F("Modus gewechselt auf: "));
      Serial.println(aktueller_modus == TAG ? "TAG" : "NACHT");
      ampeln_reseten();
      phasen_startzeit = millis();
    }
  }
}

void fussgaenger_taster_ISR(uint8_t taster_nr) {
  /* ISR für Fußgängertaster mit Entprellung */
  static uint32_t letzter_druck[4] = {0};
  
  if(millis() - letzter_druck[taster_nr] > ENTSPRELLZEIT) {
    fussg_anforderung[taster_nr] = true;
    Serial.print(F("Fußgängeranforderung: "));
    Serial.println(taster_nr);
    letzter_druck[taster_nr] = millis();
  }
}

// -------------------------
// DEBUGGING & STATUS
// -------------------------
void zeige_status() {
  /* Gibt regelmäßig Systemstatus aus */
  static uint32_t letztes_update = 0;
  
  if(millis() - letztes_update > 1000) {
    Serial.println(F("\n=== Systemstatus ==="));
    Serial.print(F("Modus: "));
    Serial.println(aktueller_modus == TAG ? "TAG" : "NACHT");
    
    Serial.print(F("Aktive Phase: "));
    Serial.println(aktuelle_phase);
    
    Serial.println(F("Sensorwerte:"));
    for(uint8_t i=0; i<4; i++) {
      Serial.print(i);
      Serial.print(F(": "));
      Serial.print(SONAR[i].ping_cm());
      Serial.print(F("cm "));
      Serial.println(auto_erkannt[i] ? "🟢" : "🔴");
    }
    
    letztes_update = millis();
  }
}