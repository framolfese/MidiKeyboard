#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include "../CodiceArduino/keyboard.h"
#include <math.h>
#include <stdint.h>

#define N 44100
#define M_PI 3.14159265358979323846

void create_sound_file(int16_t nota_da_suonare){
    // Create audio buffer
    int16_t buf[N] = {0}; // buffer
    int i;                // buffer index
    double Fs = 44100.0;  // sampling frequency
     
    // Generate 1 second of audio data - it's just a 1 kHz sine wave, ho rimpiazzato 1000 hz con 262 hz che sarebbe 
    // il numero di hz del Do, ora il file .wav suona il Do per un secondo (essendo hz = 1/fs)
    for (i=0 ; i<N ; ++i) buf[i] = 16383.0 * sin(i*nota_da_suonare*2.0*M_PI/Fs);
     
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout;
    pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - beep.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
}
 
void main()
{
    int16_t nota_do = 262;
    int16_t nota_re = 292;
    int16_t nota_mi = 330;
    int16_t nota_fa = 349;
    create_sound_file(nota_do);
    create_sound_file(nota_re);
    create_sound_file(nota_mi);
    create_sound_file(nota_fa);
}
