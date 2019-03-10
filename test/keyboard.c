#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it

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
  
  while(1){
    //int key=(PINF&mask)==0; // we extract the bit value of the 6th bit
    //printf("switch %02x, %d\n", (int) PORTF, key);
	printf("sono una BOARD\n");    
_delay_ms(500); // from delay.h, wait 1 sec
  }
  
}