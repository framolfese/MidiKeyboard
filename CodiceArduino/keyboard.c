#include <keyboard.h>
#include <util/delay.h>
#include <avr/io.h>
#include "../avr_common/uart.h" // this includes the printf and initializes it

int main(void)
{
  // this initializes the printf/uart thingies
  printf_init();

  uint8_t mask; //(1<<2);ra_prec;
  
  //int i;
  mask = 0x3C;
  DDRF &= ~mask;
  PORTF |= mask;

  //initialize the ToneStruc Vector
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
  toneStructVector[5].on =0;
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

        toneStructVector[i].on = !toneStructVector[i].on;
        toneStructVector[i].intensity = 0;
        //printf("nota: %c, premuta: %d", toneStructVector[i].nota,toneStructVector[i].on );
        SendOverSerial(&toneStructVector[i]);
      }
    }

    lettura_prec = lettura;

    //_delay_ms(50); // from delay.h, wait 1 sec
  }
}

/*char *serialize(void *s, int size)
{
  char *buffer = malloc(size);
  memcpy(buffer, s, size);
  return buffer;
}
*/
int checksum(void *buf)
{
  return 0;
}

int SendOverSerial(Tone *tone)
{

  //unsigned char *buffer = serialize(tone, sizeof(tone));
  unsigned char  buffer[sizeof(Tone)];
  memcpy(&buffer,tone, sizeof(Tone));

  //sinchro
  printf("%c", (unsigned char)0XAA);
  printf("%c", (unsigned char)0XBB);

  //struct tone
  int i = 0;
  //for (i = sizeof(Tone) - 1; i >= 0; i--)
  for(i = 0;i<sizeof(Tone);i++)  printf("%c", (unsigned char)buffer[i]);

  //checksum
  //printf("%c",checksum(buffer));

  //connection ended
  printf("%c", (unsigned char)0XBB);

  return 0;
}
