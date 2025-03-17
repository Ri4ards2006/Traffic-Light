#ifndef WIRE_SUSI_h
#define WIRE_SUSI_h

/* LIB_VERSION: 0.1.6 */

#include <Arduino.h>                                                // Libreria per le funzioni tipiche dell'Arduino IDE

#include <stdint.h>                                                 // Libreria per i tipi "uintX_t"
#include <Wire.h>                                                   // Libreria per la gestione dell'interfaccia I2c

#ifdef __AVR__                                                      // Se la piattaforma e' AVR puo' essere usata la libreria digitalPinFast; e' escludibile per risparmiare SRAM
    #include <util/delay.h>                                         // Libreria per la gestione del Delay tramite libreria AVR
#endif

#include "DataHeaders/SUSI_DATA_TYPE.h"                             // Tipi Simbolici per le Funzioni CallBack
#include "DataHeaders/SUSI_FN_BIT.h"                                // bit per il comando delle Funzioni Digitali
#include "DataHeaders/SUSI_AN_FN_BIT.h"                             // bit per il comando delle Funzioni Analogiche
#include "DataHeaders/SUSI_AUX_BIT.h"                               // bit per il comando delle AUX

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*	MASTER	*/

/*
*	sendCommandWireSusi() Spedisce un comando (2 Byte) Susi sul Bus I2c
*	Input:
*		- l'indirizzo I2c dello Slave
*		- il Primo Byte del Messaggio
*		- il Secondo Byte del Messaggio
*	Restituisce:
*		- 0 Messaggio Inviato, -1 Errore
*/
int8_t	sendCommandWireSusi(uint8_t I2C_Address, uint8_t FirstByte, uint8_t SecondByte);
/*
*	readCVsWireSusi() Richiede di Leggere il valore di una CV
*	Input:
*		- l'indirizzo I2c dello Slave
*		- l'indirizzo della CV che si vuole Leggere
*	Restituisce:
*		- il Valore della CV letto, -1 Errore
*/
int16_t	readCVsWireSusi(uint8_t I2C_Address, uint16_t cvAddress);
/*
*	readCVsWireSusi() Richiede di Scrivere il valore di una CV
*	Input:
*		- l'indirizzo I2c dello Slave
*		- l'indirizzo della CV che si vuole Scrivere
*		- il Valore della CV da Scrivere
*	Restituisce:
*		- il Valore della CV scritto
        - -1 Errore indirizzo CV
        - -2 Errore nuovo valore CV
*/
int16_t	writeCVsWireSusi(uint8_t I2C_Address, uint16_t cvAddress, uint8_t cvValue);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*	I2C EVENTS	*/

void onReceiveWireSusi(int nBytes);
void onRequestWireSusi(void);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*	SLAVE	*/

/*
*	processWireSusi() processa i messaggi Susi ricevuti dal master
*	Input:
*		- nulla
*	Restituisce:
*		- -1	MESSAGGIO NON VALIDO
*		-  0	Nessun messaggio in Coda di Decodifica
*		-  1	MESSAGGIO VALIDO
*/
int8_t processWireSusi(void);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /*	CALLBACK: Funzioni Esterne, implementabili a discrizione dell'utente	*/

#if defined (__cplusplus)
extern "C" {
#endif
    /*
    *	onReceiveWireSusiExternalHanlde() viene invocato quando i dati ricevuti hanno lunghezza maggiore di 3 (Nessun Comando SusiOverI2c ha lunghezza maggiore di 3)
    *	Input:
    *		- Quantita' di Byte ricevuti sul bus Wire
    *	Restituisce:
    *		- Nulla
    */
    extern void onReceiveWireSusiExternalHandle(uint8_t nBytes) __attribute__((weak));
    /*
    *	notifySusiRawMessage() viene invocato ogni volta che è presente un messaggio (2 Byte) da decodificare. NON viene invocato per i Messaggi di Manipolazione CVs. Mostra il messaggio Grezzo: NON DECODIFICATO.
    *	Input:
    *		- il Primo Byte del Messaggio
    *		- il Secondo Byte del Messaggio
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiRawMessage(uint8_t firstByte, uint8_t secondByte) __attribute__((weak));
    /*
    *	notifySusiFunc() viene invocato quando: si ricevono i dati dal Master su un gruppo di funzioni digitali
    *	Input:
    *		- il gruppo Funzioni decodificato
    *		- lo stato del gruppo funzioni
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiFunc(SUSI_FN_GROUP SUSI_FuncGrp, uint8_t SUSI_FuncState) __attribute__((weak));
    /*
    *	notifySusiBinaryState() viene invocato quando: si ricevono i dati dal Master sullo stato di UNA specifica funzione
    *	Input:
    *		- il numero della funzione (da 1 a 127)
    *		- lo stato della Funzione (attiva = 1, disattiva = 0)
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiBinaryState(uint16_t Command, uint8_t CommandState) __attribute__((weak));
    /*
    *	notifySusiAux() viene invocato quando: si ricevono i dati dal Master sullo stato di UNA specifica AUX
    *	Input:
    *		- il numero dell'AUX
    *		- lo stato dell'uscita (attiva = 1, disattiva = 0)
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiAux(SUSI_AUX_GROUP SUSI_auxGrp, uint8_t SUSI_AuxState) __attribute__((weak));
    /*
    *	notifySusiTriggerPulse() viene invocato quando: si riceve dal Master il comando di Trigger (o pulsazione) per eventuali sbuffi di vapore
    *	Input:
    *		- stato del comando Trigger/Pulse
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiTriggerPulse(uint8_t state) __attribute__((weak));
    /*
    *	notifySusiMotorCurrent() viene invocato quando: si riceve dal Master i dati sull'assorbimento di Corrente da parte del Motore
    *	Input:
    *		- Assorbimento di Corrente: da -128 a + 127 (gia' convertita dal Complemento a 2 originale)
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiMotorCurrent(int8_t current) __attribute__((weak));
    /*
    *	notifySusiRequestSpeed() viene invocato quando: si ricevono i dati sulla Velocita' e sulla Direzione richiesti dalla Centrale al Master
    *	Input:
    *		- la velocita' (128 step) richiesta
    *		- la direzione richiesta
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiRequestSpeed(uint8_t Speed, SUSI_DIRECTION Dir) __attribute__((weak));
    /*
    *	notifySusiRealSpeed() viene invocato quando: si ricevono i dati dal Master sulla Velocita' e sulla Direzione reali
    *	Input:
    *		- la velocita' (128 step) reale
    *		- la direzione reale
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiRealSpeed(uint8_t Speed, SUSI_DIRECTION Dir) __attribute__((weak));
    /*
    *	notifySusiMotorLoad() viene invocato quando: si riceve dal Master i dati sul carico del Motore
    *	Input:
    *		- Carico del Motore: da -128 a + 127 (gia' convertita dal Complemento a 2 originale)
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiMotorLoad(int8_t load) __attribute__((weak));
    /*
    *	notifySusiAnalogFunction() viene invocato quando: si ricevono i dati dal Master su un gruppo di funzioni analogiche
    *	Input:
    *		- il gruppo Analogico decodificato
    *		- lo stato del gruppo
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiAnalogFunction(SUSI_AN_GROUP SUSI_AnalogGrp, uint8_t SUSI_AnalogState) __attribute__((weak));
    /*
    *	notifySusiAnalogDirectCommand() viene invocato quando: si ricevono i dati dal Master i comandi diretti per il funzionamento analogico
    *	Input:
    *		- il numero del comando: 1 o 2
    *		- i bit del comando
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiAnalogDirectCommand(uint8_t commandNumber, uint8_t Command) __attribute__((weak));
    /*
    *	notifySusiNoOperation() viene invocato quando: si riceve il comando "no operation", serve prevalentemente a scopo di test
    *	Input:
    *		- l'argomento del comando
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiNoOperation(uint8_t commandArgument) __attribute__((weak));
    /*
    *	notifySusiMasterAddress() viene invocato quando: si riceve l'indirizzo digitale del Master
    *	Input:
    *		- l'indirizzo Digitale del Master
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiMasterAddress(uint16_t MasterAddress) __attribute__((weak));
    /*
    *	notifySusiControlModule() viene invocato quando: si riceve il comando sul controllo del modulo
    *	Input:
    *		- byte contenete il controllo del modulo
    *	Restituisce:
    *		- Nulla
    */
    extern	void notifySusiControllModule(uint8_t ModuleControll) __attribute__((weak));



    /* METODI MANIPOLAZIONE CVs */
    /*
    *	notifySusiCVRead() viene invocato quando: e' richiesta la lettura di una CV
    *	Input:
    *		- il numero della CV da leggere
    *	Restituisce:
    *		- ritorna il valore della CV letta
    */
    extern uint8_t notifySusiCVRead(uint16_t CV) __attribute__((weak));
    /*
    *	notifySusiCVWrite() viene invocato quando: e' richiesta la Scrittura di una CV.
    *	Input:
    *		- il numero della CV richiesta
    *		- il Nuovo valore della CV
    *	Restituisce:
    *		- il valore letto (post scrittura) nella posizione richiesta
    */
    extern uint8_t notifySusiCVWrite(uint16_t CV, uint8_t Value) __attribute__((weak));
    /* RESET CVs, viene utilizzato lo stesso metodo della Libreria NmraDcc:
    *
    *  notifyCVResetFactoryDefault() Called when CVs must be reset.
    *                                This is called when CVs must be reset
    *                                to their factory defaults.
    *
    *  Inputs:
    *    None
    *
    *  Returns:
    *    None
    */
    extern void notifyCVResetFactoryDefault(void) __attribute__((weak));
#if defined (__cplusplus)
}
#endif

#endif
