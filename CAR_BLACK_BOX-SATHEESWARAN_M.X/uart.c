#include <xc.h>
#include "uart.h"

void init_uart(unsigned long baud)
{
    /* Setting RC6 and RC7 to work as Serial Port */
    SPEN = 1;
    
    /* Baud Rate Setting Register */
    SPBRG = (unsigned char)((FOSC / (16 * baud)) - 1);
    
    /* Configure asynchronous high-speed mode */
    TXSTA = 0x24;

    /* Enable serial port */
    RCSTA = 0x80;
}



void uart_putc(unsigned char data)
{
    while (TXIF == 0)
    {
        continue;
    }

    TXREG = data;
}

void uart_puts(const char *s)
{
    while (*s)
    {
        uart_putc(*s++);
    }
}