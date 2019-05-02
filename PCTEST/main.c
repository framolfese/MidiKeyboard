#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include "../LibreriaCondivisa/libreria.h"
#include <string.h>

int main()
    {
    printf("size of tone : %ld\n",sizeof(Tone));
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
    //config.c_cc[VTIME] = 0;

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

    unsigned char aa;
    unsigned char bb;
    char buffer[sizeof(Tone)];
    int ciao;
    while (1)
    {

        ciao = read(fd, &aa, 1);
        if (aa != 0x55)
        {
            printf("errore primo parametro scynro :%c\n", aa);
            continue;
        }
        ciao = read(fd, &bb, 1);
        if (bb != 0xaa)
        {

            printf("errore secondo parametro scynro :%c\n", bb);
            continue;
        }
       
        ciao = read(fd, buffer,(2));
        //config.c_cc[VMIN] = 1;
        

        if (ciao < sizeof(Tone) || ciao > sizeof(Tone))
        {
            printf("errore lettura buffer ho letto %d byte\n", ciao);
            continue;
        }
        ciao = read(fd, &bb, 1);
        if (bb != 0xaa)
        {
            printf("errore ultimo parametro scynro\n");
            continue;
        }
        printf("ho letto correttamente adesso stampo \n");
        Tone t;
        memcpy(&t, buffer, sizeof(Tone));
        printf("Nota : %d - Value : %d\n", t.nota, t.on);
    }

    close(fd);
}