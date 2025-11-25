/*
*	Questo esempio permette di vedere la corretta decodifica dell'interfaccia SUSI:
*   -   Accendendo il led incorporato nella scheda quando la Funzione 0 e' attiva.
*   -   Permette la lettura/scrittura delle CVs salvate nella EEPROM del Microcontrollore
*/

#include <stdint.h>             // Libreria per i tipi "uintX_t"
#include <Rcn600.h>             // Includo la libreria per la gestione della SUSI
#include <EEPROM.h>             // Libreria per la gestione della EEPROM interna
#include <WireSusi.h>           // Libreria per la gestione dei messaggi Rcn600 Susi tramite Wire / I2c

#define MASTER_ADDRESS  0       // Indirizzo del Master del bus I2c / wire
#define SLAVE_ADDRESS   1       // Indirizzo dello Slave

Rcn600 SUSI(2, 3);              // (CLK pin, DATA pin) il pin di Clock DEVE ESSERE di tipo interrupt, il pin Data puo' essere in pin qualsiasi: compresi gli analogici

void notifySusiRawMessage(uint8_t firstByte, uint8_t secondByte) {                                                  // Funzione CallBack che viene invocata quando e' disponibile un messaggio Susi
    sendCommandWireSusi(SLAVE_ADDRESS, firstByte, secondByte);                                                      // Invio il comando Susi ricevuto allo Slave
}

void notifySusiFunc(SUSI_FN_GROUP SUSI_FuncGrp, uint8_t SUSI_FuncState) {                                           // Funzione CallBack che viene invocata quando e' decodificato un comando per le Funzioni
    switch (SUSI_FuncGrp) {                                                                                         // Scelgo a quale gruppo funzioni appartiene il comando
        case SUSI_FN_0_4: {                                                                                         // Funzioni da 0 a 4
            ((SUSI_FuncState & SUSI_FN_BIT_00) ? digitalWrite(LED_BUILTIN, HIGH) : digitalWrite(LED_BUILTIN, LOW)); // se la Funzione 0 e' attiva accendo il LED_BUILTIN
            break;
        }
        default: {}
    }
}

uint8_t notifySusiCVRead(uint16_t CV) {                                                                             // Funzione CallBack per leggere il valore di una CV memorizzata
    if(CV == 910) {                                                                                                 // Se la CV da leggere e' la 910
        return readCVsWireSusi(SLAVE_ADDRESS, CV);                                                                  // Allora comunico allo Slave I2c di Leggere il valore della CV dal proprio sistema di memorizzazione
    }
    else {
        return EEPROM.read(CV);                                                                                     // Restituisce il valore memorizzato nella EEPROM
    }
}

uint8_t notifySusiCVWrite(uint16_t CV, uint8_t Value) {                                                             // Funzione CallBack per scrivere il valore di una CV memorizzata
    if(CV == 910) {                                                                                                 // Se la CV da leggere e' la 910
        return writeCVsWireSusi(SLAVE_ADDRESS, CV, Value);                                                          // Allora comunico allo Slave I2c di Scrivere il valore della CV nel proprio sistema di memorizzazione
    }
    else {
        EEPROM.update(CV, Value);                                                                                   // Se il valore richiesto e' diverso aggiorno la EEPROM, in caso contrario non modifico per non rovinarla

        return EEPROM.read(CV);                                                                                     // Restituisco il nuovo valore della EEPROM
    }
}

void setup() {                                                                                                      // Setup del Codice
    pinMode(LED_BUILTIN, OUTPUT);                                                                                   // Imposto il pin a cui e' connesso il LED_BUILTIN come output
    Wire.begin(MASTER_ADDRESS);                                                                                     // Imposto la libreria Wire come Master del bus con l'indirizzo specificato
    Wire.setClock(400000);                                                                                          // Imposto la frequenza del bus I2c a 400kHz (Fast I2c)
    SUSI.init();                                                                                                    // Avvio la libreria Rcn600 per l'acquisizione del Segnale SUSI
}

void loop() {                                                                                                       // Loop del codice
    SUSI.process();                                                                                                 // Elaboro piu' volte possibile i dati acquisiti dalla libreria Rcn600 Susi
}
