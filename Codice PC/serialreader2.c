#include <stdio.h>
#include <fcntl.h>   
#include <termios.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h>

int main(){
    int fd = open("/dev/ttyACM0", O_RDONLY);
    char buf[128] = {0};
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
    int n = read(fd, buf, 128);
    close(fd);
    return 0;
}