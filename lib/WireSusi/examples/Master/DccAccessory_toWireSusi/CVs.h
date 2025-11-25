#ifndef Default_CVs_h
#define Default_CVs_h

#include <avr/wdt.h>                                                            // Gestione WatchDog Timer
#include <avr/eeprom.h>                                                         // Gestione della EEPROM
#include <avr/pgmspace.h>                                                       // Utilizzatr Flash per tenere valori costanti
#include <stdint.h>                                                             // Tipi u/intX_t

/*  Indirizzi CV  */
#define CV_SLAVE_SELECTOR               31                                      // CV che indica quale Slave I2c su vuole programmare

/*  Valori di Default  */
#define PROJECT_ID                      21                                      // ID del progetto
#define DEFAULT_SLAVE_ADDRESS           0                                       // Di base lavoro sulle CV interne
#define FIRMWARE_VERSION                1                                       // Versione FIrmWare 1

////////////////////////////////////////////////
    /*  Funzioni  */

#if defined (__cplusplus)                                                       // Libreria scritta in C
extern "C" {
#endif
    /*
    * resetCVs() Esegue un reset di tutte le CVs
    *  Input:
    *  - Nulla
    * Restituisce:
    *  -  Nulla
    */
    void resetCVs(void);
#if defined (__cplusplus)
}
#endif

////////////////////////////////////////////////

//lista dei valori 'di fabbrica' delle CVs per il modulo TFX004

#define CV0_VALUE   PROJECT_ID                                                  // CV Identificativo progetto
#define CV1_VALUE   1                                                           //8 bit meno significativi indirizzo Dcc
#define CV2_VALUE   0
#define CV3_VALUE   0
#define CV4_VALUE   0
#define CV5_VALUE   0
#define CV6_VALUE   0
#define CV7_VALUE   FIRMWARE_VERSION
#define CV8_VALUE   13                                                          // Manufacter ID
#define CV9_VALUE   0
#define CV10_VALUE   0
#define CV11_VALUE   0
#define CV12_VALUE   0
#define CV13_VALUE   0
#define CV14_VALUE   0
#define CV15_VALUE   0
#define CV16_VALUE   0
#define CV17_VALUE   0
#define CV18_VALUE   0
#define CV19_VALUE   0
#define CV20_VALUE   0
#define CV21_VALUE   0
#define CV22_VALUE   0
#define CV23_VALUE   0
#define CV24_VALUE   0
#define CV25_VALUE   0
#define CV26_VALUE   0
#define CV27_VALUE   0
#define CV28_VALUE   0
#define CV29_VALUE   0
#define CV30_VALUE   0
#define CV31_VALUE   DEFAULT_SLAVE_ADDRESS                                      // Selettore Slave I2c                    
#define CV32_VALUE   0
#define CV33_VALUE   0
#define CV34_VALUE   0
#define CV35_VALUE   0
#define CV36_VALUE   0
#define CV37_VALUE   0
#define CV38_VALUE   0
#define CV39_VALUE   0
#define CV40_VALUE   0
#define CV41_VALUE   0
#define CV42_VALUE   0
#define CV43_VALUE   0
#define CV44_VALUE   0
#define CV45_VALUE   0
#define CV46_VALUE   0
#define CV47_VALUE   0
#define CV48_VALUE   0
#define CV49_VALUE   0
#define CV50_VALUE   0
#define CV51_VALUE   0
#define CV52_VALUE   0
#define CV53_VALUE   0
#define CV54_VALUE   0
#define CV55_VALUE   0
#define CV56_VALUE   0
#define CV57_VALUE   0
#define CV58_VALUE   0
#define CV59_VALUE   0
#define CV60_VALUE   0
#define CV61_VALUE   0
#define CV62_VALUE   0
#define CV63_VALUE   0
#define CV64_VALUE   0
#define CV65_VALUE   0
#define CV66_VALUE   0
#define CV67_VALUE   0
#define CV68_VALUE   0
#define CV69_VALUE   0
#define CV70_VALUE   0
#define CV71_VALUE   0
#define CV72_VALUE   0
#define CV73_VALUE   0
#define CV74_VALUE   0
#define CV75_VALUE   0
#define CV76_VALUE   0
#define CV77_VALUE   0
#define CV78_VALUE   0
#define CV79_VALUE   0
#define CV80_VALUE   0
#define CV81_VALUE   0
#define CV82_VALUE   0
#define CV83_VALUE   0
#define CV84_VALUE   0
#define CV85_VALUE   0
#define CV86_VALUE   0
#define CV87_VALUE   0
#define CV88_VALUE   0
#define CV89_VALUE   0
#define CV90_VALUE   0
#define CV91_VALUE   0
#define CV92_VALUE   0
#define CV93_VALUE   0
#define CV94_VALUE   0
#define CV95_VALUE   0
#define CV96_VALUE   0
#define CV97_VALUE   0
#define CV98_VALUE   0
#define CV99_VALUE   0
#define CV100_VALUE   0
#define CV101_VALUE   0
#define CV102_VALUE   0
#define CV103_VALUE   0
#define CV104_VALUE   0
#define CV105_VALUE   0
#define CV106_VALUE   0
#define CV107_VALUE   0
#define CV108_VALUE   0
#define CV109_VALUE   0
#define CV110_VALUE   0
#define CV111_VALUE   0
#define CV112_VALUE   29    // Numero Funzioni per Modulo I2c con indirizzo 2
#define CV113_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 3
#define CV114_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 4
#define CV115_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 5
#define CV116_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 6
#define CV117_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 7
#define CV118_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 8
#define CV119_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 9
#define CV120_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 10
#define CV121_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 11
#define CV122_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 12
#define CV123_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 13
#define CV124_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 14
#define CV125_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 15
#define CV126_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 16
#define CV127_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 17
#define CV128_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 18
#define CV129_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 19
#define CV130_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 20
#define CV131_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 21
#define CV132_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 22
#define CV133_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 23
#define CV134_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 24
#define CV135_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 25
#define CV136_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 26
#define CV137_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 27
#define CV138_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 28
#define CV139_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 29
#define CV140_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 30
#define CV141_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 31
#define CV142_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 32
#define CV143_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 33
#define CV144_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 34
#define CV145_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 35
#define CV146_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 36
#define CV147_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 37
#define CV148_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 38
#define CV149_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 39
#define CV150_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 40
#define CV151_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 41
#define CV152_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 42
#define CV153_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 43
#define CV154_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 44
#define CV155_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 45
#define CV156_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 46
#define CV157_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 47
#define CV158_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 48
#define CV159_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 49
#define CV160_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 50
#define CV161_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 51
#define CV162_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 52
#define CV163_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 53
#define CV164_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 54
#define CV165_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 55
#define CV166_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 56
#define CV167_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 57
#define CV168_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 58
#define CV169_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 59
#define CV170_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 60
#define CV171_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 61
#define CV172_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 62
#define CV173_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 63
#define CV174_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 64
#define CV175_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 65
#define CV176_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 66
#define CV177_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 67
#define CV178_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 68
#define CV179_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 69
#define CV180_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 70
#define CV181_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 71
#define CV182_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 72
#define CV183_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 73
#define CV184_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 74
#define CV185_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 75
#define CV186_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 76
#define CV187_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 77
#define CV188_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 78
#define CV189_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 79
#define CV190_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 80
#define CV191_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 81
#define CV192_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 82
#define CV193_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 83
#define CV194_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 84
#define CV195_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 85
#define CV196_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 86
#define CV197_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 87
#define CV198_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 88
#define CV199_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 89
#define CV200_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 90
#define CV201_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 91
#define CV202_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 92
#define CV203_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 93
#define CV204_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 94
#define CV205_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 95
#define CV206_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 96
#define CV207_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 97
#define CV208_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 98
#define CV209_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 99
#define CV210_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 100
#define CV211_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 101
#define CV212_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 102
#define CV213_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 103
#define CV214_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 104
#define CV215_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 105
#define CV216_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 106
#define CV217_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 107
#define CV218_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 108
#define CV219_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 109
#define CV220_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 110
#define CV221_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 111
#define CV222_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 112
#define CV223_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 113
#define CV224_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 114
#define CV225_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 115
#define CV226_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 116
#define CV227_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 117
#define CV228_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 118
#define CV229_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 119
#define CV230_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 120
#define CV231_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 121
#define CV232_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 122
#define CV233_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 123
#define CV234_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 124
#define CV235_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 125
#define CV236_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 126
#define CV237_VALUE   0     // Numero Funzioni per Modulo I2c con indirizzo 127

extern const PROGMEM uint8_t FactoryDefaultCVsValue[238];

#endif
