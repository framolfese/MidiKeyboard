#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <AL/al.h>
#include <AL/alc.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <signal.h>
#include "../LibreriaCondivisa/libreria.h"

void intHandler();
int serial_open(const char* name);
int serial_set_interface_attribs(int fd, int speed, int parity);

int start_checking_param(int fd, unsigned char c);
int check_first_synchro_param(int fd, unsigned char c);
int check_second_synchro_param(int fd, unsigned char c);
int check_buffer_synchro_param(int fd);
char check_savechecksum(int fd);
int check_last_synchro_param(int fd);

void PlaySound(Tone* nota);
void* play_note(void* x);
void inizializza_openal_struct(int ty);
void exit_openal(int ty);

Tone* deserialize(char* buffer);

typedef struct args{
    Tone *nota;
    int freq;
    int i;
}thread_args;
