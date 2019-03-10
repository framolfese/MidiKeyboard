#include <stdio.h> 
#include <stdlib.h> 

int main() {
    FILE* seriale = fopen("/dev/ttyACM0", "r"); 
    if (seriale == NULL) { 
        printf("Couldn't open file\n"); 
        exit(0); 
    }
    while(1){
        int parola = fgetc(seriale);
        printf("%d\n", parola);
    }
    fclose(seriale);
    return 0;
}