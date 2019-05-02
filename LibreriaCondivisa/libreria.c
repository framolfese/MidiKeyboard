#include "libreria.h"

char checkSum(char *buffer, size_t size)
{
  //return 0xff;

  uint8_t sizee=size;
  //char *buffer = (char *)todo;
  uint8_t i;
  char result = 'd';
  char xor= 'q';
  for (i = 0; i< sizee-1; i++)
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
