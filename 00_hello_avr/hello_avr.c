#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uart.h" // this includes the printf and initializes it
#include "hello_avr.h"

int main(void)
{
  // this initializes the printf/uart thingies
  printf_init();

  Prova *prova = (Prova *)malloc(sizeof(Prova));
  prova->value = 5;
  prova->valore = 6;

  char buffer[sizeof(Prova)];
  memcpy(buffer, prova, sizeof(Prova));
  int i;
  while (1)
  {
    printf("%c", 0xAA);
    printf("%c", 0xBB);
    for (i = 0; i < sizeof(Prova); i++)
    {
      printf("%c", buffer[i]);
    }
    printf("%c", 0xBB);
    _delay_ms(1000);
  }

  return 0;
}
