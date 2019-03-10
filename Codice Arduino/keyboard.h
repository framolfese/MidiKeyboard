#include <string.h>

typedef struct Tone
{
    char value;
    int checksum;
} Tone;

int SendToSerial(Tone tone)
{

    char buffer[sizeof(Tone)];
    memcpy(buffer, &tone, sizeof(Tone));
    int i = 0;
    for (i; i < sizeof(Tone); i++)
        printf(buffer + i);
    printf("\n");
    /*
  char *p = (char*) &tone;
  int i;
  for(i=0; i<sizeof(tone); i++) 
   {
   putcUSART(*p++);
   }
   */
    return 0;
}