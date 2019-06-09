#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#pragma pack(push,1)
typedef struct Tone
{
    uint16_t nota; 
    uint8_t on;
    uint8_t tasto;
} Tone;
#pragma pack(pop)

char checkSum(char *buffer, size_t size);
int sendoverserial(Tone tone);