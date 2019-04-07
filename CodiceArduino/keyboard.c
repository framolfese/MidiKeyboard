#include <keyboard.h>
#include <util/delay.h>
#include <avr/io.h>


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
        sendoverserial(toneStructVector[i]);
      }
    }

    lettura_prec = lettura;

    //_delay_ms(50); // from delay.h, wait 1 sec
  }
}



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
  unsigned char pippo = checkSum(buffer,sizeof(Tone));
  //printf("%c",pippo);

  //connection ended
  printf("%c", (unsigned char)0Xbb);

  return 0;
}
