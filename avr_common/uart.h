#pragma once
#include <stdint.h>

struct Uart;
struct Uart* Uart_init(void);
void Uart_write(struct Uart*, uint8_t);
uint8_t Uart_available(struct Uart*);
uint16_t Uart_txFree(struct Uart*);