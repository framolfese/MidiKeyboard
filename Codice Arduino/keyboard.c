#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it
#include <keyboard.h>


int SendToSerial(Tone *tone)
{

    char buffer[sizeof(Tone)];
    memcpy(buffer, tone, sizeof(Tone));
    int i = 0;
    for (i=sizeof(Tone)-1; i>=0 ; i--)
    printf("%c",buffer[i]);
   
  
    return 0;
}


int main(void)
{
  // this initializes the printf/uart thingies
  printf_init();

  uint8_t mask; //(1<<2);
  int i;
  for (i = 2; i < 6; i++)
  {
    mask = (1 << i);
    DDRF &= ~mask;
    PORTF |= mask;
  }

  //initialize the ToneStruc Vector
  Tone toneStructVector[8];
  toneStructVector[2].value = 'F';
  toneStructVector[2].marco = 1;
 
  toneStructVector[3].value = 'M';
  toneStructVector[3].marco = 2;
  
  toneStructVector[4].value = 'R';
  toneStructVector[4].marco = 3;
  
  toneStructVector[5].value = 'D';
  toneStructVector[5].marco = 4;

  while (1)
  {
    for (i = 2; i < 6; i++)
    {
      mask = (1 << i);
      int key = (PINF & mask) == 0;
      if (key != 0)
      {

        //printf("switch %02x, %d, sono la nota %c,\n", (int) PORTF, key,toneStructVector[i].value);
        //printf("la struct %p\n",toneStructVector[i]);
    
        SendToSerial(&toneStructVector[i]);
      }
    }
    _delay_ms(100); // from delay.h, wait 1 sec
  }
}