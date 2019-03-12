#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "../00_hello_avr/hello_avr.h"

int main()
{
   char buf[sizeof(Prova)];
   char ciao;
   FILE *fp;

   fp = fopen("/dev/ttyACM0", "r+"); // read mode

   if (fp == NULL)
   {
      perror("Error while opening the file.\n");
      return -1;
   }
   printf("ei");
   while (1)
   {
      printf("ciao\n");
      ciao = (char)fgetc(fp);
      if (ciao == 0xAA)
      {
         printf("ciao\n");
         ciao = (char)fgetc(fp);
         if (ciao == 0xBB)
         {
            printf("ci siamo");
            fread(buf, sizeof(Prova), 1, fp);
            printf("%c\n", ((Prova *)buf)->value);
         }
      }
   }

   fclose(fp);
   return 0;
}