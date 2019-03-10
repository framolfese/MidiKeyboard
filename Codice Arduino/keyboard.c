#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it
#include <keyboard.h>



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

  //initialize the ToneStruc Vector
  Tone toneStructVector[8];
  toneStructVector[2].value='F';
  toneStructVector[3].value='M';
  toneStructVector[4].value='R';
  toneStructVector[5].value='D';


  while(1){
    for(i=2;i<6;i++){
    mask = (1<<i);
    int key=(PINF&mask)==0; 
    if(key!=0){
      
      
      printf("switch %02x, %d, sono la nota %c,\n", (int) PORTF, key,toneStructVector[i].value);
      printf("la struct %p\n",toneStructVector[i]);
      }
    }
    _delay_ms(100); // from delay.h, wait 1 sec
  }
  
}