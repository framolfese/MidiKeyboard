    #include <stdio.h>
    #include <stdlib.h>
    #include "../Codice Arduino/keyboard.h"

     
    int main()
    {
       char ch[sizeof(Tone)];
       FILE *fp;
     
       fp = fopen("/dev/ttyACM0", "r"); // read mode
     
       if (fp == NULL)
       {
          perror("Error while opening the file.\n");
          exit(EXIT_FAILURE);
       }
     
       while(1){
            int i = 0;
            fread(ch,sizeof(Tone),1,fp);
            Tone *test = (Tone*) ch;
            printf("la nota è %c\n",test->value);
            printf("la valore è %d\n",test->marco);           
          
       }
         
     
       fclose(fp);
       return 0;
    }