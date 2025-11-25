/*
*	Questo esempio permette di vedere la corretta Acquisizione dei messaggi SUSI tramite Bus I2c:
*   -   Stampa a video i Byte ricevuti (Esclusi quelli per la gestione CVs)
*   -   Permette la lettura/scrittura delle CVs salvate nella EEPROM del Microcontrollore
*/

#define SLAVE_ADDRESS 1   // Indirizzo dello Slave sul bus Wire

#include <stdint.h>       // Libreria per i tipi "uintX_t"
#include <WireSusi.h>     // Includo la libreria per la gestione del Messaggi SUSI sul Bus I2c
#include <EEPROM.h>       // Libreria per la gestione della EEPROM interna

void notifySusiRawMessage(uint8_t firstByte, uint8_t secondByte) {                                                  // Funzione CallBack invocata quanto un messaggio e' in attesa di decodifica
    Serial.print("notifySusiRawMessage : ");

    Serial.print(firstByte, BIN);                                                                                   // Stampo i Byte ricevuti in formato Binario
    Serial.print(" - "); 
    Serial.print(secondByte, BIN); 

    Serial.print(" ( ");                                                                                            // Stampo il valore Decimale dei Byte ricevuti
    Serial.print(firstByte, DEC); 
    Serial.print(" - "); 
    Serial.print(secondByte, DEC); 
    Serial.println(" )");
}

void onReceiveWireSusiExternalHanlde(uint8_t nBytes) {                                                              // Handle chiamato quando si ricevono piu' di 3 Bytes: comandi esterni
    for(uint8_t i = 0; i < nBytes; ++i) {                                                                           // Fino a quando ci sono dati nel Buffer I2c
        Serial.print(Wire.read());                                                                                  // Li stampo sulla Seriale
    }
}

uint8_t notifySusiCVRead(uint16_t CV) {                                                                             // Funzione CallBack per leggere il valore di una CV memorizzata
    return EEPROM.read(CV);                                                                                         // Restituisce il valore memorizzato nella EEPROM
}

uint8_t notifySusiCVWrite(uint16_t CV, uint8_t Value) {                                                             // Funzione CallBack per scrivere il valore di una CV memorizzata
    EEPROM.update(CV, Value);                                                                                       // Se il valore richiesto e' diverso aggiorno la EEPROM, in caso contrario non modifico per non rovinarla

    return EEPROM.read(CV);                                                                                         // Restituisco il nuovo valore della EEPROM
}

void setup() {                                                                                                      // Setup del Codice
    Serial.begin(500000);                                                                                           // Avvio la comunicazione Seriale
    while (!Serial) {}                                                                                              // Attendo che la comunicazione seriale sia disponibile

    Serial.println("SUSI Over I2c Print Print Raw Messages:");                                                      // Messaggio di Avvio

    Wire.begin(SLAVE_ADDRESS);                                                                                      // Avvio il bus I2c con Indirizzo Specificato

    /* Imposto Gli Handler per gli eventi del Bus */
    Wire.onReceive(onReceiveWireSusi);
    Wire.onRequest(onRequestWireSusi);
}

void loop() {                                                                                                       // Loop del codice
    processWireSusi();                                                                                              // Elaboro piu' volte possibile i dati ricevuti via Bus I2c
}
