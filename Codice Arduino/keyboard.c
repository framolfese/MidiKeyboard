#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it
#include <keyboard.h>

typedef struct Tone{
    int value;
} Tone;



int main(void){
  // this initializes the printf/uart thingies
  printf_init(); 
 
  uint8_t mask;  //(1<<2);
  int i;
  for(i=2;i<6;i++){
  mask = (1<<i);
  DDRF &= ~mask;
  PORTF |= mask;
  }

  while(1){
    for(i=2;i<6;i++){
    mask = (1<<i);
    int key=(PINF&mask)==0; 
    if(key!=0){
      Tone toneRead;
      toneRead->value=i;
      printf("switch %02x, %d, sono la porta %d,\n", (int) PORTF, key,toneReaded->value);
      }
    }
    _delay_ms(100); // from delay.h, wait 1 sec
  }
  
}