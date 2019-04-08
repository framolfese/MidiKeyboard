#include "serialreader.h"

#define SUCCESS 1
#define WRONG -1

ALCdevice  * openal_output_device;
ALCcontext * openal_output_context;

ALuint internal_buffer;
ALuint streaming_source[1];

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
        PlaySound(nota);
      }
    }
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
  
  tty.c_cflag &= ~(PARENB | PARODD);
  tty.c_cflag |= parity;
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

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
  tty.c_cc[VTIME] = 5;

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

void PlaySound(Tone* nota){
  unsigned int valore_da_suonare;
  switch (nota->nota)
  {
    case 'D':
      valore_da_suonare = 262;
      break;
    case 'R':
      valore_da_suonare = 300;
      break;
    case 'M':
      valore_da_suonare = 320;
      break;
    case 'F':
      valore_da_suonare = 340;
      break;
    default:
      break;
  }
  inizializza_openal_struct();
  play_note(valore_da_suonare);
}

void inizializza_openal_struct(){
  const char* nome_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

  openal_output_device  = alcOpenDevice(nome_device);
  openal_output_context = alcCreateContext(openal_output_device, NULL);
  alcMakeContextCurrent(openal_output_context);

  alGenBuffers(1, & internal_buffer);
  error_controllore("Errore nella alGenBuffers\n");
}

void play_note(unsigned int nota){
    /* Fill buffer with Sine-Wave */
    // float freq = 440.f;
    float freq = (float)nota;
    //float incr_freq = 0.1f;

    int seconds = 4;
    // unsigned sample_rate = 22050;
    unsigned sample_rate = 44100;
    double my_pi = 3.14159;
    size_t buf_size = seconds * sample_rate;

    // allocate PCM (pulse code modulation) audio buffer      
    short * samples = malloc(sizeof(short) * buf_size);

    printf("here is freq %f\n", freq);
    int i=0;
    for(; i<buf_size; ++i) {
        samples[i] = 32760 * sin( (2.f * my_pi * freq)/sample_rate * i );

        //freq += incr_freq;
        // incr_freq += incr_freq;
        // freq *= factor_freq;

        //if (100.0 > freq || freq > 5000.0) {

            //incr_freq *= -1.0f;
        //}
    }

    /* upload buffer to OpenAL */
    alBufferData( internal_buffer, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
    error_controllore("populating alBufferData");

    free(samples);

    /* Set-up sound source and play buffer */
    // ALuint src = 0;
    // alGenSources(1, &src);
    // alSourcei(src, AL_BUFFER, internal_buffer);
    alGenSources(1, & streaming_source[0]);
    alSourcei(streaming_source[0], AL_BUFFER, internal_buffer);
    // alSourcePlay(src);
    alSourcePlay(streaming_source[0]);

    // ---------------------

    ALenum current_playing_state;
    alGetSourcei(streaming_source[0], AL_SOURCE_STATE, & current_playing_state);
    error_controllore("alGetSourcei AL_SOURCE_STATE");

    while (AL_PLAYING == current_playing_state) {

        printf("still playing ... so sleep\n");

        sleep(1);   // should use a thread sleep NOT sleep() for a more responsive finish

        alGetSourcei(streaming_source[0], AL_SOURCE_STATE, & current_playing_state);
        error_controllore("alGetSourcei AL_SOURCE_STATE");
    }

    printf("end of playing\n");

    /* Dealloc OpenAL */
    exit_openal();
}

int error_controllore(char* errore) {
    ALenum errore_openal;
    errore_openal = alGetError();
    if(AL_NO_ERROR != errore_openal){
        printf("ERROR - %s  (%s)\n", alGetString(errore_openal), errore);
        return errore_openal;
    }
    return 0;
}

void exit_openal(){
    ALenum errorCode = 0;
    // Stop the sources
    alSourceStopv(1, & streaming_source[0]);        //      streaming_source
    int i;
    for (i = 0; i < 1; ++i) {
        alSourcei(streaming_source[i], AL_BUFFER, 0);
    }
    // Clean-up
    alDeleteSources(1, &streaming_source[0]);
    alDeleteBuffers(16, &streaming_source[0]);
    errorCode = alGetError();
    alcMakeContextCurrent(NULL);
    errorCode = alGetError();
    alcDestroyContext(openal_output_context);
    alcCloseDevice(openal_output_device);
}