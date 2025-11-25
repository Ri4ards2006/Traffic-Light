# API Libreria

* [Funzioni Obbligatorie Master](#Funzioni-Obbligatorie-Master)
* [Funzioni Obbligatorie Slave](#Funzioni-Obbligatorie-Slave)
* [Funzioni Comandi Master](#Funzioni-Comandi-Master)
* [Funzioni CallBack Slave](#Funzioni-CallBack-Slave)
* [Manipolazione CVs](#Manipolazione-CVs)
* [Tipi di Dati](#Tipi-di-Dati)

------------

# Funzioni Obbligatorie Master
Il dispositivo *Master* NON richiede funzioni particolari, e' necessario avviare il bus I2c tramite i metodi messi a disposizione dalla libreria [Wire](https://www.arduino.cc/en/Reference/Wire):

```c
Wire.begin();
```
**Facoltativamente** si puo' impostare un indirizzo per tale dispositivo.

**CONSIGLIATO:**
```c
Wire.setClock(400000);
```
Imposta la frequenza del bus a 400kHz, **necessari resistori di pull-up da 2kOhm**.</br>

------------

# Funzioni Obbligatorie Slave

I seguenti metodi sono **obbligatori** per il corretto funzionamento della libreria.

Avviare della libreria Wire **con indirizzo**.</br>
```c
Wire.begin(indirizzo);
```


Impostare *gli Handle* per gli eventi I2c:
```c
Wire.onReceive(onReceiveWireSusi);
Wire.onRequest(onRequestWireSusi);
```


**Elaborare i dati ricevuti:**
```c
int8_t processWireSusi(void);
```
**E' necessario invocarlo piu' volte possibile** nel 'loop' del codice: decodifica i pacchetti SUSI ricevuti.
- Input:
  - Nulla
- Restituisce:
  -  -1 **Messaggio Non Valido**
  -  0  Nessun Messaggio in Coda di Decodifica
  -  1  **Messaggio Valido**

------------

# Funzioni Comandi Master

Le seguenti funzioni sono a disposizione del dispositivo Master per comandare gli Slave.

------------

```c
int8_t	sendCommandWireSusi(uint8_t I2C_Address, uint8_t FirstByte, uint8_t SecondByte);
```
*sendCommandWireSusi* spedisce un comando Susi ad uno Slave, NON e' necessaria una risposta dallo Slave.</br>
- Input:
  - L'indirizzo del dispositivo I2c Ricevente
  - Il Primo Byte del Messaggio ( Comando )
  - Il Secondo Byte del Messaggio ( Argomento )
- Restituisce:
  - 0 Messaggio inviato, -1 in caso di Errore di Trasmissione, -2 Valore CV dopo scrittura non uguale al richiesto

------------

```c
int16_t	readCVsWireSusi(uint8_t I2C_Address, uint16_t cvAddress);
```
*readCVsWireSusi* richiede ad uno Slave di *Leggere* una CV.</br>
- Input:
  - L'indirizzo del dispositivo I2c Ricevente
  - L'indirizzo della CV da leggere
- Restituisce:
  - Il valore della CV letta, -1 Errore

------------

```c
int16_t	writeCVsWireSusi(uint8_t I2C_Address, uint16_t cvAddress, uint8_t cvValue);
```
*writeCVsWireSusi* richiede ad uno Slave di *Scrivere* una CV.</br>
- Input:
  - L'indirizzo del dispositivo I2c Ricevente
  - L'indirizzo della CV da scrivere
  - Il nuovo valore della CV da scrivere
- Restituisce:
  - Il valore della CV scritta, -1 Errore

------------

# Funzioni CallBack Slave
Le seguenti funzioni CallBack sono **facoltative** (definiti come 'extern' alla libreria), e permettono all'utente di definire il comportamento da adottare in caso di un particolare comando.</br>

------------

```c
void onReceiveWireSusiExternalHandle(uint8_t nBytes);
```
Handle **Facoltativo**, permette all'utente di definire azioni in caso di comandi non riconosciuti (dimensione maggiore di 3):
* Input:
  - Quantita' di Byte ricevuti sul bus Wire
* Restituisce:
  - Nulla

------------

```c
void notifySusiFunc(SUSI_FN_GROUP SUSI_FuncGrp, uint8_t SUSI_FuncState);
```
*notifySusiFunc()* viene invocato quando: si ricevono i dati dal Master su un gruppo di funzioni digitali:
* Input:
  - il gruppo Funzioni decodificato
  - lo stato del gruppo funzioni
* Restituisce:
  - Nulla

------------

```c
void notifySusiBinaryState(uint16_t Command, uint8_t CommandState);
```
*notifySusiBinaryState()* viene invocato quando: si ricevono i dati dal Master sullo stato di UNA specifica funzione:
- Input:
  - il numero della funzione (da 1 a 32767)
  - lo stato della Funzione (attiva = 1, disattiva = 0)
- Restituisce:
  - Nulla

------------

```c
void notifySusiAux(SUSI_AUX_GROUP SUSI_auxGrp, uint8_t SUSI_AuxState);
```
*notifySusiAux()* viene invocato quando: si ricevono i dati dal Master sullo stato di UNA specifica AUX:
- Input:
  - il numero dell'AUX
  - lo stato dell'uscita (attiva = 1, disattiva = 0)
- Restituisce:
  - Nulla

------------

```c
void notifySusiTriggerPulse(uint8_t state);
```
notifySusiTriggerPulse() viene invocato quando: si riceve dal Master il comando di Trigger (o pulsazione) per eventuali sbuffi di vapore
- Input:
  - stato del comando Trigger/Pulse
- Restituisce:
  - Nulla

------------

```c
void notifySusiMotorCurrent(int8_t current);
```
*notifySusiMotorCurrent()* viene invocato quando: si riceve dal Master i dati sull'assorbimento di Corrente da parte del Motore
- Input:
  - Assorbimento di Corrente: da -128 a + 127 (gia' convertita dal Complemento a 2 originale)
- Restituisce:
  - Nulla

------------

```c
void notifySusiRequestSpeed(uint8_t Speed, SUSI_DIRECTION Dir);
```
*notifySusiRequestSpeed()* viene invocato quando: si ricevono i dati sulla Velocita' e sulla Direzione richiesti dalla Centrale al Master
- Input:
  - la velocita' (128 step) richiesta
  - la direzione richiesta
- Restituisce:
  - Nulla

------------

```c
void notifySusiRealSpeed(uint8_t Speed, SUSI_DIRECTION Dir);
```
*notifySusiRealSpeed()* viene invocato quando: si ricevono i dati dal Master sulla Velocita' e sulla Direzione reali 
- Input:
  - la velocita' (128 step) reale
  - la direzione reale
- Restituisce:
  - Nulla

------------

```c
void notifySusiMotorLoad(int8_t load);
```
*notifySusiAnalogFunction()* viene invocato quando: si riceve dal Master i dati sul carico del Motore
- Input:
  - Carico del Motore: da -128 a + 127 (gia' convertita dal Complemento a 2 originale)
- Restituisce:
  - Nulla

------------

```c
void notifySusiAnalogFunction(SUSI_AN_GROUP SUSI_AnalogGrp, uint8_t SUSI_AnalogState);
```
*notifySusiAnalogFunction()* viene invocato quando: si ricevono i dati dal Master su un gruppo di funzioni analogiche
- Input:
  - il gruppo Analogico decodificato
  - lo stato del gruppo
- Restituisce:
  - Nulla

------------

```c
void notifySusiAnalogDirectCommand(uint8_t commandNumber, uint8_t Command);
```
*notifySusiAnalogDirectCommand()* viene invocato quando: si ricevono i dati dal Master i comandi diretti per il funzionamento analogico
- Input:
  - il numero del comando: 1 o 2
  - i bit del comando
- Restituisce:
  - Nulla

------------

```c
void notifySusiMasterAddress(uint16_t MasterAddress);
```
*notifySusiMasterAddress()* viene invocato quando: si riceve l'indirizzo digitale del Master
- Input:
  - l'indirizzo Digitale del Master
- Restituisce:
  - Nulla

------------

```c
void notifySusiControllModule(uint8_t ModuleControll);
```
*notifySusiControlModule()* viene invocato quando: si riceve il comando sul controllo del modulo
- Input:
  - byte contenete il controllo del modulo
- Restituisce:
  - Nulla

------------

# Manipolazione CVs
Le seguenti funzioni sono **facoltative** (definite come 'extern' alla libreria), ma permettono alla libreria di dialogare con il Master in caso di *Lettura/Scrittura CVs*</br>
La libreria **gestisce l'ACK** che permette al decoder di conoscere l'esito dell'operazione richiesta.

------------

```c
uint8_t notifySusiCVRead(uint16_t CV);
```
*notifySusiCVRead()* viene invocato quando: e' richiesta la lettura di una CV
- Input:
  - il numero della CV da leggere
- Restituisce:
  - il valore della CV letta

------------

```c
uint8_t notifySusiCVWrite(uint16_t CV, uint8_t Value);
```
*notifySusiCVWrite()* viene invocato quando: e' richiesta la Scrittura di una CV.
- Input:
  - il numero della CV richiesta
  - il Nuovo valore della CV
- Restituisce:
  - il valore letto (post scrittura) della CV da scrivere

------------

RESET CVs, viene utilizzato la *stessa funzione* della Libreria [NmraDcc](https://github.com/mrrwa/NmraDcc):</br>
```c
void notifyCVResetFactoryDefault(void);
```
*notifyCVResetFactoryDefault()* Called when CVs must be reset. This is called when CVs must be reset to their factory defaults.
- Inputs:
  - None                                                                                                       
- Returns:
  - None

**N.B.**: Per eseguire un reset delle CVs e' necessario scrivere il valore **8** in una delle seguenti CVs: *8, 900, 940, 980*.

------------

# Tipi di Dati
I seguenti tipi di dati vengono utilizzati dai metodi/funzioni della libreria, *sono tipi simbolici* definiti tramite "#define" e servono a migliorare la leggibilita' del codice, corrispondono al tipo *uint8_t*</br>

```c
#define	SUSI_DIRECTION		uint8_t
...
#define	SUSI_FN_GROUP		uint8_t
...
#define	SUSI_AUX_GROUP		uint8_t
...
#define	SUSI_AN_GROUP		uint8_t
```

------------

```c
SUSI_DIRECTION
```
Identifica *simbolicamente* la direzione trasmessa dal Decoder Master: </br>
- SUSI_DIR_REV : Direzione *reverse*
- SUSI_DIR_FWD : Direzione *forward*

------------

```c
SUSI_FN_GROUP
```
Identifica *simbolicamente* il gruppo di Funzioni Digitali trasmesse dal Decoder Master:</br>
- SUSI_FN_0_4 : Funzioni dalla 0 alla 4
- SUSI_FN_5_12 : Funzioni dalla 5 alla 12
- SUSI_FN_13_20 : Funzioni dalla 13 alla 20
- SUSI_FN_21_28 : Funzioni dalla 21 alla 28
- SUSI_FN_29_36 : Funzioni dalla 29 alla 36
- SUSI_FN_37_44 : Funzioni dalla 37 alla 44
- SUSI_FN_45_52 : Funzioni dalla 45 alla 52
- SUSI_FN_53_60 : Funzioni dalla 53 alla 60
- SUSI_FN_61_68 : Funzioni dalla 61 alla 68

------------

```c
SUSI_AUX_GROUP
```
Identifica *simbolicamente* il gruppo di AUXs trasmesse dal Decoder Master:</br>
- SUSI_AUX_1_8 : AUX dalla 1 alla 8
- SUSI_AUX_9_16 : AUX dalla 9 alla 16
- SUSI_AUX_17_24 : AUX dalla 17 alla 24
- SUSI_AUX_25_32 : AUX dalla 25 alla 32

------------

```c
SUSI_AN_FN_GROUP
```
Identifica *simbolicamente* il gruppo di Funzioni Analogiche trasmesse dal Decoder Master:</br>
- SUSI_AN_FN_0_7 : Funzioni Analogiche dalla 0 alla 7
- SUSI_AN_FN_8_15 : Funzioni Analogiche dalla 8 alla 15
- SUSI_AN_FN_16_23 : Funzioni Analogiche dalla 16 alla 23
- SUSI_AN_FN_24_31 : Funzioni Analogiche dalla 24 alla 31
- SUSI_AN_FN_32_39 : Funzioni Analogiche dalla 32 alla 39
- SUSI_AN_FN_40_47 : Funzioni Analogiche dalla 40 alla 47
- SUSI_AN_FN_48_55 : Funzioni Analogiche dalla 48 alla 55
- SUSI_AN_FN_56_63 : Funzioni Analogiche dalla 56 alla 63

------------
