#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define BUF_SIZE 256

typedef struct Uart{
  uint8_t rx_buffer[BUF_SIZE];
  uint8_t rx_start;
  uint8_t rx_end;
  uint8_t rx_size;

  uint8_t tx_buffer[BUF_SIZE];
  uint8_t tx_start;
  uint8_t tx_end;
  uint8_t tx_size;
}Uart;

static Uart uart;

void setBaud19200(void) {
#define BAUD 19200
#include <util/setbaud.h>
UBRR0H = UBRRH_VALUE;
UBRR0L = UBRRL_VALUE;

#ifdef USE_2X
UCSR0A |= (1<<U2X0);
#endif
#undef BAUD
}

struct Uart* Uart_init(void) {
  cli();
  setBaud19200();
  uart.rx_start=0;
  uart.rx_end=0;
  uart.rx_size=0;
  uart.tx_start=0;
  uart.tx_end=0;
  uart.tx_size=0;
  for(int i=0;i<BUF_SIZE;++i)
    uart.tx_buffer[i]=0xCE;

  UCSR0A=0x00;
  UCSR0C=(1<<UCSZ01) | (1<<UCSZ00);
  UCSR0B=(1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
  sei();
  return &uart;
}

void Uart_write(struct Uart* u, uint8_t c) {
  while(u->tx_size>=BUF_SIZE) {}
  u->tx_buffer[u->tx_end]=c;
  u->tx_end++;
  u->tx_size++;
  UCSR0B |= (1<<UDRIE0);
  return;
}

uint8_t Uart_available(struct Uart* u) {
  return u->rx_size;
}

uint16_t Uart_txFree(struct Uart* u) {
  return BUF_SIZE-u->tx_size;
}

ISR(USART0_RX_vect) {
  uint8_t c=UDR0; 
  if(uart.rx_size<BUF_SIZE) {
    uart.rx_buffer[uart.rx_end]=c;
    ++uart.rx_end;
    uart.rx_size++;
  }
}

ISR(USART0_UDRE_vect) {
  if(!uart.tx_size) { 
    UCSR0B &= ~(1<<UDRIE0); 
  } else {
    UDR0=uart.tx_buffer[uart.tx_start];
    ++uart.tx_start;
    --uart.tx_size;
  }  
}