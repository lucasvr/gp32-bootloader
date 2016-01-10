#ifndef UART_H
#define UART_H 1

#include "registers.h"

void init_uart(int base_clock);
void uart_printf(const char *format, ...);
void send_uart(unsigned long uart, char *string);

#endif /* UART_H */
