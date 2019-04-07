#include <string.h>

#include <stdio.h>
#include <stdint.h>

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


// dove buffer è la struct di cui vogliamo calcolare il checksum e size è la size di todo
unsigned char checksum(char *buffer, size_t size);