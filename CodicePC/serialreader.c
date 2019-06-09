#include "serialreader.h"

#define SUCCESS 1
#define WRONG -1

ALCdevice *openal_output_device;
ALCcontext *openal_output_context[4];

ALuint internal_buffer[4];
ALuint streaming_source[4];
ALenum current_playing_state[4];

char buffer[sizeof(Tone)];

pthread_t threads[4];
sem_t mysem;

int alive[4] = {0, 0, 0, 0}; //poichè abbiamo solo 4 tasti

int main(int argc, char **argv)
{

  sem_init(&mysem, 0, 1);
  signal(SIGINT, intHandler);

  int fd = serial_open("/dev/ttyACM0");
  if (fd < 0)
  {
    return -1;
  }
  //inizio a settare i parametri per la comunicazione con la seriale
  int attrib_ok = serial_set_interface_attribs(fd, B19200, 0);
  if (attrib_ok != 0)
    printf("attr: %d\n", attrib_ok);
  int all_are_checked;
  int last_is_checked;
  char checksum_saved;
  //inizio ricezione pacchetti dalla seriale

  printf("PROGETTO SISTEMI OPERATIVI 2018/2019\n");
  usleep(1000000);
  printf("NOME PROGETTO: MIDI KEYBOARD\n");
  usleep(1000000);
  printf("COMPONENTI DEL GRUPPO:\n");
  usleep(1000000);
  printf("FRANCESCO DOUGLAS SCOTTI DI VIGOLENO\n");
  usleep(1000000);
  printf("FRANCESCO MOLFESE\n");
  usleep(1000000);
  printf("NOTE DISPONIBILI: D,R,M,F\n");
  usleep(1000000);
  printf("FREQUENZE: DO = 262 Hz, RE = 293 Hz, MI = 329 Hz, FA = 349 Hz\n");
  usleep(1000000);
  printf("PREMI PER UN TASTO SULLA BOARD PER INZIARE, CTRL-C PER TERMINARE\n\n\n");

  while (1)
  {
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
    //se anche l'ultimo parametro della sincronizzazione è giusto, capisco che
    //quello che ho ricevuto è il valore effettivo di una struct, ma prima controllo se
    //il mio checksum e quello ricevuto dalla seriale sono coerenti
    if (last_is_checked == SUCCESS)
    {
      char controlla_vero_checksum = checkSum(buffer, sizeof(Tone));
      if (checksum_saved == controlla_vero_checksum)
      {
        //checksum controllato con successo, procedo a deserializzare il pacchetto e a
        //valutarne i campi
        Tone *nota = deserialize(buffer);
        system("clear");
        printf("PROGETTO SISTEMI OPERATIVI 2018/2019\n");
        printf("NOME PROGETTO: MIDI KEYBOARD\n");
        printf("COMPONENTI DEL GRUPPO:\n");
        printf("FRANCESCO DOUGLAS SCOTTI DI VIGOLENO\n");
        printf("FRANCESCO MOLFESE\n");
        printf("NOTE DISPONIBILI: D,R,M,F\n");
        printf("FREQUENZE: DO = 262 Hz, RE = 293 Hz, MI = 329 Hz, FA = 349 Hz\n");
        printf("PREMI PER UN TASTO SULLA BOARD PER INZIARE, CTRL-C PER TERMINARE\n\n\n");
        printf("La frequenza della nota che sta suonando è %d\n", nota->nota);
        printf("Il campo on della nota è settato a %d\n", nota->on);
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

  tty.c_iflag &= ~(IGNBRK | PARMRK);
  tty.c_cflag &= OPOST;
  tty.c_lflag &= ~(ICANON | IEXTEN | ISIG);
  tty.c_cflag &= ~(PARENB);
  tty.c_cflag |= CS8;
  //tty.c_cc[VMIN] = 2;
  tty.c_cc[VMIN] = sizeof(Tone);

  if (tcsetattr(fd, TCSAFLUSH, &tty) != 0)
  {
    printf("error %d from tcsetattr", errno);
    return -1;
  }
  return 0;
}

//apertura seriale
int serial_open(const char *name)
{
  int fd = open(name, O_RDONLY);
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
//dalla prima read che è 0x55
int start_checking_param(int fd, unsigned char c)
{
  int res = check_first_synchro_param(fd, c);
  if (res == SUCCESS)
  {
    return SUCCESS;
  }
  return WRONG;
}

//se leggo 0x55 allora faccio una read per leggere 0xaa e mando il dato letto alla prossima funzione
int check_first_synchro_param(int fd, unsigned char first_synchro_param)
{
  int res;
  unsigned char next_synchro_param;
  if (first_synchro_param == 0x55)
  {
    read(fd, &next_synchro_param, 1);
    res = check_second_synchro_param(fd, next_synchro_param);
  }
  if (res == SUCCESS)
    return SUCCESS;
  return WRONG;
}

//se leggo 0xaa allora chiamo la funzione che si occupa della struct
int check_second_synchro_param(int fd, unsigned char second_synchro_param)
{
  int res;
  if (second_synchro_param == 0xaa)
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
  if (last_synchro_param == 0xaa)
  {
    return SUCCESS;
  }
  return WRONG;
}

//definizione della frequenza della nota da suonare
//e chiamata a funzione per inizializzazione delle struct OpenAL
//infine chiamata per suonare la nota effettiva
void PlaySound(Tone *nota)
{
  int i = nota->tasto;

  if (nota->on == 1)
  {
    alive[i] = 0;
    pthread_join(threads[i], NULL);
  }
  else
  {
    if (alive[i] == 1)
    {
      alive[i] = 0;
      pthread_join(threads[i], NULL);
      printf("mi hai chiesto di suonare una nota che sta già suonando!\n");
      printf("immagino ci sia stato un erroe nella comunicazione!\n");
      return;
    }
    alive[i] = 1;
    thread_args *arg = malloc(sizeof(thread_args));
    arg->nota = nota;
    //arg->freq = freq_da_suonare;
    arg->freq = nota->nota;
    arg->i = i;
    printf("aggiornato il thread:%d\n", i);
    pthread_create(&threads[i], NULL, &play_note, arg);
  }
}

//inizializzazione struct OpenAL
void inizializza_openal_struct(int ty)
{
  const char *nome_device = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);

  openal_output_device = alcOpenDevice(nome_device);
  openal_output_context[ty] = alcCreateContext(openal_output_device, NULL);
  alcMakeContextCurrent(openal_output_context[ty]);

  alGenBuffers(1, &internal_buffer[ty]);
}

//creazione del pcm da parte del thread i-esimo
void *play_note(void *argomenti_thread)
{
  sem_wait(&mysem);
  thread_args *arg = (thread_args *)argomenti_thread;
  int freq_da_suonare = arg->freq;
  int ty = arg->i;

  inizializza_openal_struct(ty);
  //ty è il numero del thread i-esimo associato alla nota i-esima
  //prendo il valore della nota e lo converto a float per generare
  //un'onda sinusoidale
  float freq = (float)freq_da_suonare;
  int seconds = 10; //per avere note infinite basta mettere un altro while prima del while dove controlliamo se la nota sta ancora suonando
  unsigned sample_rate = 44100;
  double my_pi = 3.14159;
  size_t buf_size = seconds * sample_rate;
  // alloco PCM (pulse code modulation) audio buffer
  short *samples = malloc(sizeof(short) * buf_size);
  float weaken = 32760; //attenuare la nota
  printf("La frequenza della nota è %f\n", freq);
  int i = 0;
  for (; i < buf_size; ++i)
  {
    samples[i] = (weaken)*sin((2.f * my_pi * (freq)) / sample_rate * i);
    weaken -= 0.2;
  }

  //carico il buffer con OpenAL
  alBufferData(internal_buffer[ty], AL_FORMAT_MONO16, samples, buf_size, sample_rate);

  free(samples); //free dell'onda

  // inizializzazione della sorgente del suono e play del buffer
  alGenSources(1, &streaming_source[ty]);
  alSourcei(streaming_source[ty], AL_BUFFER, internal_buffer[ty]);
  alSourcePlay(streaming_source[ty]);
  alGetSourcei(streaming_source[ty], AL_SOURCE_STATE, &current_playing_state[ty]);

  sem_post(&mysem);

  while (AL_PLAYING == current_playing_state[ty])
  {
    if (alive[ty] == 0)
    {
      free(arg);
      exit_openal(ty);
      pthread_exit(NULL);
    }
    alGetSourcei(streaming_source[ty], AL_SOURCE_STATE, &current_playing_state[ty]);
  }
  printf("end of playing\n");
  alive[ty] = 0;
  //dealloco la struct OpenAL
  free(arg);
  exit_openal(ty);
  pthread_exit(NULL);
}

void exit_openal(int ty)
{
  // Stoppo la sorgente
  alSourceStopv(1, &streaming_source[ty]);
  alSourcei(streaming_source[ty], AL_BUFFER, 0);
  // Chiudo tutto
  alDeleteSources(1, &streaming_source[ty]);
  alDeleteBuffers(1, &internal_buffer[ty]);
  alcMakeContextCurrent(NULL);
  alcDestroyContext(openal_output_context[ty]);
  alcCloseDevice(openal_output_device);
}

void intHandler()
{
  printf("\r");
  printf("CHIUSURA IN CORSO\n");
  int i;
  int j;
  for (i = 0; i < 4; i++)
  {
    printf("\r");
    printf("[");
    for (j = 0; j < 25 * (i + 1); j++)
    {
      alive[i] = 0;
      pthread_join(threads[i], NULL);
      if (j == 50)
        printf("%d%%", (i + 1) * 25);
      else
      {
        usleep(10000);
        printf("#");
        fflush(NULL);
      }
    }
    for (; j < 100; j++)
    {
      if (j == 50)
        printf("%d%%", (i + 1) * 25);
      else
        printf(" ");
    }
    printf("]");
    fflush(NULL);
  }

  sem_destroy(&mysem);
  printf("\nIL PROGRAMMA È TERMINATO CON SUCCESSO\n");
  exit(0);
}