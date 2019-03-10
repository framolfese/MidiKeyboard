    #include <stdio.h>
    #include <stdlib.h>
     
    int main()
    {
       char ch;
       FILE *fp;
     
       fp = fopen("/dev/ttyACM0", "r"); // read mode
     
       if (fp == NULL)
       {
          perror("Error while opening the file.\n");
          exit(EXIT_FAILURE);
       }
     
       while((ch = fgetc(fp)) != EOF)
          printf("%c", ch);
     
       fclose(fp);
       return 0;
    }