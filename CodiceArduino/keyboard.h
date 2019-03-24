#include <string.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it

#include <stdlib.h>


#pragma pack(push,1)
typedef struct Tone
{
    //questo campo vale 1 se il tasto è premuto e 0 altrimenti
    uint8_t nota; 
    //valore campo della nota premuta
    uint8_t on;
    uint8_t intensity;

    
} Tone;
#pragma pack(pop)

//this funcition provides synchronization over the uart
// 0XAA
// 0X55
// 0XAA
// Struct Tone di dimensione sizeof(Tone)
// eventuale checksum che implementeremo, per adesso invio "0"
// 0X55 questo serve per dire di aver finito i pacchetti inviati

int SendToSerial(Tone *tone);