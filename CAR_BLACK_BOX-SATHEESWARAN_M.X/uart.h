/* 
 * File:   uart.h
 */

#ifndef UART_H
#define	UART_H

#define FOSC                20000000

void init_uart(unsigned long baud);
void uart_putc(unsigned char data);
void uart_puts(const char *s);

#endif	/* UART_H */