#include <keyboard.h>
#include <util/delay.h>
#include <avr/io.h>
uint16_t globalSize;

int main(void)
{
  globalSize= sizeof(Tone);
  printf_init();

  uint8_t mask;
  mask = 0x3C;
  DDRF &= ~mask;
  PORTF |= mask;


  //modificare qua per cambiare le note da eseguire
  Tone toneStructVector[8];
  toneStructVector[2].nota = 349;
  toneStructVector[2].on = 0;
  toneStructVector[2].tasto=0;

  toneStructVector[3].nota = 329;
  toneStructVector[3].on = 0;
  toneStructVector[3].tasto=1;

  toneStructVector[4].nota = 293;
  toneStructVector[4].on = 0;
  toneStructVector[4].tasto=2;

  toneStructVector[5].nota = 262;
  toneStructVector[5].on = 0;
  toneStructVector[5].tasto=3;


  uint8_t lettura, lettura_prec;
  lettura = PINF & mask;
  lettura_prec = lettura;

  while (1)
  {
    lettura = PINF & mask;

    for (int i = 2; i < 6; ++i)
    {
      if (((lettura >> i) & 0x1) != ((lettura_prec >> i) & 0x1))
      {
        uint8_t ca = (lettura >> i) & 0x01;
        if(ca!=1 ) toneStructVector[i].on=0;
        else toneStructVector[i].on=1 ;
        sendoverserial(toneStructVector[i]);
      }
    }

    lettura_prec = lettura;

    _delay_ms(25);
  }
}

int sendoverserial(Tone tone)
{  
  char buffer[sizeof(Tone)+2];
  memcpy(buffer, &tone, sizeof(Tone));
                                                          
  //inizio sincronizzazione
  _delay_ms(1);
  printf("%c", (unsigned char)0X55);
  _delay_ms(1);
  printf("%c", (unsigned char)0Xaa);
  //mandati primi due parametri, mando il buffer
  int i;
  for (i = 0; i < sizeof(Tone); ++i){
     usart_putchar(buffer[i]);   
  }    
  //mando il checksum
  unsigned char pippo = checkSum(buffer,sizeof(Tone));
  printf("%c",pippo);
  _delay_ms(1);
  //ultimo parametro
  printf("%c", (unsigned char)0Xaa);

  return 0;
}
