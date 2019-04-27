#include "serialreader.h"

#define SUCCESS 1
#define WRONG -1

ALCdevice* openal_output_device;
ALCcontext* openal_output_context;

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
  //inizio a settare i parametri per la comunicazione con la seriale
  int attrib_ok = serial_set_interface_attribs(fd, B19200, 0);
  serial_set_blocking(fd, 1);
  printf("attr: %d\n", attrib_ok);
  int all_are_checked;
  int last_is_checked;
  char checksum_saved;
  //inizio ricezione pacchetti dalla seriale
  while (1)
  {
    unsigned char c;
    read(fd, &c, 1);
    //prima read, inizio sincronizzazione
    all_are_checked = start_checking_param(fd, c);
    if(all_are_checked == SUCCESS){
      //se la sincronizzazione è avvenuta, procedo leggendo il checksum
      checksum_saved = check_savechecksum(fd);
    }
    last_is_checked = check_last_synchro_param(fd);
    //se anche l'ultimo paramentoro della sincronizzazione è giusto, capisco che 
    //quello che ho ricevuto è il valore effettivo di una struct, ma prima controllo se
    //il mio checksum e quello ricevuto dalla seriale sono coerenti
    if(last_is_checked == SUCCESS){
      char controlla_vero_checksum = checkSum(buffer, sizeof(Tone));
      if(checksum_saved == controlla_vero_checksum){
        //checksum controllato con successo, procedo a deserializzare il pacchetto e a 
        //valutarne i campi
        Tone *nota = deserialize(buffer);
        printf("La nota è %c\n", nota->nota);
        printf("Il campo on della nota è settato a %d\n", nota->on);
        printf("il campo intensità della nota è settato a %d\n", nota->intensity);
        //una volta resi noti i valori, procedo alla produzione del pcm
        PlaySound(nota);
      }
    }
  }
}

//setto attributi seriale
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

//blocking/non-blocking
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

//apertura seriale
int serial_open(const char *name)
{
  int fd = open(name, O_RDWR | O_NOCTTY | O_SYNC);
  if (fd < 0)
  {
    printf("error %d opening serial, fd %d\n", errno, fd);
  }
  return fd;
}

//deserializzazione pacchetto contenente la struct d'interesse
Tone *deserialize(char *buffer)
{
  Tone *curr = (Tone *)buffer;
  return curr;
}

//chiamo la prima check che a sua volta chiamerà le successive, passo il valore letto
//dalla prima read che è 0xaa
int start_checking_param(int fd, unsigned char c){
  int res = check_first_synchro_param(fd, c);
  if(res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//se leggo 0xaa allora faccio una read per leggere 0xbb e mando il dato letto alla prossima funzione
int check_first_synchro_param(int fd, unsigned char first_synchro_param){
  int res;
  unsigned char next_synchro_param;
  if(first_synchro_param == 0xaa){
      read(fd, &next_synchro_param, 1);
      res = check_second_synchro_param(fd, next_synchro_param);
  }
  if(res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//se leggo 0xbb allora chiamo la funzione che si occupa della struct
int check_second_synchro_param(int fd, unsigned char second_synchro_param){
  int res;
  if(second_synchro_param == 0xbb){
    res = check_buffer_synchro_param(fd);
  }
  if(res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//prendo il buffer contenente la struct
int check_buffer_synchro_param(int fd){
  read(fd, buffer, sizeof(Tone));
  return SUCCESS;
}

//salvo il checksum ricevuto dalla seriale
char check_savechecksum(int fd){
  char checksum;
  read(fd, &checksum, 1);
  return checksum;
}

//controllo l'ultimo paramentro di sincronizzazione con la seriale
int check_last_synchro_param(int fd){
  unsigned char last_synchro_param;
  read(fd, &last_synchro_param, 1);
  if(last_synchro_param == 0xbb){
    return SUCCESS;
  }
  return WRONG;
}

//definizione della frequenza della nota da suonare 
//e chiamata a funzione per inizializzazione delle struct OpenAL
//infine chiamata per suonare la nota effettiva
void PlaySound(Tone* nota){
  unsigned int valore_da_suonare;
  switch (nota->nota)
  {
    case 'D':
      valore_da_suonare = 262;
      break;
    case 'R':
      valore_da_suonare = 293;
      break;
    case 'M':
      valore_da_suonare = 329;
      break;
    case 'F':
      valore_da_suonare = 349;
      break;
    default:
      break;
  }
  inizializza_openal_struct();
  play_note(nota, valore_da_suonare);
}

//inizializzazione struct OpenAL
void inizializza_openal_struct(){
  const char* nome_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

  openal_output_device  = alcOpenDevice(nome_device);
  openal_output_context = alcCreateContext(openal_output_device, NULL);
  alcMakeContextCurrent(openal_output_context);

  alGenBuffers(1, & internal_buffer);
  error_controllore("Errore nella alGenBuffers\n");
}


void play_note(Tone* nota, unsigned int valore_da_suonare){
    //prendo il valore della nota e lo converto a float per generare
    //un'onda sinusoidale 
    float freq = (float)valore_da_suonare;
    //float incr_freq = serve per incrementare la frequenza, usato
    //quando il tasto Intensity è premuto
    int seconds = 1;
    // unsigned sample_rate = 22050, valore a caso
    unsigned sample_rate = 44100;
    double my_pi = 3.14159;
    size_t buf_size = seconds * sample_rate;
    // allocate PCM (pulse code modulation) audio buffer      
    short * samples = malloc(sizeof(short) * buf_size);

    printf("La frequenza della nota è %f\n", freq);
    int i=0;
    for(; i<buf_size; ++i) {
        samples[i] = 32760 * sin( (2.f * my_pi * freq)/sample_rate * i );
    }

    //carico il buffer con OpenAL
    alBufferData( internal_buffer, AL_FORMAT_MONO16, samples, buf_size, sample_rate);
    error_controllore("populating alBufferData");

    free(samples);

    // inizializzazione della sorgente del suono e play del buffer
    // ALuint src = 0;
    // alGenSources(1, &src);
    // alSourcei(src, AL_BUFFER, internal_buffer);
    alGenSources(1, &streaming_source[0]);
    //if(nota->on == 0){
      //alSourcei(streaming_source[0], AL_LOOPING, 1);
    //}
    //else{
      //alSourcei(streaming_source[0], AL_BUFFER, internal_buffer);
    //}
    alSourcei(streaming_source[0], AL_BUFFER, internal_buffer);
    // alSourcePlay(src);
    alSourcePlay(streaming_source[0]);
    //alSourcei(streaming_source[0], AL_LOOPING, 1); questo è per il loop

    ALenum current_playing_state; //vediamo se attualmente sta suonando
    alGetSourcei(streaming_source[0], AL_SOURCE_STATE, &current_playing_state);
    error_controllore("alGetSourcei AL_SOURCE_STATE");

    int k = 0;
    while (AL_PLAYING == current_playing_state) {

        printf("still playing ... so sleep\n");

        sleep(1);   // should use a thread sleep NOT sleep() for a more responsive finish

        alGetSourcei(streaming_source[0], AL_SOURCE_STATE, &current_playing_state);
        error_controllore("alGetSourcei AL_SOURCE_STATE");
    }

    printf("end of playing\n");

    //dealloco la struct OpenAL
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
    // Stoppo la sorgente 
    alSourceStopv(1, &streaming_source[0]);
    int i;
    for (i = 0; i < 1; ++i) {
        alSourcei(streaming_source[i], AL_BUFFER, 0);
    }
    // Chiudo tutto
    alDeleteSources(1, &streaming_source[0]);
    alDeleteBuffers(16, &streaming_source[0]);
    errorCode = alGetError();
    alcMakeContextCurrent(NULL);
    errorCode = alGetError();
    alcDestroyContext(openal_output_context);
    alcCloseDevice(openal_output_device);
}
