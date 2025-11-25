/*
*  Questo esempio permette utilizzare una scheda Arduino AVR come Decoder Accessori Dcc e di replicare il camndo degli stati sul Bus WireSusi.
*  Utilizza 29 indirizzi, corrispondenti alle F0 - F28 dei moduli Slave WireSusi.
*/

#ifndef __AVR__                                                                 // Se la piattaforma NON e' AVR -> Errore!
    #error "Esempio Compatibile solo con AVR!"                                  // Errore! Esempio compatibile solo con schede AVR!
#endif

#include  <avr/eeprom.h>                                                        // Gestione della EEPROM
#include  <NmraDcc.h>                                                           // Libreria Dcc
#include  <WireSusi.h>                                                          // Comando dei moduli sul bus Wire con protocollo SUSI
#include  "CVs.h"                                                               // Gestione CVs

#define   READ_EEPROM(x)                  eeprom_read_byte((uint8_t*)x)         // Define simbolica per 'leggere la EEEPROM'
#define   UPDATE_EEPROM(x, y)             eeprom_update_byte((uint8_t*)x, y)    // Define simbolica per 'Aggiornare il valore nella EEEPROM'

#define   DccPin                          2                                     // Pin a cui e' conesso il segnale Dcc (tramite optoisolatore)
#define   DccInterrupt                    0                                     // Interrupt corrispondente all'ingresso Dcc 
#define   AckPin                          15                                    // Pin che controlla il sistema di ACK (A1 - D15)

NmraDcc   Dcc ;                                                                 // Classe NmraDcc  
uint16_t  DccAddress = 0;                                                       // Indirizzo Dcc     

void setup() {
    Wire.begin(1);                                                              // Imposto la libreria Wire come Master del bus con l'indirizzo Master 1
    Wire.setClock(400000);                                                      // Imposto la frequenza del bus I2c a 400kHz (Fast I2c)

    DccAddress = READ_EEPROM(9);                                                // Leggo gli 8 bit piu' significativi
    DccAddress = DccAddress << 8;                                               // Traslo a sinistra gli 8 bit
    DccAddress |= READ_EEPROM(1);                                               // Aggiungo gli 8 bit meno significativi

    pinMode( AckPin, OUTPUT );                                                  // Pin controllo Ack configurato per l'utilizzo 
    Dcc.pin(DccInterrupt, DccPin, 0);                                           // Inizializzo la Libreria Dcc con i dati del Pin di input  
    Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );       // Inizializzo la libreria come Decoder Accessori e Accessorio Output (ON / OFF), resistore PullUp disattivato
}

void loop()  {                                                                  // Loop del codice                
    while(1) {                                                                  // Ciclo infinito (per non uscire dalla funzione Loop
        Dcc.process();                                                          // Decodifica i dati DCC
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*  DCC Call-Backs  */

void notifyCVAck(void) {                                                        // Funzione chiamata per rispondere con un Ack alla centrale
    digitalWrite( AckPin, HIGH );                                               // Pin ACK a 'ALTO' (Optoisolatore Attivo!)
    delay( 5 );                                                                 // Faccio passare il tempo minimo
    digitalWrite( AckPin, LOW );                                                // Pin ACK a 'BASSO' (Optoisolatore Disattivo!)
}

////////////////////////////////////////////////

uint8_t notifyCVRead (uint16_t CV) {                                            // Legge una CV salvata nella EEPROM interna o di uno Slave
    if(CV == CV_SLAVE_SELECTOR) {                                               // Se la CV e' quella per selezionare uno slave in cui leggere le CVs
        return READ_EEPROM(CV_SLAVE_SELECTOR);                                  // Restituisce il valore nella Cella EEPROM
    }
    else {                                                                      // In caso di altre CV devo capire se è richiesta una CV del Decoder o di un modulo esterno
        if(READ_EEPROM(CV_SLAVE_SELECTOR) < 2 || READ_EEPROM(CV_SLAVE_SELECTOR) > 127) {    // Fuori range indirizzi -> CV interna
            return READ_EEPROM(CV);                                             // Restituisce il valore nella Cella EEPROM
        }
        else {                                                                  // CV su un modulo esterno
            return readCVsWireSusi(READ_EEPROM(CV_SLAVE_SELECTOR), CV);         // Leggo la CV dal modulo
        }
    }
}

////////////////////////////////////////////////

uint8_t notifyCVWrite (uint16_t CV, uint8_t Value){                             // Scrive una CV salvata nella EEPROM interna o di uno Slave
    if(CV == CV_SLAVE_SELECTOR) {                                               // Se la CV e' quella per selezionare uno slave in cui leggere le CVs
        UPDATE_EEPROM(CV, Value);                                               // Aggiorno il valore
        return READ_EEPROM(CV);                                                 // Restituisco il nuovo valore
    }
    else {                                                                      // In caso di altre CV devo capire se è richiesta una CV del Decoder o di un modulo esterno
        if(READ_EEPROM(CV_SLAVE_SELECTOR) < 2 || READ_EEPROM(CV_SLAVE_SELECTOR) > 127) {    // Fuori range indirizzi -> CV interna
            switch(CV) {                                                        // Devo distinguere le CVs 'speciali'
                case 7: {                                                       // Manufacter Version -> NON SCRIVIBILE
                    return READ_EEPROM(CV);                                     // Restituisco il valore Letto
                    break;
                }
                case 8: {                                                       // Manufacter ID -> NON SCRIVIBILE
                    if(Value == 8) {                                            // Reset CVs Interne!
                        resetCVs();                                             // Eseguo il reset
                        return 8;                                               // Restituisco il valore
                    }
                    //else {}                                                   // Altri valori non ammessi
                    return READ_EEPROM(CV);                                     // Restituisco il valore letto
                    break;
                }
                default: {                                                      // Tutte le altre CVs
                  UPDATE_EEPROM(CV, Value);                                     // Aggiorno il valore
                  return READ_EEPROM(CV_SLAVE_SELECTOR);                        // Restituisco il nuovo valore
                }
            }            
        }
        else {                                                                  // CV su un modulo esterno
            return writeCVsWireSusi(READ_EEPROM(CV_SLAVE_SELECTOR), CV, Value); // Scrivo la CV sul modulo
        }
    }
}

////////////////////////////////////////////////

void notifyCVResetFactoryDefault() {                                            // Esegue il reset delle CVs interne
    resetCVs();                                                                 // Eseguo il reset
}

////////////////////////////////////////////////

void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower ) {   // Call-Back invocata quando arriva un comando accessorio, stato On / Off                  
    if( (Addr - DccAddress) < 28) {                                             // Controllo se l'indirizzo pilotato e' nel range 0 - 28 rispetto all'indirizzo di base del Decoder Dcc  
        static uint8_t functionsStatus[4] = { 0 };                              // Identifica lo stato delle funzioni da F0 a F28
        uint8_t functionsGroup;                                                 // Identifica il Gruppo Funzioni
        uint8_t functionNumber;                                                 // Identifica la posizione del bit corrispondente alla funzione
        
        if(Addr == DccAddress) {                                                // Funzione 0
            functionNumber = 4;                                                 // F0 corrisponde al bit5
            functionsGroup = 0;                                                 // F0 nel gruppo 0
        }
        else {                                                                  // Altre funzioni
            functionsGroup = (Addr-2) / 4;                                      // Identifica il Gruppo Funzioni
            functionNumber = (Addr-2) % 4;                                      // Identifica la posizione del bit corrispondente alla funzione
        }
        
        switch(functionsGroup) {                                                // In base al gruppo funzione
            case 0: {                                                           // F0 - F4  
                functionsGroup = 0;
                break; 
            }
            case 1: {                                                           // F5 - F8
                functionsGroup = 1;
                break;
            }
            case 2: {                                                           // F9 - F12 
                functionsGroup = 1;
                functionNumber += 4;   
                break;
            }
            case 3: {                                                           // F13 - F16
                functionsGroup = 2;
                break;
            }
            case 4: {                                                           // F17 - F20   
                functionsGroup = 2;
                functionNumber += 4;
                break;
            }
            case 5: {                                                           // F21 - F24
                functionsGroup = 3;
                break;
            }
            case 6: {                                                           // F25 - F28   
                functionsGroup = 3; 
                functionNumber += 4;
                break;
            }
            default: {  }
        }

        bitWrite(functionsStatus[functionsGroup], functionNumber, Direction);   // Imposto il bit della Funzione al nuovo valore

        for(uint8_t TwiAddress = 2; TwiAddress < 128; ++TwiAddress) {           // Per ogni indirizzo I2c valido
            sendCommandWireSusi(TwiAddress, (96 + functionsGroup), functionsStatus[functionsGroup]);    // Invio il comando della Funzione    
        }
            
    }
    //else {}                                                                   // In caso contrario non faccio nulla
}
