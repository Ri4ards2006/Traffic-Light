/* LIB_VERSION: 0.1.6 */

#include "WireSusi.h"                                                                                   // Inclusione del Header

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* TIPI DI DATI E VARIABILI GLOBALI PRIVATE */
#define SUSI_MESSAGE_DIMENSION              (uint8_t)2                                                  // Dimensione Messaggi SUSI su I2c
#define SUSI_BUFFER_LENGTH                  5                                                           // lunghezza buffer dove sono contenuti i messaggi

typedef struct message{                                                                                 // Dato che rappresenta un Messaggio SUSI
    uint8_t Byte[2];                                                                                    // I 2 Byte che compongono un Messaggio RCN600

    struct message* nextMessage;                                                                        // Puntatore al prossimo messaggio ricevuto dal Master
} Rcn600Message;

const Rcn600Message*                        FREE_MESSAGE = (Rcn600Message*)(~0);                        // Valore simbolico "opposto" a NULL
#define FREE_MESSAGE_SLOT                   (Rcn600Message*)FREE_MESSAGE                                // Valore simbolico per indicare che uno Slot e' libero

Rcn600Message*                              _BufferPointer = NULL;                                      // Puntatore per scorrere il buffer dei messaggi ricevuti
Rcn600Message                               _Buffer[SUSI_BUFFER_LENGTH];                                // Buffer dove salvare i messaggi ricevuti in attesa di decodifica
#define	CLEAR_BUFFER                        for(uint8_t i = 0; i < SUSI_BUFFER_LENGTH; ++i) {	_Buffer[i].nextMessage = FREE_MESSAGE_SLOT;	}       // Pulisce il Buffer dei Messaggi

/* Manipolazione CVs */
#define ERROR_CV_OPERATION                  129                                                         // Valore Simbolico che rappresenta un Errore nella Gestione delle CVs
#define CVs_MESSAGE_DIMENSION               (uint8_t)3                                                  // Dimensione Messaggio Manipolazione CVs
#define	WRITE_CV_BIT                        0b1000000000000000                                          // Identifica il bit 15, se e' 1 allora si vuole scrivere una CV

typedef struct {                                                                                        // Manipolazione CVs
    uint16_t    cvAddress;                                                                              // Inirizzo CV & bit di Lettura/Scrittura (bit 15) -> massimo indirizzo CV: 32768
    uint8_t     cvValue;                                                                                // Valore da scrivere/leggere
} CVs_Message;

CVs_Message _cvMessage;                                                                                 // Variabile privata per la gestione delle CVs

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* Prototipi */

Rcn600Message*  searchFreeMessage(void);                                                                // Cerca uno slot dove salvare un messaggio SUSI
void            setNextMessage(Rcn600Message* nextMessage);                                             // Mette in coda di elaborazione uno slot dove e' stato salvato un messaggio
static int      ConvertTwosComplementByteToInteger(char rawValue);                                      // converte una variabile a 8bit in complemento a due in un intero

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*	MASTER	*/

int8_t sendCommandWireSusi(uint8_t I2C_Address, uint8_t FirstByte, uint8_t SecondByte) {                // Invio Comando RCN600 ad uno Slave
    uint8_t error;                                                                                      // Variabile dove memorizzo se la comunicazione e' andata a buon fine
    Wire.beginTransmission(I2C_Address);                                                                // Inizio la trasmissione verso lo Slave interessato
    Wire.write(FirstByte);                                                                              // Spedisco il primo Byte ( Comando )
    Wire.write(SecondByte);                                                                             // Spedisco il secondo Byte ( Argomento )
    error = Wire.endTransmission();                                                                     // Termino la transmissione

    if (error == 0) {                                                                                   // Controllo che la Trasmissione sia stata completata con successo
        return 0;                                                                                       // Trasmissione completata correttamente
    }
    else {                                                                                              // Se ci sono stati problemi nella comunicazione
        return -1;                                                                                      // Restituisco ERRORE
    }
}

int16_t readCVsWireSusi(uint8_t I2C_Address, uint16_t cvAddress) {                                      // Lettura CV su uno Slave
    uint8_t error;                                                                                      // Risultato del metodo Wire.endTransmission
    interrupts();                                                                                       // Abilito gli Interrupt per la Gestione I2c
    Wire.beginTransmission(I2C_Address);                                                                // Inizio la trasmissione verso lo Slave interessato
    Wire.write((uint8_t)(cvAddress >> 8));                                                              // Invio gli 8 bit PIU' significativi dell'indirizzo
    Wire.write((uint8_t)(cvAddress & 0xFF));                                                            // Invio gli 8 bit MENO significativi dell'Indirizzo
    Wire.write((uint8_t)ERROR_CV_OPERATION);                                                            // Invio il valore Errato per verificare che la lettura sia avvenuta correttamente
    error = Wire.endTransmission();                                                                     // Termino la transmissione

    if (error != 0) {                                                                                   // Se la comunicazione e' andata a buon fine error = 0; altrimenti errore
        return -1;                                                                                      // Restituisco l'errore
    }

#ifdef __AVR__                                                                                          // Piattaforma AVR -> Funzione 'AVR'
    _delay_us(100);                                                                                     // Attendo che lo slave abbia tempo di elaborare il dato (100uS)
#else                                                                                                   // ALTRE piattaforme supportate da Arduino IDE -> Funzione 'Arduino'
    delayMicroseconds(100);                                                                             // Attendo che lo slave abbia tempo di elaborare il dato (100uS)
#endif

    Wire.requestFrom(I2C_Address, CVs_MESSAGE_DIMENSION);                                               // Richiedo la i dati di confronto

    _cvMessage.cvAddress = (uint8_t)Wire.read();                                                        // Leggo gli 8 bit PIU' significativi
    _cvMessage.cvAddress = _cvMessage.cvAddress << 8;                                                   // Sposto gli 8 bit
    _cvMessage.cvAddress += (uint8_t)Wire.read();                                                       // Leggo gli 8 bit MENO significativi
    _cvMessage.cvValue = (uint8_t)Wire.read();                                                          // Leggo il valore della CV

    if (_cvMessage.cvAddress == cvAddress) {                                                            // Controllo che la Comunicazione sia avvenuta Correttamente
        return _cvMessage.cvValue;                                                                      // Restituisco il valore della CV
    }
    return -1;                                                                                          // Se arrivo qui la comunicazione NON E' andata a buon fine -> ERRORE
}

int16_t writeCVsWireSusi(uint8_t I2C_Address, uint16_t cvAddress, uint8_t cvValue) {                    // Scrittura CV su uno Slave
    uint8_t error;                                                                                      // Risultato del metodo Wire.endTransmission
    interrupts();                                                                                       // Abilito gli Interrupt per la Gestione I2c
    Wire.beginTransmission(I2C_Address);                                                                // Inizio la trasmissione verso lo Slave interessato
    Wire.write((uint8_t)((uint16_t)(cvAddress | WRITE_CV_BIT) >> 8));                                   // Invio gli 8 bit PIU' significativi dell'indirizzo + il bit 15 ( bit indicante la scrittura )
    Wire.write((uint8_t)(cvAddress & 0xFF));                                                            // Invio gli 8 bit MENO significativi dell'Indirizzo
    Wire.write((uint8_t)cvValue);                                                                       // Invio il nuovo valore della CV
    error = Wire.endTransmission();                                                                     // Termino la transmissione

    if (error != 0) {                                                                                   // Se la comunicazione e' andata a buon fine error = 0; altrimenti errore
        return -1;                                                                                      // Restituisco l'errore
    }

#ifdef __AVR__                                                                                          // Piattaforma AVR -> Funzione 'AVR'
    _delay_ms(5);                                                                                       // Attendo che lo slave abbia tempo di elaborare il dato (5mS)
#else                                                                                                   // ALTRE piattaforme supportate da Arduino IDE -> Funzione 'Arduino'
    delay(5);                                                                                           // Attendo che lo slave abbia tempo di elaborare il dato (5mS)
#endif

    Wire.requestFrom(I2C_Address, CVs_MESSAGE_DIMENSION);                                               // Richiedo la struct di confronto

    _cvMessage.cvAddress = (uint8_t)Wire.read();                                                        // Leggo gli 8 bit PIU' significativi
    _cvMessage.cvAddress = _cvMessage.cvAddress << 8;                                                   // Sposto gli 8 bit
    _cvMessage.cvAddress += (uint8_t)Wire.read();                                                       // Leggo gli 8 bit MENO significativi
    _cvMessage.cvValue = (uint8_t)Wire.read();                                                          // Leggo il valore della CV

    _cvMessage.cvAddress &= ~(WRITE_CV_BIT);                                                            // Rimuovo il bit 15 (bit indicante la Scrittura) nel caso Non lo avesse gia' fatto lo Slave

    if (_cvMessage.cvAddress == cvAddress) {                                                            // Controllo che la Comunicazione sia avvenuta Correttamente
        if (_cvMessage.cvValue == cvValue) {                                                            // Se il nuovo valore della CV corrisponde a quello richiesto
            return _cvMessage.cvValue;                                                                  // Restituisco il valore letto della CV
        }
        else {                                                                                          // Se il nuovo valore CV e' diverso da quello richiesto
            return -2;                                                                                  // Errore Nuovo valore CV
        }
    }
    return -1;                                                                                          // Se arrivo qui la comunicazione NON E' andata a buon fine -> ERRORE
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /* Gestione Buffer */

Rcn600Message* searchFreeMessage(void) {                                                                // Cerco uno Slot libero dove salvare i dati ricevuti
    for (uint8_t i = 0; i < SUSI_BUFFER_LENGTH; ++i) {                                                  // Scorro tutto il Buffer
        if (_Buffer[i].nextMessage == FREE_MESSAGE_SLOT) {                                              // Se uno slot risulta libero
            return &_Buffer[i];                                                                         // Restituisco l'indirizzo di quello Slot
        }
        //else {}                                                                                       // In caso contrario continuo a cercare
    }

    return NULL;                                                                                        // Se non ho trovato uno Slot libero restituisco NULL: Buffer pieno
}

void setNextMessage(Rcn600Message* nextMessage) {                                                       // Inserisco uno Slot nella coda di decodifica
    Rcn600Message* p = _BufferPointer;                                                                  // Variabile che mi serve per scorrere la coda di decodifica senza interferire con essa

    if (p != NULL) {                                                                                    // C'e' almeno un messaggio in coda 
        while (p->nextMessage != NULL) {                                                                // Se c'e' un messagio dopo quello corrente
            p = p->nextMessage;                                                                         // Il messagio successivo diventa quello corrente
        }
                                                                                                        // Arrivato alla coda
        p->nextMessage = nextMessage;                                                                   // Imposto il messaggio passato come successivo a quello attuale
    }
    else {                                                                                              // In caso di nessun messaggio in coda
        _BufferPointer = nextMessage;                                                                   // Questo sara' il prossimo messaggio in coda di decodifica
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*	I2C EVENTS	*/

void onReceiveWireSusi(int nBytes) {                                                                    // Handle che viene invocato alla ricezione di dati sul bus I2c
    static uint8_t initNeeded = 1;                                                                      // Alla prima invocazione devo "pulire" il buffer di acquisizione
    if (initNeeded) {                                                                                   // Controllo se e' la prima invocazione dell'evento
        CLEAR_BUFFER;                                                                                   // Pulisco il buffer
        initNeeded = 0;                                                                                 // Inizializzazione non piu' necessaria
    }

    switch (nBytes) {                                                                                   // In base a quanti Bytes ho ricevuto dal Master eseguo un azione
        case SUSI_MESSAGE_DIMENSION: {                                                                  // 2 Bytes: comando SUSI normale
            Rcn600Message *p = searchFreeMessage();                                                     // Cerco uno slot libero dove salvare il messaggio

            if (p != NULL) {                                                                            // Slot libero trovato
                p->nextMessage = NULL;                                                                  // Lo imposto come ultimo della Coda

                p->Byte[0] = Wire.read();                                                               // Acquisisco il primo Byte ( Comando )
                p->Byte[1] = Wire.read();                                                               // Acquisisco il secondo Byte ( Argomento )

                setNextMessage(p);                                                                      // Metto il messaggio acquisito in coda di decodifica
            }
            else {                                                                                      // Nessuno slot libero trovato
                while (Wire.available() > 0) {                                                          // Pulisco il Buffer 
                    Wire.read();
                }
            }	
            break;
        }
        case CVs_MESSAGE_DIMENSION: {                                                                   // 3 Bytes: comando SUSI CV -> Processare subito!		
            _cvMessage.cvAddress = (uint8_t)Wire.read();                                                // Leggo gli 8 bit piu' significativi
            _cvMessage.cvAddress = _cvMessage.cvAddress << 8;                                           // Sposto gli 8 bit
            _cvMessage.cvAddress += (uint8_t)Wire.read();                                               // Leggo gli 8 bit MENO significativi
            _cvMessage.cvValue = (uint8_t)Wire.read();                                                  // Leggo il valore della CV

            if (_cvMessage.cvAddress & WRITE_CV_BIT) {                                                  // Controllo se si vuole scrivere la CV -> bit 15 = 1
                if (notifySusiCVWrite) {                                                                // Controllo che sia presente un sistema di memorizzazione CVs
                    _cvMessage.cvAddress &= ~(WRITE_CV_BIT);                                            // Rimuovo il bit 15 (bit indicante la Scrittura)
                    if (_cvMessage.cvAddress == 8 || _cvMessage.cvAddress == 900 || _cvMessage.cvAddress == 940 || _cvMessage.cvAddress == 980) {       // Controllo se non si vuole fare un reset
                        if (_cvMessage.cvValue == 8) {                                                                                                  // Si vuole fare un reset
                            if (notifyCVResetFactoryDefault) {                                                                                          // Controllo se' disponibile la funzione per fare il reset
                                notifyCVResetFactoryDefault();                                                                                          // Eseguo il reset delle CVs
                            }
                            else {
                                _cvMessage.cvValue = ERROR_CV_OPERATION;                                                                                // Utilizzzo un valore simbolico
                            }
                        }
                        else {
                            _cvMessage.cvValue = ERROR_CV_OPERATION;                                                                                    // Utilizzzo un valore simbolico
                        }
                    }
                    else {
                        _cvMessage.cvValue = notifySusiCVWrite(_cvMessage.cvAddress, _cvMessage.cvValue);                                               // Scrivo la CV
                    }
                }
                else {                                                                                  // Se non e' presente un sistema di memorizzazione CVs
                    _cvMessage.cvValue = ERROR_CV_OPERATION;                                            // Utilizzzo un valore simbolico
                }
            }
            else {                                                                                      // bit 15 = 0 -> Lettura CVs
                if (notifySusiCVRead) {                                                                 // Controllo che sia presente un sistema di memorizzazione CVs
                    _cvMessage.cvValue = notifySusiCVRead(_cvMessage.cvAddress);                        // Leggo il valore della CV
                }
                else {                                                                                  // Se non e' presente un sistema di memorizzazione CVs
                    _cvMessage.cvValue = ERROR_CV_OPERATION;                                            // Utilizzzo un valore simbolico
                }
            }
            break;
        }
        default: {                                                                                      // Dimensioni non gestite, controllo se e' presente un Handle esterno	
            if (onReceiveWireSusiExternalHandle) {                                                      // Controllo se l'utente ha definito un Handle esterno
                onReceiveWireSusiExternalHandle(nBytes);                                                // Chiamo l'Handle esterno
            }

            while (Wire.available() > 0) {                                                              // Se sono avanzati dati nel Buffer
                Wire.read();                                                                            // Li elimino
            }
        }
    }
}

void onRequestWireSusi(void) {                                                                          // Richiesti Byte dal Master: operazione sulle CVs
    Wire.write((uint8_t)(_cvMessage.cvAddress >> 8));                                                   // Invio gli 8 bit piu' significativi dell'indirizzo
    Wire.write((uint8_t)(_cvMessage.cvAddress & 0xFF));                                                 // Invio gli 8 bit MENO significativi dell'Indirizzo
    Wire.write((uint8_t)_cvMessage.cvValue);                                                            // Invio il valore Errato per verificare che la lettura sia avvenuta correttamente
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /*	SLAVE	*/

/* Il seguente metodo e' stato copiato da qua: https://stackoverflow.com/questions/48567214/how-to-convert-a-byte-in-twos-complement-form-to-its-integer-value-c-sharp */
static int ConvertTwosComplementByteToInteger(char rawValue) {
    // If a positive value, return it
    if ((rawValue & 0x80) == 0) {
        return rawValue;
    }

    // Otherwise perform the 2's complement math on the value
    return (byte)(~(rawValue - 0x01)) * -1;
}

int8_t processWireSusi(void) {
    uint8_t processNextMessage = 1;	// Indica se devo processare piu' messaggi

    while (processNextMessage) {
        processNextMessage = 0;

        if (_BufferPointer != NULL) {		//controllo che siano stati ricevuti dei messaggi

            if (notifySusiRawMessage) {
                notifySusiRawMessage(_BufferPointer->Byte[0], _BufferPointer->Byte[1]);
            }

            /* Devo controllare il valore del primo Byte */
            switch (_BufferPointer->Byte[0]) {
                case 96: {
                    /* "Funktionsgruppe 1" : 0110-0000 (0x60 = 96) 0 0 0 F0 - F4 F3 F2 F1 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_0_4, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 97: {
                    /* "Funktionsgruppe 2" : 0110-0001 (0x61 = 97) F12 F11 F10 F9 - F8 F7 F6 F5 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_5_12, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 98: {
                    /* "Funktionsgruppe 3" : 0110-0010 (0x62 = 98) F20 F19 F18 F17 - F16 F15 F14 F13 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_13_20, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 99: {
                    /* "Funktionsgruppe 4" : 0110-0011 (0x63 = 99) F28 F27 F26 F25 - F24 F23 F22 F21 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_21_28, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 100: {
                    /* "Funktionsgruppe 5" : 0110-0100 (0x64 = 100) F36 F35 F34 F33 - F32 F31 F30 F29 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_29_36, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 101: {
                    /* "Funktionsgruppe 6" : 0110-0101 (0x65 = 101) F44 F43 F42 F41 - F40 F39 F38 F37 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_37_44, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 102: {
                    /* "Funktionsgruppe 7" : 0110-0110 (0x66 = 102) F52 F51 F50 F49 - F48 F47 F46 F45 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_45_52, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 103: {
                    /* "Funktionsgruppe 8" : 0110-0111 (0x67 = 103) F60 F59 F58 F57 - F56 F55 F54 F53 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_53_60, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 104: {
                    /* "Funktionsgruppe 9" : 0110-1000 (0x68 = 104) F68 F67 F66 F65 - F64 F63 F62 F61 */
                    if (notifySusiFunc) {
                        notifySusiFunc(SUSI_FN_61_68, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 109: {
                    /* "Binärzustände kurze Form" :  0110 - 1101 (0x6D = 109) D L6 L5 L4 - L3 L2 L1 L0
                    *
                    *	D = 0 bedeutet Funktion L ausgeschaltet, D = 1 eingeschaltet
                    *	L = Funktionsnummer 1 ... 127
                    *	L = 0 (Broadcast) schaltet alle Funktionen 1 bis 127 aus (D = 0) oder an (D = 1)
                    *
                    *	D = 0 significa funzione L disattivata, D = 1 attivata
                    *	L = numero funzione 1 ... 127
                    *	L = 0 (trasmissione) disattiva (D = 0) o attiva tutte le funzioni da 1 a 127 (D = 1)
                    */

                    static uint8_t functionNumber, funcState;

                    funcState = _BufferPointer->Byte[1] & 0x80;					// leggo il valore dello stato 'D' ()

                    functionNumber = _BufferPointer->Byte[1] & 0b01111111;		// elimino il bit piu' significativo (bit7)

                    if (notifySusiBinaryState) {
                        if (functionNumber == 0) {
                            // Comanda tutte le funzioni
                            uint8_t i;
                            if (funcState == 0) {	// disattivo tutte le funzioni
                                for (i = 1; i < 128; ++i) {
                                    notifySusiBinaryState(i, 0);
                                }
                            }
                            else {				// attivo tutte le funzioni 
                                for (i = 1; i < 128; ++i) {
                                    notifySusiBinaryState(i, 1);
                                }
                            }
                        }
                        else {
                            // Comanda una singola funzione
                            notifySusiBinaryState(functionNumber, funcState);
                        }
                    }
                    break;
                }
                case 110: {	// && 111
                    /*	"Binärzustände lange Form low Byte" : 0110-1110 (0x6E = 110) D L6 L5 L4 - L3 L2 L1 L0
                    *
                    *	Befehl wird immer paarweise vor dem Binärzustand lange Form high Byte gesendet.
                    *	Folgen die beiden Befehle nicht direkt aufeinander, so sind sie zu ignorieren.
                    *	D = 0 bedeutet Binärzustand L ausgeschaltet, D = 1 "eingeschaltet"
                    *	L = niederwertige Bits der Binärzustandsnummer 1 ... 32767
                    *
                    *	Il comando viene sempre inviato in coppie prima che lo stato binario formi lunghi byte alti.
                    *	Se i due comandi non seguono direttamente, devono essere ignorati.
                    *	D = 0 significa stato binario L spento, D = 1 "acceso"
                    *	L = bit di basso valore dello stato binario numero 1 ... 32767
                    *
                    *
                    *	"Binärzustände lange Form high Byte" : 0110-1111 (0x6F = 111) H7 H6 H5 H4 - H3 H2 H1 H0
                    *
                    *	Befehl wird immer paarweise nach dem Binärzustand lange Form low
                    *	Byte gesendet. Folgen die beiden Befehle nicht direkt aufeinander, so sind sie zu ignorieren.
                    *	Erst dieser Befehl führt zur Ausführung des Gesamtbefehls.
                    *	H = höherwertigen Bits der Binärzustandsnummer high 1 ... 32767
                    *	H und L = 0 (Broadcast) schaltet alle 32767 verfügbaren Binärzustände
                    *	aus (D = 0) oder an (D = 1)
                    *
                    *	Il comando viene sempre inviato in coppie dopo che lo stato binario forma lunghi byte bassi. Se i due comandi non si sus seguono direttamente, devono essere ignorati.
                    *	Solo dopo questo comando eseguira' l'intero comando.
                    *	H = bit di qualita' superiore dello stato binario numero alto 1 ... 32767
                    */

                    Rcn600Message* next = _BufferPointer->nextMessage;
                    if (next != NULL) {
                        if (next->Byte[0] == 111) {					// Posso eseguire il comando solo se ho ricevuto sia il Byte piu' significativo che quello meno significativo
                            if (notifySusiBinaryState) {			// Controllo se e' presente il metodo per gestire il comando
                                static uint16_t Command;
                                static uint8_t State;

                                Command = next->Byte[1];				// memorizzo i bit "piu' significativ"
                                Command = Command << 7;					// sposto i bit 7 posti a 'sinistra'
                                Command |= _BufferPointer->Byte[1];				// aggiungo i 7 bit "meno significativi"

                                State = bitRead(_BufferPointer->Byte[1], 7);

                                notifySusiBinaryState(Command, State);
                            }
                        }
                    }
                    else {
                        // Il messaggio successivo NECESSARIO non e' ancora stato ricevuto, esco dal process senza modifiche
                        return 1;
                    }
                    break;
                }
                case 64: {
                    /*	"Direktbefehl 1" (2-Byte): 0100-0000 (0x40 = 64) X8 X7 X6 X5 - X4 X3 X2 X1
                    *
                    *	Die Direktbefehle dienen zur direkten Ansteuerung von Ausgängen und
                    *	anderen Funktionen nach der Anwendung der Funktionstabelle im Master.
                    *	Ein Bit = 1 bedeutet der entsprechende Ausgang ist eingeschaltet.
                    *
                    *	I comandi diretti vengono utilizzati per il controllo diretto delle uscite e
                    *	altre funzioni dopo aver utilizzato la tabella delle funzioni nel master.
                    *	Un bit = 1 significa che l'uscita corrispondente è attivata.
                    */
                    if (notifySusiAux) {
                        notifySusiAux(SUSI_AUX_1_8, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 65: {
                    /*	"Direktbefehl 2" : 0100-0001 (0x41 = 65) X16 X15 X14 X13 - X12 X11 X10 X9 */
                    if (notifySusiAux) {
                        notifySusiAux(SUSI_AUX_9_16, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 66: {
                    /*	"Direktbefehl 3" : 0100-0010 (0x42 = 66) X24 X23 X22 X21 - X20 X19 X18 X17 */
                    if (notifySusiAux) {
                        notifySusiAux(SUSI_AUX_17_24, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 67: {
                    /*	"Direktbefehl 4" : 0100-0011 (0x43 = 67) X32 X31 X30 X29 - X28 X27 X26 X25 */
                    if (notifySusiAux) {
                        notifySusiAux(SUSI_AUX_25_32, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 33: {
                    /*	"Trigger-Puls" : 0010-0001 (0x21 = 33) 0 0 0 0 - 0 0 0 1
                    *
                    *	Der Befehl dient zur Synchronisation eines Dampfstoßes. Er wird je
                    *	Dampfstoß einmal gesendet. Die Bits 1 bis 7 sind für zukünftige
                    *	Anwendungen reserviert.
                    *
                    *
                    *	Il comando viene utilizzato per sincronizzare una scarica di vapore.
                    *	Lo fara' mai scoppio di vapore inviato una volta.
                    *	I bit da 1 a 7 sono per uso futuro, Applicazioni riservate.
                    */
                    if (notifySusiTriggerPulse) {
                        notifySusiTriggerPulse(_BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 35: {
                    /*	"Strom" : 0010-0011 (0x23 = 35) S7 S6 S5 S4 - S3 S2 S1 S0
                    *
                    *	Vom Motor verbrauchter Strom. Der Wert hat einen Bereich von -128 bis
                    *	127, wird im 2er-Komplement übertragen und wird über eine
                    *	herstellerspezifische CV im Lokdecoder kalibriert. Negative Werte
                    *	bedeuten ein Rückspeisen wie es bei modernen E-Loks möglich ist.
                    *
                    *	Elettricita' consumata dal motore. Il valore ha un intervallo da -128 a
                    *	127, viene trasmesso in complemento a 2 e viene trasmesso tramite a
                    *	CV specifico del costruttore calibrato nel decoder della locomotiva. Valori negativi
                    *	significa un feed back come e' possibile con le moderne locomotive elettriche.
                    */
                    if (notifySusiMotorCurrent) {
                        notifySusiMotorCurrent(ConvertTwosComplementByteToInteger(_BufferPointer->Byte[1]));
                    }
                    break;
                }
                case 36: {	/* DEPRECATO DALLA REVISIONE 6 / 12 / 2020 */
                    /*	" "Ist" Lok-Fahrstufe " : 0010-0100 (0x24 = 36) R G6 G5 G4 - G3 G2 G1 G0
                    *
                    *	Die Fahrstufe und Richtung entsprechen dem realen Zustand am Motor.
                    *	Der Wert G ist als 0...127 auf die im Modell eingestellte Vmax normiert zu
                    *	übertragen. G = 0 bedeutet Lok steht, G = 1 ... 127 ist die normierte
                    *	Geschwindigkeit R = Fahrtrichtung mit R = 0 für rückwärts und R = 1
                    *	für vorwärts.
                    *	Dieser und der folgende Befehl werden nicht für neue Implementierungen
                    *	empfohlen. Slaves sollten nach Möglichkeit die Befehle 0x50 bis 0x52
                    *	auswerten. Master, die aus Gründen der Kompatibilität zu existierenden
                    *	Produkten abweichende und/oder unterschiedliche Umsetzungen bei den
                    *	Befehlen 0x24 und 0x25 verwenden, sind normkonform.
                    *
                    *	Il livello di velocita' e la direzione corrispondono allo stato reale del motore.
                    *	Il valore G e' normalizzato come 0 ... 127 alla Vmax impostata nel modello
                    *	Trasferimento. G = 0 significa che la locomotiva e' ferma, G = 1 ... 127 e' quella standardizzata
                    *	Velocita' R = senso di marcia con R = 0 all'indietro e R = 1 per avanti.
                    *	Questo e il seguente comando non vengono utilizzati per le nuove implementazioni
                    *	consigliato. Se possibile, gli slave dovrebbero utilizzare i comandi da 0x50 a 0x52
                    *	valutare. Master, che per motivi di compatibilita' con gli esistenti
                    *	Prodotti che deviano e / o implementazioni differenti nel
                    *	L'uso dei comandi 0x24 e 0x25 e' conforme allo standard.
                    */
                    if (notifySusiRealSpeed) {
                        if (bitRead(_BufferPointer->Byte[1], 7) == 1) {
                            notifySusiRealSpeed(_BufferPointer->Byte[1] - 128, SUSI_DIR_FWD);
                        }
                        else {
                            notifySusiRealSpeed(_BufferPointer->Byte[1], SUSI_DIR_REV);
                        }
                    }
                    break;
                }
                case 37: {	/* DEPRECATO DALLA REVISIONE 6 / 12 / 2020 */
                    /*	" "Soll" Lok-Fahrstufe " : 0010-0101 (0x25 = 37) R G6 G5 G4 - G3 G2 G1 G0
                    *
                    *	Empfangene Fahrstufe des "Masters" auf 127 Fahrstufen normiert.
                    *	G = 0 bedeutet Lok hält, G = 1 ... 127 ist die normierte Geschwindigkeit
                    *	R = Fahrtrichtung mit R = 0 für rückwärts und R = 1 für vorwärts
                    *
                    *	Passo di velocita' ricevuto del "master" normalizzato a 127 passi di velocita'.
                    *	G = 0 significa che la locomotiva si ferma, G = 1 ... 127 e' la velocita' normalizzata
                    *	R = senso di marcia con R = 0 indietro e R = 1 avanti
                    */
                    if (notifySusiRequestSpeed) {
                        if (bitRead(_BufferPointer->Byte[1], 7) == 1) {
                            notifySusiRequestSpeed(_BufferPointer->Byte[1] - 128, SUSI_DIR_FWD);
                        }
                        else {
                            notifySusiRequestSpeed(_BufferPointer->Byte[1], SUSI_DIR_REV);
                        }
                    }
                    break;
                }
                case 38: {
                    /*	"Lastausregelung" : 0010-0110 (0x26 = 38) P7 P6 P5 P4 - P3 P2 P1 P0
                    *
                    *	Die Erfassung des Lastzustandes kann über Motorspannung, -strom oder -
                    *	leistung erfolgen. 0 = keine Last, 127 = maximale Last. Es sind auch
                    *	negative Werte möglich, die im 2er-Komplement übertragen werden.
                    *	Diese bedeuten weniger Last als Fahren in der Ebene.
                    *
                    *	Lo stato del carico puo' essere registrato tramite tensione del motore, corrente o prestazione.
                    *	0 = nessun carico, 127 = carico massimo.
                    *	Ci sono anche possibili valori negativi, che vengono trasferiti nel complemento di 2.
                    *	Significa meno carico rispetto alla guida in piano.
                    */

                    if (notifySusiMotorLoad) {
                        notifySusiMotorLoad(ConvertTwosComplementByteToInteger(_BufferPointer->Byte[1]));
                    }
                    break;
                }
                case 80: {
                    /*	" "Ist" Geschwindigkeit " : 0101-0000 (0x50 = 80) R G6 G5 G4 - G3 G2 G1 G0
                    *
                    *	Die Geschwindigkeit und Richtung entsprechen dem realen Zustand am
                    *	Motor. Dieses ist ein Regelwert in Bezug auf die "Soll"- Geschwindigkeit,
                    *	d.h. dass nach Nachführung der Geschwindigkeitsrampe Ist und Soll gleich
                    *	sein sollen (ausgeregelter Zustand). Der Wert G ist als 0...127 auf die im
                    *	Modell eingestellte Vmax normiert zu übertragen. G = 0 bedeutet Lok steht, G
                    *	= 1 ... 127 ist die normiert Geschwindigkeit R = Fahrtrichtung mit R = 0
                    *	für rückwärts und R = 1 für vorwärts
                    *
                    *	La velocita' e la direzione corrispondono alla situazione reale del Motore.
                    *	Questo e' un valore di controllo in relazione alla velocita' "target",
                    *	ovvero che dopo aver tracciato la rampa di velocita', i valori effettivi e target sono gli stessi, dovrebbe essere (stato stabile).
                    *	Il valore G deve essere trasferito come 0 ... 127 standardizzato alla Vmax impostata nel modello.
                    *	G = 0 significa che la locomotiva e' ferma, G = 1 ... 127 e' la velocita' normalizzata, R = senso di marcia
                    *	con R = 0 per indietro e R = 1 per avanti
                    */
                    if (notifySusiRealSpeed) {
                        if (bitRead(_BufferPointer->Byte[1], 7) == 1) {
                            notifySusiRealSpeed(_BufferPointer->Byte[1] - 128, SUSI_DIR_FWD);
                        }
                        else {
                            notifySusiRealSpeed(_BufferPointer->Byte[1], SUSI_DIR_REV);
                        }
                    }
                    break;
                }
                case 81: {
                    /*	" "Soll" Geschwindigkeit " : 0101-0001 (0x51 = 81) R G6 G5 G4 - G3 G2 G1 G0
                    *
                    *	Interne Fahrstufe des "Masters" entsprechend der über die Kennlinie (CVs
                    *	67 bis 94, CVs 2, 6 und 5) und sonstiger CVs, die die Geschwindigkeit des
                    *	Fahrzeugs bestimmen, umgerechnete empfangene Fahrstufe auf 127
                    *	Fahrstufen normalisiert. D.h. der höchste aufgrund der CVs 94 und/oder
                    *	CV5 bzw. sonstiger entsprechender CVs erreichbare Wert wird auf 127
                    *	normiert. CVs für Beschleunigung und Bremsen wie die CVs 3, 4, 23 und
                    *	24 gehen in die Berechnung nicht ein. G = 0 bedeutet Lok hält, G = 1 ...
                    *	127 ist die Geschwindigkeit R = Fahrtrichtung mit R = 0 für rückwärts und
                    *	R = 1 für vorwärts.
                    *	Da die Decoder unterschiedliche Verfahren zur Bestimmung der
                    *	Höchstgeschwindigkeit verwenden, kann es hier leicht unterschiedliche
                    *	Implementationen geben. Wichtig ist vor allem, dass sich die Befehle für
                    *	Ist und Soll-Geschwindigkeit gleich verhalten.
                    *
                    *	Livello di guida interno del "Master" in base al controllo della caratteristica (CV)
                    *	da 67 a 94, CV 2, 6 e 5) e altri CV che determinano la velocita' della normalizzazione del veicolo convertito hanno ricevuto un livello di guida a 127 livelli di guida.
                    *	Cio' significa che il valore piu' alto ottenibile grazie ai CV 94 e/o CV5 o ad altri CV corrispondenti e' normalizzato a 127.
                    *	I CV per l'accelerazione e la frenata come i CV 3, 4, 23 e 24 non vengono utilizzati nel calcolo.
                    *	G = 0 significa appigli per locomotive, G = 1 ... 127 e' la velocita' R = direzione di marcia con R = 0 per l'indietro e R = 1 per l'avanti.
                    *	Poiche' i decodificatori utilizzano metodi diversi per determinare la velocita' massima, potrebbero esserci implementazioni leggermente diverse.
                    *	Soprattutto, e' importante che i comandi per la velocita' effettiva e di destinazione si comportino allo stesso modo.
                    */
                    if (notifySusiRequestSpeed) {
                        if (bitRead(_BufferPointer->Byte[1], 7) == 1) {
                            notifySusiRequestSpeed(_BufferPointer->Byte[1] - 128, SUSI_DIR_FWD);
                        }
                        else {
                            notifySusiRequestSpeed(_BufferPointer->Byte[1], SUSI_DIR_REV);
                        }
                    }
                    break;
                }
                case 82: {	/* NON IMPLEMENTATO */
                    /*	"DCC-Fahrstufe" : 0101-0010 (0x52 = 82) R G6 G5 G4 - G3 G2 G1 G0
                    *
                    *	Dieser Wert ist nur ggf. von 14 oder 28 Fahrstufen auf 127 Fahrstufen normiert.
                    *	Es findet keine Anpassung durch irgendwelches CVs statt.
                    *
                    *	Questo valore e' standardizzato solo da 14 o 28 passaggi di guida a 127 passaggi di guida.
                    *	Non vi e' alcuna regolazione da parte di CV.
                    */
                    break;
                }
                case 40: {
                    /*	"Analogfunktionsgruppe 1" : 0010-1000 (0x28 = 40) A7 A6 A5 A4 - A3 A2 A1 A0
                    *
                    *	Die acht Befehle dieser Gruppe erlauben die Übertragung von acht
                    *	verschiedenen Analogwerten im Digitalbetrieb.
                    *
                    *	Gli otto comandi di questo gruppo consentono la trasmissione di otto diversi valori analogici in modalita' digitale.
                    */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_0_7, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 41: {
                    /*	"Analogfunktionsgruppe 2" : 0010-1001 (0x29 = 41) A15 A14 A13 A12 - A11 A10 A9 A8 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_8_15, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 42: {
                    /*	"Analogfunktionsgruppe 3" : 0010-1010 (0x2A = 42) A23 A22 A21 A20 - A19 A18 A17 A16 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_16_23, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 43: {
                    /*	"Analogfunktionsgruppe 4" : 0010-1011 (0x2B = 43) A31 A30 A29 A28 - A27 A26 A25 A24 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_24_31, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 44: {
                    /*	"Analogfunktionsgruppe 5" : 0010-1100 (0x2C = 44) A39 A38 A37 A36 - A35 A34 A33 A32 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_32_39, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 45: {
                    /*	"Analogfunktionsgruppe 6" : 0010-1101 (0x2D = 45) A47 A46 A45 A44 - A43 A42 A42 A40 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_40_47, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 46: {
                    /*	"Analogfunktionsgruppe 7" : 0010-1110 (0x2E = 46) A55 A54 A53 A52 - A51 A50 A49 A48 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_48_55, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 47: {
                    /*	"Analogfunktionsgruppe 8" : 0010-1111 (0x2F = 47) A63 A62 A61 A60 - A59 A58 A57 A56 */
                    if (notifySusiAnalogFunction) {
                        notifySusiAnalogFunction(SUSI_AN_FN_56_63, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 48: {
                    /*	"Direktbefehl 1 für Analogbetrieb" : 0011-0000 (0x30 = 48) D7 D6 D5 D4 - D3 D2 D1 D0
                    *
                    *	Einstellung von Grundfunktionen im Analogbetrieb unter Umgehung einer Funktionszuordnung.
                    *	- Bit 0: Sound an/aus
                    *	- Bit 1: Auf-/Abrüsten
                    *	- Bit 2-6: reserviert
                    *	- Bit 7: Reduzierte Lautstärke
                    *
                    *	Impostazione delle funzioni di base in modalità analogica ignorando un'assegnazione di funzione.
                    *	- Bit 0: Suono on/off
                    *	- Bit 1: Aggiornamento/Disarmo
                    *	- Bit 2-6: riservato
                    *	- Bit 7: Volume ridotto
                    */
                    if (notifySusiAnalogDirectCommand) {
                        notifySusiAnalogDirectCommand(1, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 49: {
                    /*	"Direktbefehl 2 für Analogbetrieb" : 0011-0000 (0x31 = 49) D7 D6 D5 D4 - D3 D2 D1 D0
                    *
                    *	Einstellung von Grundfunktionen im Analogbetrieb unter Umgehung einer Funktionszuordnung.
                    *	- Bit 0: Spitzenlicht
                    *	- Bit 1: Schlusslicht
                    *	- Bit 2: Standlicht
                    *	- Bit 3-7: reserviert
                    *
                    *	Impostazione delle funzioni di base in modalità analogica ignorando un'assegnazione di funzione.
                    *	- Bit 0: Luce di picco
                    *	- Bit 1: Fanale posteriore
                    *	- Bit 2: Stand light
                    *	- Bit 3-7: riservato
                    */
                    if (notifySusiAnalogDirectCommand) {
                        notifySusiAnalogDirectCommand(2, _BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 0: {
                    /*	"No Operation" : 0000-0000 (0x00 = 0) X X X X - X X X X
                    *
                    *	Der Befehl bewirkt keine Aktion im Slave. Die Daten können einen
                    *	beliebigen Wert haben. Der Befehl kann als Lückenfüller oder zu
                    *	Testzwecken verwendet werden.
                    *
                    *	Il comando non esegue alcuna azione nello slave.
                    *	I dati possono avere qualsiasi valore. Il comando puo' essere utilizzato come gap filler o a scopo di test.
                    */
                    if (notifySusiNoOperation) {
                        notifySusiNoOperation(_BufferPointer->Byte[1]);
                    }
                    break;
                }
                case 94: {	//&& 95
                    /*	"Moduladresse low" : 0101-1110 (0x5E = 94) A7 A6 A5 A4 - A3 A2 A1 A0
                    *
                    *	Übermittelt die niederwertigen Bits der aktiven Digitaladresse des "Masters", wenn er sich in einer digitalen Betriebsart befindet.
                    *	Der Befehl wird immer paarweise vor der Adresse high Byte gesendet.
                    *	Folgen die beiden Befehle nicht direkt aufeinander, so sind sie zu ignorieren.
                    *
                    *	Invia i bit di basso valore dell'indirizzo digitale attivo del "master" quando e' in modalita' digitale.
                    *	Il comando viene sempre inviato in coppie in coppia prima dell'indirizzo high bytes.
                    *	Se i due comandi non si susseguono direttamente, devono essere ignorati.
                    *
                    *
                    *	"Moduladresse high" : 0101-1111 (0x5F = 95) A15 A14 A13 A12 - A11 A10 A9 A8
                    *
                    *	Übermittelt die höherwertigen Bits der aktiven Digitaladresse des "Masters", wenn er sich in einer digitalen Betriebsart befindet.
                    *	Der Befehl wird immer paarweise nach der Adresse low Byte gesendet.
                    *	Folgen die beiden Befehle nicht direkt aufeinander, so sind sie zu ignorieren.
                    *
                    *	Invia i bit di qualita' superiore dell'indirizzo digitale attivo del "master" quando e' in modalita' digitale.
                    *	Il comando viene sempre inviato a coppie in base all'indirizzo a byte basso.
                    *	Se i due comandi non si susseguono direttamente, devono essere ignorati.
                    */

                    Rcn600Message* next = _BufferPointer->nextMessage;
                    if (next != NULL) {
                        if (next->Byte[0] == 95) {							//i byte di comando devono susseguirsi
                            if (notifySusiMasterAddress) {					// Controllo se e' presente il metodo per gestire il comando
                                static uint16_t MasterAddress;

                                MasterAddress = next->Byte[1];				//memorizzo i bit "piu' significativ"
                                MasterAddress = MasterAddress << 8;			//sposto i bit 7 posti a 'sinistra'
                                MasterAddress |= _BufferPointer->Byte[1];	//aggiungo i 7 bit "meno significativi"

                                notifySusiMasterAddress(MasterAddress);
                            }
                        }
                    }
                    else {
                        // Il messaggio successivo NECESSARIO non e' ancora stato ricevuto, esco dal process senza modifiche
                        return 1;
                    }
                    break;
                }
                case 108: {
                    /*	"Modul-Steuerbyte" : 0110-1100 (0x6C = 108) B7 B6 B5 B4 - B3 B2 B1 B0
                    *
                    *	- Bit 0 = Buffer Control: 0 = Puffer aus, 1 = Puffer an
                    *	- Bit 1 = Rücksetzfunktion: 0 = alle Funktionen auf "Aus" setzen, 1 = normaler Betrieb
                    *	Alle weiteren Bits von der RailCommunity reserviert.
                    *	Falls implementiert sind nach einem Reset die Bits 0 und 1 im Slave auf 1 zu setzen.
                    *
                    *	- Bit 0 = Controllo buffer: 0 = Buffer off, 1 = Buffer on
                    *	- Bit 1 = Funzione di ripristino: 0 = impostare tutte le funzioni su "Off", 1 = normale funzionamento
                    *	Tutti gli altri bit riservati dalla RailCommunity.
                    *	Se implementati, i bit 0 e 1 nello slave devono essere impostati su 1 dopo un ripristino.
                    */

                    if (notifySusiControllModule) {
                        notifySusiControllModule(_BufferPointer->Byte[1]);
                    }
                    break;
                }
                default: {	// Messagio non Valido
                    // Aggiorno il puntatore del Buffer
                    Rcn600Message* p = _BufferPointer->nextMessage;
                    _BufferPointer->nextMessage = FREE_MESSAGE_SLOT;
                    _BufferPointer = p;

                    return -1;
                }
            }

            // Aggiorno il puntatore del Buffer
            Rcn600Message* p = _BufferPointer->nextMessage;
            _BufferPointer->nextMessage = FREE_MESSAGE_SLOT;
            _BufferPointer = p;

            // Messaggio decodificato correttamente
            return 1;
        }
        else {
            // Nessun messaggio da decodificare
            return 0;
        }
    }

    // Nessun messaggio da decodificare
    return 0;
}
