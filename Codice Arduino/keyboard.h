#include <string.h>

typedef struct Tone
{
    int marco;
    char value;
    
} Tone;

int SendToSerial(Tone *tone);