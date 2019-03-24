#include "serialreader.h"

int main(int argc, char **argv)
{
  int fd = serial_open("/dev/ttyACM0");
  if (fd < 0)
  {
    return -1;
  }
  int attrib_ok = serial_set_interface_attribs(fd, B19200, 0);
  serial_set_blocking(fd, 1);
  printf("attr: %d\n", attrib_ok);
<<<<<<< HEAD
  unsigned char buffer[sizeof(Tone)];
  while (1)
  {
    unsigned char c;
    read(fd, &c, 1);
    //printf("pritnf 1 %02x\n", (unsigned int)c);
    if (c == 0xaa)
    {
      //printf("entrato in a\n");
      read(fd, &c, 1);
      //printf("pritnf 2 %02x\n", (unsigned int)c);
      if (c == 0xbb)
      {
        //printf("entrato in b\n");
        read(fd, buffer, sizeof(Tone));
        read(fd, &c, 1);
        //printf("pritnf 3 %02x\n", (unsigned int)c);
        if (c == 0xbb)
        {
          //printf("entrato in c\n");
          Tone *nota = deserialize(buffer);
          printf("La nota è %c\n", nota->nota);
          printf("Il campo on della nota è settato a %d\n", nota->on);
          printf("il campo intensità della nota è settato a %d\n", nota->intensity);
        }
        //printf("letto %02x\n", (unsigned int)c);
      }
    }
=======
  while (1){
      unsigned char c ;
      read(fd,&c,1);
      printf("%02x\n",(unsigned char) c);
      if( c == 0xaa) printf("olle");
>>>>>>> 02cb1954f5b4fa20fd0b0922fee59d9f9f19907c
  }
}

int serial_set_interface_attribs(int fd, int speed, int parity)
{
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) != 0)
  {
    printf("error %d from tcgetattr", errno);
    return -1;
  }
  cfsetospeed(&tty, speed);
  cfsetispeed(&tty, speed);
  cfmakeraw(&tty);
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD); // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars

  if (tcsetattr(fd, TCSANOW, &tty) != 0)
  {
    printf("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

void serial_set_blocking(int fd, int should_block)
{
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) != 0)
  {
    printf("error %d from tggetattr", errno);
    return;
  }

  tty.c_cc[VMIN] = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

  if (tcsetattr(fd, TCSANOW, &tty) != 0)
    printf("error %d setting term attributes", errno);
}

int serial_open(const char *name)
{
  int fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0)
  {
    printf("error %d opening serial, fd %d\n", errno, fd);
  }
  return fd;
}

Tone *deserialize(char *buffer)
{
  Tone *curr = (Tone *)buffer;
  return curr;
}