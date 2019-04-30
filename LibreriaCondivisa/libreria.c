#include "libreria.h"





unsigned char checkSum(char *buffer, uint8_t size)
{
  //return 0x01;
  //return 0xff;
  //return 0;
  
  //char *buffer = (char *)todo;
  uint8_t i;
  uint8_t result = 'd';
  uint8_t xor= 'q';
  for (i = 0; i<sizeof(Tone); i++)
  {
    /*int a = x & y;
      int b = ~x & ~y;
      int z = ~a & ~b;
    
      */
    xor = ~(buffer[i] & buffer[i + 1]) & ~(~buffer[i] & ~buffer[i + 1]);
    result = ~ (xor&result) & ~(~xor&~result);
  }

  
  return result;
}
