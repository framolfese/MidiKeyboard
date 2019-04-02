#include <keyboard.h>
#include <util/delay.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it

int main(void)
{
  // this initializes the printf/uart thingies
  printf_init();

  uint8_t mask; //(1<<2);
  //int i;
  mask = 0x3C;
  DDRF &= ~mask;
  PORTF |= mask;

  //initialize the ToneStruc Vector
  //usiamo un vettore di 8 perchè abbiamo intenzione di avere 8 notte più avanti
  Tone toneStructVector[8];
  toneStructVector[2].nota = 'F';
  toneStructVector[2].on = 0;
  toneStructVector[2].intensity = 0;

  toneStructVector[3].nota = 'M';
  toneStructVector[3].on = 0;
  toneStructVector[3].intensity = 0;

  toneStructVector[4].nota = 'R';
  toneStructVector[4].on = 0;
  toneStructVector[4].intensity = 0;

  toneStructVector[5].nota = 'D';
  toneStructVector[5].on = 0;
  toneStructVector[5].intensity = 0;

  uint8_t lettura, lettura_prec;
  lettura = PINF & mask;
  lettura_prec = lettura;

  while (1)
  {
    lettura = PINF & mask;
    //printf("nota: %c, premuta: %c", toneStructVector[0].nota,toneStructVector[0].on );
    for (int i = 2; i < 6; ++i)
    {
      if (((lettura >> i) & 0x1) != ((lettura_prec >> i) & 0x1))
      {
        // cambiamento
        // trasmetti (lettura>>i)&0x01 che vale 0 o 1 alla pressione

        toneStructVector[i].on = (lettura >> i) & 0x01;
        toneStructVector[i].intensity = 0;
        //printf("nota: %c, premuta: %c", toneStructVector[i].nota,toneStructVector[i].on );
        SendOverSerial(toneStructVector[i]);
      }
    }

    lettura_prec = lettura;

    //_delay_ms(50); // from delay.h, wait 1 sec
  }
}

int Checksum(void *todo, size_t size)
{

  char buffer[size]= todo;
  int i;
  char result = 0;
  for (i = 0; i+3 <size; i+=4)
  {
    /*int a = x & y;
      int b = ~x & ~y;
      int z = ~a & ~b;
      */
    int xor = ~(buffer[i] & buffer[i + 1]) & ~(~buffer[i] & ~buffer[i + 1]);
    result = ~(xor & result) & ~(~xor & ~result);
  }
  for (; i < size; i++)
    result = ~(buffer[i] & result) & ~(~buffer[i] & ~result);
  
  return result;
}

//sendOverìserial test senza puntatore a struct
int SendOverSerial(Tone tone)
{
  char buffer[sizeof(Tone)]; 
  memcpy(buffer,&tone,sizeof(Tone));

  //sinchro
  printf("%c", 0Xaa);
  printf("%c", 0Xbb);

  //struct tone
  int i = 0;
  for (i; i<sizeof(Tone); i++)
    printf("%c", buffer[i]);

  //checksum
  //printf("%c",CheckSum(&tone,sizeof(tone)));

  //connection ended
  printf("%c", 0Xbb);

  return 0;
}
