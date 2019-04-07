#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
//#include "../CodiceArduino/keyboard.h"
#include "../LibreriaCondivisa/libreria.h"


int serial_open(const char* name);


int serial_set_interface_attribs(int fd, int speed, int parity);

void serial_set_blocking(int fd, int should_block);

int start_checking_param(int fd, unsigned char c);
int check_first_synchro_param(int fd, unsigned char c);
int check_second_synchro_param(int fd, unsigned char c);
int check_buffer_synchro_param(int fd);
char check_savechecksum(int fd);
int check_last_synchro_param(int fd);

Tone* deserialize(char* buffer);



