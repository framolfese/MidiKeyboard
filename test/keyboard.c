#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it
#include "../CodiceArduino/keyboard.h"


//sendOverìserial test senza puntatore a struct
int sendoverserial(Tone tone)
{
  //testare correttezza con unsigned
  //unsigned char buffer[sizeof(Tone)];

  char buffer[sizeof(Tone)];
  memcpy(buffer, &tone, sizeof(Tone));

  //sinchro
  printf("%c", (unsigned char)0Xaa);
  printf("%c", (unsigned char)0Xbb);

  //struct tone
  int i;
  for (i = 0; i < sizeof(Tone); i++)
    printf("%c", buffer[i]); //verficare se ci voglia o meno l'unsigned printf("%c", (unsigned char) buffer[i]);

  //checksum
  printf("%c",(unsigned char) checksum((char*)&tone,sizeof(tone)));

  //connection ended
  printf("%c", (unsigned char)0Xbb);

  return 0;
}



int main(void){
  // this initializes the printf/uart thingies
  printf_init(); 

  // we connect the switch to pin 12
  // that is the bit 6 of port b
  
  const uint8_t mask=(1<<0);
  // we configure the pin as input, clearing the bit 6
  DDRF &= ~mask;
  
  // we enable pullup resistor on that pin
  PORTF |= mask;
  Tone nota;
  nota.nota = 'F';
  nota.on = 0;
  nota.intensity = 0;

  
  while(1){
    //int key=(PINF&mask)==0; // we extract the bit value of the 6th bit
    //printf("switch %02x, %d\n", (int) PORTF, key);


  sendoverserial(nota);

 _delay_ms(500); // from delay.h, wait 1 sec
  
  
  
  }
  
}
