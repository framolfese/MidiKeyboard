#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "../CodiceArduino/keyboard.h"

int serial_open(const char* name);

int serial_set_interface_attribs(int fd, int speed, int parity);

void serial_set_blocking(int fd, int should_block);

Tone* deserialize(char* buffer);

