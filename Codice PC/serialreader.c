#include "serialreader.h"

#define SUCCESS 1
#define WRONG -1

char buffer[sizeof(Tone)];

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
  int all_are_checked;
  int last_is_checked;
  char checksum_saved;
  while (1)
  {
    unsigned char c;
    read(fd, &c, 1);
    all_are_checked = start_checking_param(fd, c);
    if(all_are_checked == SUCCESS){
      checksum_saved = check_savechecksum(fd);
    }
    last_is_checked = check_last_synchro_param(fd);
    if(last_is_checked == SUCCESS){
      char controlla_vero_checksum = checkSum(buffer, sizeof(Tone));
      //printf("ho letto %c , ho calcolato %c\n",checksum_saved,controlla_vero_checksum);
      if(checksum_saved == controlla_vero_checksum){
        Tone *nota = deserialize(buffer);
        printf("La nota è %c\n", nota->nota);
        printf("Il campo on della nota è settato a %d\n", nota->on);
        printf("il campo intensità della nota è settato a %d\n", nota->intensity);
      }
    }
    //printf("pritnf 1 %02x\n", (unsigned int)c);
    //if (c == 0xaa)
    //{
    //printf("entrato in a\n");
    //read(fd, &c, 1);
    //printf("pritnf 2 %02x\n", (unsigned int)c);
    //if (c == 0xbb)
    //{
    //printf("entrato in b\n");
    //read(fd, buffer, sizeof(Tone));
    //read(fd, &c, 1);
    //printf("pritnf 3 %02x\n", (unsigned int)c);
    //if (c == 0xbb)
    //{
    //printf("entrato in c\n");
    //printf("letto %02x\n", (unsigned int)c);
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

int start_checking_param(int fd, unsigned char c){
  //chiamo la prima check che a sua volta chiamerà le successive, passo il valore letto
  //dalla prima read che è 0xaa
  int res = check_first_synchro_param(fd, c);
  if(res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

int check_first_synchro_param(int fd, unsigned char first_synchro_param){
  int res;
  unsigned char next_synchro_param;
  //se leggo 0xaa allora faccio una read per leggere 0xbb e mando il dato letto alla prossima funzione
  if(first_synchro_param == 0xaa){
      read(fd, &next_synchro_param, 1);
      res = check_second_synchro_param(fd, next_synchro_param);
  }
  if(res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

int check_second_synchro_param(int fd, unsigned char second_synchro_param){
  int res;
  //se leggo 0xbb allora chiamo la funzione che si occupa della struct
  if(second_synchro_param == 0xbb){
    res = check_buffer_synchro_param(fd);
  }
  if(res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

int check_buffer_synchro_param(int fd){
  read(fd, buffer, sizeof(Tone));
  return SUCCESS;
}

char check_savechecksum(int fd){
  char checksum;
  read(fd, &checksum, 1);
  return checksum;
}

int check_last_synchro_param(int fd){
  unsigned char last_synchro_param;
  read(fd, &last_synchro_param, 1);
  if(last_synchro_param == 0xbb){
    return SUCCESS;
  }
  return WRONG;
}