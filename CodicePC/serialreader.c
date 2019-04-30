#include "serialreader.h"
#include <semaphore.h>
#define SUCCESS 1
#define WRONG -1

ALCdevice *openal_output_device;
ALCcontext *openal_output_context;

ALuint internal_buffer[4];
ALuint streaming_source[4];


//char buffer[sizeof(Tone)];

pthread_t thread[4];
sem_t sema[4];
args arg[4];
char tuttenote[7] = {'D', 'R', 'M', 'F', 'S', 'L', 'i'};
unsigned int freqnote[4] = {262, 293, 329, 349};
unsigned int alive[4] = {0, 0, 0, 0};

int boolean = 0;
int main(int argc, char **argv)
{
  
  char buffer[sizeof(Tone)];
  int all_are_checked;
  int last_is_checked;
  char checksum_saved;
   printf("size of tone : %d\n",sizeof(Tone));
    const char *device = "/dev/ttyACM0";

    int fd = open(device, O_RDONLY);
    if (fd == -1)
    {
        printf("failed to open port\n");
        exit(0);
    }

    struct termios config;

    //
    // Check if the file descriptor is pointing to a TTY device or not.
    //
    if (!isatty(fd))
    { //... error handling ...
        printf("errore riga 27\n");
    }

    //
    // Get the current configuration of the serial interface
    //
    if (tcgetattr(fd, &config) < 0)
    { //... error handling ...
        printf("errore riga 35\n");
    }

    //
    // Input flags - Turn off input processing
    //
    // convert break to null byte, no CR to NL translation,
    // no NL to CR translation, don't mark parity errors or breaks
    // no input parity check, don't strip high bit off,
    // no XON/XOFF software flow control
    //
    //  _delay_ms(1);
    //config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
    //                    INLCR | PARMRK | INPCK | ISTRIP | IXON);

    config.c_iflag &= ~(IGNBRK | PARMRK);
    //

    // Output flags - Turn off output processing
    //
    // no CR to NL translation, no NL to CR-NL translation,
    // no NL to CR translation, no column 0 CR suppression,
    // no Ctrl-D suppression, no fill characters, no case mapping,
    // no local output processing
    //
    //config.c_oflag &= ~( OCRNL | ONLCR | ONLRET |ONOCR| OFILL | OLCUC | OPOST);
    //config.c_oflag = 0;
    config.c_cflag &= (OPOST);
    //config.c_lflag |= ICANON;

    //
    // No line processing
    //
    // echo off, echo newline off, canonical mode off,
    // extended input processing off, signal chars off
    //
    //config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
    config.c_lflag &= ~(ICANON | IEXTEN | ISIG);
    //
    // Turn off character processing
    //
    // clear current char size mask, no parity checking,
    // no output processing, force 8 bit input
    //
    //config.c_cflag &= ~(CSIZE | PARENB);
    //config.c_cflag |= CS8;
    config.c_cflag &= ~(PARENB);
    config.c_cflag |= CS8;
    //
    // One input byte is enough to return from read()
    // Inter-character timer off
    //
    config.c_cc[VMIN] = 2;
    //config.c_cc[VMIN] = 1;
    config.c_cc[VTIME] = 0;

    //
    // Communication speed (simple version, using the predefined
    // constants)
    //
    if (cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0)
    {
        printf("errore riga 91\n"); // ... error handling ...
    }

    //
    // Finally, apply the configuration
    //
    if (tcsetattr(fd, TCSAFLUSH, &config) < 0)
    { //... error handling ...
        printf("errore riga 99\n");
    }



  //inizio ricezione pacchetti dalla seriale
  unsigned char rr;
  unsigned char ck;
  int popo;
  while (1)
  {

    
    popo= read(fd, &rr, 1);
    if(popo<0) printf("XXXXXXXerrore prima read\n");
    if (rr == 0x55)
    {
      popo= read(fd, &rr, 1);
      if(popo<0) printf("XXXXXXXerrore seconda read\n");
      if (rr == 0xaa)
      {
        popo= read(fd, buffer, 2);
        if(popo<0) printf("XXXXXXXerrore terza read\n");
        /*
        popo= read(fd, &ck, 1);
        if(popo<0) printf("XXXXXXXerrore quarta read\n");
        printf("ck pre check aa: %c\n",ck);
        */
        popo= read(fd, &rr, 1);
        if(popo<0) printf("XXXXXXXerrore quita read\n");
        if (rr == 0xaa)
        {
          //printf("abbiamo letto ocn successo\n");

          //unsigned char ckr = checkSum(buffer, sizeof(Tone));
          //printf("mio checksum :%c tuo checksum: %c\n", ck, ckr);
          if(1) //(ck == ckr)
          {
            Tone* ciao= malloc(sizeof(Tone));// = buffer;
            memcpy(ciao,buffer,sizeof(Tone));
            printf("letta nota : %c , on: %c  \n",ciao->nota,ciao->on);
            PlaySound(ciao);
            ck = '0';
            ck = '0';
          }
        }
        else
        {
          printf("errore nella lettura del aa finale ho letto:%u    \n",rr);
          //printf("ho letto:%c\n",rr);
          
        }
      }
      else
      {
        printf("errore nella lettura del primo  bb\n");
        
      }
    }
    else
    {
      printf("errore nella lettura del primo aa\n");

    }

  }
  exit_Openal();
  return 0;

  unsigned char c;
  read(fd, &c, 1);
  //prima read, inizio sincronizzazione
  all_are_checked = start_checking_param(fd, c);
  if (all_are_checked == SUCCESS)
  {
    //se la sincronizzazione è avvenuta, procedo leggendo il checksum
    checksum_saved = check_savechecksum(fd);
  }
  last_is_checked = check_last_synchro_param(fd);
  //se anche l'ultimo paramentoro della sincronizzazione è giusto, capisco che
  //quello che ho ricevuto è il valore effettivo di una struct, ma prima controllo se
  //il mio checksum e quello ricevuto dalla seriale sono coerenti
  if (last_is_checked == SUCCESS)
  {
    char controlla_vero_checksum = checkSum(buffer, sizeof(Tone));
    printf(" mio: %c tuo:%d\n", controlla_vero_checksum, checksum_saved);
    if (checksum_saved == controlla_vero_checksum)
    {
      //checksum controllato con successo, procedo a deserializzare il pacchetto e a
      //valutarne i campi
      Tone *nota = deserialize(buffer);
      printf("La nota è %c\n", nota->nota);
      printf("Il campo on della nota è settato a %d\n", nota->on);
      //printf("il campo intensità della nota è settato a %d\n", nota->intensity);
      //una volta resi noti i valori, procedo alla produzione del pcm
      PlaySound(nota);
    }
  }
}

//setto attributi seriale
int serial_set_interface_attribs(int fd, int speed, int parity)
{
  if(!isatty(fd)){
    printf("MEGA ERROR ODDDIO\n");
    exit(-1);
  }
  struct termios tty;
  memset(&tty, 0, sizeof tty);
  if (tcgetattr(fd, &tty) < 0)
  {
    printf("error %d from tcgetattr", errno);
    return -1;
  }
  if(cfsetispeed(&tty, B19200) < 0 || cfsetospeed(&tty, B19200) < 0) {
    printf("MEGA ERROR ODDDIO della velocity\n");
    return -1;
 }
  //old
  //cfsetospeed(&tty, speed);
  //cfsetispeed(&tty, speed);
  cfmakeraw(&tty);

  tty.c_cflag &= ~(PARENB | PARODD);

  //test
  /*
  tty.c_oflag = 0; 
  tty.c_oflag &= ~(OCRNL | ONLCR | ONLRET |ONOCR | ENOENT | OFILL | OLCUC | OPOST);
  
  tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
  tty.c_oflag &= ~OPOST;
  tty.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);
  tty.c_cflag &= ~(CSIZE | PARENB);
 tty.c_cflag |= CS8;
  tty.c_cc[VMIN]  = 1;
 tty.c_cc[VTIME] = 0;
  */
  //old
  tty.c_cflag |= parity;
  

  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
  //tty.c_cflag &= (tty.c_cflag & ~CSIZE) | CS8;

  //test
  /* fetch bytes as they become available */
  tty.c_cc[VMIN] = 2;
  tty.c_cc[VTIME] = 1;
  //



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
int start_checking_param(int fd, unsigned char c)
{
  int res = check_first_synchro_param(fd, c);
  if (res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//se leggo 0xaa allora faccio una read per leggere 0xbb e mando il dato letto alla prossima funzione
int check_first_synchro_param(int fd, unsigned char first_synchro_param)
{
  int res;
  unsigned char next_synchro_param;
  if (first_synchro_param == 0xaa)
  {
    read(fd, &next_synchro_param, 1);
    res = check_second_synchro_param(fd, next_synchro_param);
  }
  if (res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//se leggo 0xbb allora chiamo la funzione che si occupa della struct
int check_second_synchro_param(int fd, unsigned char second_synchro_param)
{
  int res;
  if (second_synchro_param == 0xbb)
  {
    res = check_buffer_synchro_param(fd);
  }
  if (res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//prendo il buffer contenente la struct
int check_buffer_synchro_param(int fd)
{
  read(fd, buffer, sizeof(Tone));
  return SUCCESS;
}

//salvo il checksum ricevuto dalla seriale
char check_savechecksum(int fd)
{
  char checksum;
  read(fd, &checksum, 1);
  return checksum;
}

//controllo l'ultimo paramentro di sincronizzazione con la seriale
int check_last_synchro_param(int fd)
{
  unsigned char last_synchro_param;
  read(fd, &last_synchro_param, 1);
  if (last_synchro_param == 0xbb)
  {
    return SUCCESS;
  }
  return WRONG;
}

//start pthread

void startPtrhead(int i)
{
  int r = i;
  /*int r;
    for(r=1;r<4;r++){  
      sleep(4);  
      //arg[r].nota=tuttenote[r];
      arg[r].val= freqnote[r] ;
      arg[r].i = r;
      pthread_create(&thread[r], NULL, play_note, &arg[r]);
      //play_note(nota, valore_da_suonare);
    }
    */

  arg[r].val = freqnote[r];
  arg[r].i = r;
  pthread_create(&thread[r], NULL, play_note, &arg[r]);
}

//definizione della frequenza della nota da suonare
//e chiamata a funzione per inizializzazione delle struct OpenAL
//infine chiamata per suonare la nota effettiva
void PlaySound(Tone *nota)
{

  unsigned int valore_da_suonare;
  int i;
  switch (nota->nota)
  {
  case 'D':
    valore_da_suonare = 262;
    i = 0;
    break;
  case 'R':
    valore_da_suonare = 293;
    i = 1;
    break;
  case 'M':
    i = 2;
    valore_da_suonare = 329;
    break;
  case 'F':
    valore_da_suonare = 349;
    i = 3;
    break;
  default:
    break;
  }
  if (nota->on == 1)
  {
    //pthread_kill(thread[i],SIGTERM);
    //int k = pthread_kill(thread[i],0);
    //if((alive[i]==0)) return;
    //pthread_kill(thread[i],SIGTERM); //pthread_kill(thread[i],SIGKILL);
    alive[i] = 0;
    //pthread_cancel(thread[i]);
    pthread_join(thread[i], NULL);
  }
  else
  {
    if (alive[i] == 1)
    {
      alive[i] = 0;
      pthread_join(thread[i], NULL);
      printf("mi hai chiesto di suonare una nota che sta già suonando!\n");
      printf("immagino ci sia stato un erroe nella comunicazione!\n");
      return;
    }
    alive[i] = 1;
    startPtrhead(i);
  }
  //play_note(nota, valore_da_suonare);
}

//inizializzazione struct OpenAL
void inizializza_openal_struct()
{
  const char *nome_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

  openal_output_device = alcOpenDevice(nome_device);
  openal_output_context = alcCreateContext(openal_output_device, NULL);
  alcMakeContextCurrent(openal_output_context);

  alGenBuffers(1, &internal_buffer[0]);
  alGenBuffers(1, &internal_buffer[1]);
  alGenBuffers(1, &internal_buffer[2]);
  alGenBuffers(1, &internal_buffer[3]);
  error_controllore("Errore nella alGenBuffers\n");
}

//Tone* nota, unsigned int valore_da_suonare
void *play_note(void *x)
{
  args *arg = (args *)x;
  unsigned int valore_da_suonare = arg->val;
  unsigned int ty = arg->i;

  exit_openal(ty);
  alGenBuffers(1, &internal_buffer[0]);
  //inizializza_openal_struct();
  //inizializza_openal_struct(ty);
  //alGenBuffers(1, &internal_buffer[ty]);

  //prendo il valore della nota e lo converto a float per generare
  //un'onda sinusoidale
  float freq = (float)valore_da_suonare;
  //float incr_freq = serve per incrementare la frequenza, usato
  //quando il tasto Intensity è premuto
  int seconds = 10; //numero di secondi massimo quale la nota può venire suonata
  // unsigned sample_rate = 22050, valore a caso
  unsigned sample_rate = 44100;
  double my_pi = 3.14159;
  size_t buf_size = seconds * sample_rate;
  // allocate PCM (pulse code modulation) audio buffer
  short *samples = malloc(sizeof(short) * buf_size);

  //printf("La frequenza della nota è %f\n", freq);
  int i = 0;
  for (; i < buf_size; ++i)
  {
    samples[i] = 32760 * sin((2.f * my_pi * freq) / sample_rate * i);
  }
  //printf("onda riempita\n");

  //carico il buffer con OpenAL
  alBufferData(internal_buffer[ty], AL_FORMAT_MONO16, samples, buf_size, sample_rate);
  error_controllore("populating alBufferData");

  free(samples);
  //printf("%d\n",internal_buffer[ty]);
  // inizializzazione della sorgente del suono e play del buffer
  // ALuint src = 0;
  // alGenSources(1, &src);
  // alSourcei(src, AL_BUFFER, internal_buffer);
  alGenSources(1, &streaming_source[ty]);
  //if(nota->on == 0){
  //alSourcei(streaming_source[0], AL_LOOPING, 1);
  //}
  //else{
  //alSourcei(streaming_source[0], AL_BUFFER, internal_buffer);
  //}
  alSourcei(streaming_source[ty], AL_BUFFER, internal_buffer[ty]);
  // alSourcePlay(src);
  //free(arg);

  alSourcePlay(streaming_source[ty]);

  //alSourcei(streaming_source[ty], AL_LOOPING, 1);// questo è per il loop
  // ---------------------

  ALenum current_playing_state;
  alGetSourcei(streaming_source[ty], AL_SOURCE_STATE, &current_playing_state);
  error_controllore("alGetSourcei AL_SOURCE_STATE");

  while (AL_PLAYING == current_playing_state)
  {
    if (alive[ty] == 0)
    {
      //printf("sono il figlio :%d e mihanno detto che devo morire\n",ty);
      // exit_openal(ty);
      alSourceStop(streaming_source[ty]);
      //pthread_exit(NULL);
      //break;
      alive[ty] = 0;

      pthread_exit(NULL);
      break;
    }
    //printf("still playing ... so sleep\n");

    //sleep(1);   // should use a thread sleep NOT sleep() for a more responsive finish

    alGetSourcei(streaming_source[ty], AL_SOURCE_STATE, &current_playing_state);
    error_controllore("alGetSourcei AL_SOURCE_STATE");
  }

  //int k = 0;

  printf("end of playing\n");

  //dealloco la struct OpenAL
  alive[ty] = 0;

  pthread_exit(NULL);
}

int error_controllore(char *errore)
{
  ALenum errore_openal;
  errore_openal = alGetError();
  if (AL_NO_ERROR != errore_openal)
  {
    printf("ERROR - %s  (%s)\n", alGetString(errore_openal), errore);
    return errore_openal;
  }
  return 0;
}

void exit_openal(int ty)
{
  ALenum errorCode = 0;
  // Stoppo la sorgente
  alSourceStop(streaming_source[ty]);
  alSourceStopv(1, &streaming_source[ty]);
  //printf("1\n");

  //alSourcei(streaming_source[ty], AL_BUFFER, 0);

  // Chiudo tutto
  alDeleteSources(1, &streaming_source[ty]);
  //printf("2\n");
  alDeleteBuffers(16, &streaming_source[ty]);
  //printf("3\n");
  errorCode = alGetError();
  //printf("4\n");
  //alcMakeContextCurrent(NULL);
  //printf("5\n");
  //errorCode = alGetError();
  //printf("6\n");
  //alcDestroyContext(openal_output_context);
  //printf("7\n");
  //alcCloseDevice(openal_output_device);
  //printf("8\n");
}

void exit_Openal()
{
  ALenum errorCode = 0;
  int ty = 0;
  for (; ty < 4; ty++)
  {
    alSourceStopv(1, &streaming_source[ty]);
    alDeleteSources(1, &streaming_source[ty]);
    alDeleteBuffers(16, &streaming_source[ty]);
    errorCode = alGetError();
  }
  alcMakeContextCurrent(NULL);

  errorCode = alGetError();
  alcDestroyContext(openal_output_context);
  alcCloseDevice(openal_output_device);
}
