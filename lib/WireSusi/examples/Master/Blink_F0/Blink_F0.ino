// Esempio che Attiva e Disattiva la F0, e lo comunica ad uno Slave

#include <Wire.h>                                                   // Includo la libreria per la gestione del bus Wire / I2c
#include <WireSusi.h>                                               // Libreria per la gestione dei messaggi Rcn600 Susi tramite Wire / I2c

#define MASTER_ADDRESS  0                                           // Indirizzo del Master del bus I2c / wire
#define SLAVE_ADDRESS   1                                           // Indirizzo dello Slave

void setup() {
    Wire.begin(MASTER_ADDRESS);                                     // Imposto la libreria Wire come Master del bus con l'indirizzo specificato
    Wire.setClock(400000);                                          // Imposto la frequenza del bus I2c a 400kHz (Fast I2c)

    pinMode(LED_BUILTIN, OUTPUT);                                   // Rendo il led incorporato atto a fornire un FeedBack visivo
}

void loop() {
    sendCommandWireSusi(SLAVE_ADDRESS, 96, SUSI_FN_BIT_00);         // Spedisco il Gruppo Funzioni 1 (96, da F0 a F4) con attivo il bit della F0
    digitalWrite(LED_BUILTIN, HIGH);                                // Accendo il LED per indiciare che la F0 e' Attiva
    delay(1000);                                                    // Attendo 1s (1000mS)
    sendCommandWireSusi(SLAVE_ADDRESS, 96, 0);                      // Spengo tutto il Gruppo Funzioni 1 
    digitalWrite(LED_BUILTIN, LOW);                                 // Spengo il LED per indiciare che la F0 e' Disattiva
    delay(1000);                                                    // Attendo 1s (1000mS)
}