#include <string.h>

#pragma pack(push,1)
typedef struct Tone
{
    uint8_t some_value;
    char value;
    
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