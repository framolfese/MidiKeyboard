#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it
#include <keyboard.h>


int SendToSerial(Tone *tone)
{

    char buffer[sizeof(Tone)+1];
    
    memcpy(buffer, tone, sizeof(Tone));
    //sinchro
    printf("%c",0XAA);
    printf("%c",0X55);
    printf("%c",0XAA);
    
    //struct tone
    int i = 0;
    for (i=sizeof(Tone)-1; i>=0 ; i--) printf("%c",buffer[i]);
    
    //checksum
    printf("%c",0);
    
    //connection ended
    printf("%c",0X55);
  
    return 0;
}


int main(void)
{
  // this initializes the printf/uart thingies
  printf_init();

    uint8_t mask; //(1<<2);
    int i;
    mask = 0x1E;
    DDRF &= ~mask;
    PORTF |= mask;
  
  //initialize the ToneStruc Vector
  Tone toneStructVector[8];
  toneStructVector[2].value = 'F';
  toneStructVector[2].some_value = 0;
 
  toneStructVector[3].value = 'M';
  toneStructVector[3].some_value = 0;
  
  toneStructVector[4].value = 'R';
  toneStructVector[4].some_value = 0;
  
  toneStructVector[5].value = 'D';
  toneStructVector[5].some_value = 0;

    
  uint8_t lettura, lettura_prec;
  
  while (1)
  {
    lettura = PINF & mask;
    for (int i=2;i<6;++i){
        if(((lettura>>i)&0x1) != ((lettura_prec>>i)&0x1)) {
            // cambiamento
            // trasmetti (lettura>>i)&0x01 che vale 0 o 1 alla pressione 
            SendToSerial(&toneStructVector[i]);
            toneStructVector[5].some_value = (lettura>>i)&0x01;
        }
    }
    
    lettura_prec=lettura
    
    /*for (i = 2; i < 6; i++)
    {
      mask = (1 << i);
      int key = (PINF & mask) == 0;
      if (mask!=lastMask )
      {
        lastMask=mask;
        //printf("switch %02x, %d, sono la nota %c,\n", (int) PORTF, key,toneStructVector[i].value);
        //printf("la struct %p\n",toneStructVector[i]);
    
        SendToSerial(&toneStructVector[i]);
      }
    }
    */
    
    //_delay_ms(50); // from delay.h, wait 1 sec
  }
}