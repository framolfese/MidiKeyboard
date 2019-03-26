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
 
void main()
{
    // Create audio buffer
    int16_t buf[N] = {0}; // buffer
    int n;                // buffer index
    double Fs = 44100.0;  // sampling frequency
     
    // Generate 1 second of audio data - it's just a 1 kHz sine wave
    for (n=0 ; n<N ; ++n) buf[n] = 16383.0 * sin(n*1000.0*2.0*M_PI/Fs);
     
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout;
    pipeout = popen("ffmpeg -y -f s16le -ar 44100 -ac 1 -i - beep.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
}