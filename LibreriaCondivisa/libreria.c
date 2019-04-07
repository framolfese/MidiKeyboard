#include "libreria.h"





unsigned char checkSum(char *buffer, size_t size)
{

  //char *buffer = (char *)todo;
  int i;
  unsigned char result = 0;
  unsigned char xor=0;
  for (i = 0; i< size; i++)
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
