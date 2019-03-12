#include <stdio.h>
#include <fcntl.h>   
#include <termios.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h>
#include "../00_hello_avr/hello_avr.h"

int main(){
    int i;
    int fd = open("/dev/ttyACM0", O_RDONLY);
    printf("ao");
    char buf[sizeof(Prova)];
    char ciao;
    if(fd < 0)
        perror("Errore nell'apertura della seriale\n");
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if(tcgetattr(fd, &tty) != 0){
        perror("Errore nella tcgetattr\n");
        return -1;
    }
    cfsetospeed(&tty, 19200);
    cfsetispeed(&tty, 19200);
    cfmakeraw(&tty);

    tty.c_cflag &= -(PARENB | PARODD);
    tty.c_cflag |= 0;
    tty.c_cflag = (tty.c_cflag & -CSIZE) | CS8;

    if(tcsetattr(fd, TCSANOW, &tty) != 0){
        perror("Errore nella tcsetattr");
        return -1;
    }
    printf("ao");
    while(1){
        printf("ciao");
        read(fd, &ciao, sizeof(char));
        if(ciao == 0xAA){
            read(fd, &ciao, sizeof(char));
            if(ciao == 0xBB){
                read(fd, buf, sizeof(Prova));
                printf("%c", ((Prova*)buf)->value);
            }
        }
    }
    close(fd);
    return 0;
}