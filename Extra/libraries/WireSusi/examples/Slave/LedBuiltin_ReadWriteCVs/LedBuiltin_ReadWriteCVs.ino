// Esempio che Accende/Spegne il LED incorporato in base allo stato delle F0 ricevuti, permette di Leggere/Scrivere CVs nella EEPROM del micro

#include <Wire.h>                                                                                                     // Includo la libreria per la gestione del bus Wire / I2c
#include <WireSusi.h>                                                                                                 // Libreria per la gestione dei messaggi Rcn600 Susi tramite Wire / I2c
#include <EEPROM.h>                                                                                                   // Libreria per la gestione della EEPROM interna al micro

#define SLAVE_ADDRESS   1                                                                                             // Indirizzo dello Slave sul bus I2c / wire

void setup() {
  Wire.begin(SLAVE_ADDRESS);                                                                                          // Avvio la libreria Wire come Slave sul Bus I2c con indirizzo specificato
  
  pinMode(LED_BUILTIN, OUTPUT);                                                                                       // Imposto il led incorporato come FeedBack visivo

  Wire.onReceive(onReceiveWireSusi);                                                                                  // Imposto l' Handle per la Ricezione dei Dati sul Bus Wire
  Wire.onRequest(onRequestWireSusi);                                                                                  // Imposto l' Handle per la Richiesta di Dati sul Bus Wire
}

void loop() {
  processWireSusi();                                                                                                  // Processo piu' volte possibili i messaggi ricevuti
}

uint8_t notifySusiCVRead(uint16_t CV) {                                                                               // Funzione CallBack per quando si vuole leggere una CV
    return EEPROM.read(CV);                                                                                           // Restituisco il valore letto
}

uint8_t notifySusiCVWrite(uint16_t CV, uint8_t Value) {                                                               // Funzione CallBack per quando si vuole scrivere una CV
    EEPROM.update(CV, Value);                                                                                         // Se diverso, aggiorno la EEPROM con il nuovo valore
    return EEPROM.read(CV);                                                                                           // Restituisco il valore letto
}

void notifySusiFunc(SUSI_FN_GROUP SUSI_FuncGrp, uint8_t SUSI_FuncState) {                                             // Funzione CallBack che viene invocata quando e' decodificato un comando per le Funzioni
    switch (SUSI_FuncGrp) {                                                                                           // Scelgo a quale gruppo funzioni appartiene il comando
        case SUSI_FN_0_4: {                                                                                           // Funzioni da 0 a 4
            ((SUSI_FuncState & SUSI_FN_BIT_00) ? digitalWrite(LED_BUILTIN, HIGH) : digitalWrite(LED_BUILTIN, LOW));   // se la Funzione 0 e' attiva accendo il LED_BUILTIN
            break;
        }
        default: {}                                                                                                   // Per gli altri comandi non esegui azioni
    }
}