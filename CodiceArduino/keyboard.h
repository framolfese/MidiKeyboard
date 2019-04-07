#include <string.h>

#include <stdio.h>
#include <stdint.h>

#include <stdlib.h>

#include "../LibreriaCondivisa/libreria.h"
#include "../avr_common/uart.h" // this includes the printf and initializes it

//this funcition provides synchronization over the uart
// 0XAA
// 0XBB
// Struct Tone di dimensione sizeof(Tone)
// eventuale checksum che implementeremo, per adesso invio "0"
// 0XBB questo serve per dire di aver finito i pacchetti inviati
//int sendoverserial(Tone tone);

