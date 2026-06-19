#include <avr/io.h>

void uart0_Init(unsigned int ubrr)
{
    UCSR0A |= (1 << U2X0);                                 // double speed
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // enable receiver and transmit
    UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);               // 8 data bit, no parity
    // baud rate values up to 16 bit therefore to registers
    UBRR0H = (unsigned char)(ubrr >> 8);
    UBRR0L = (unsigned char)ubrr;
}

void uart1_Init(unsigned int ubrr)
{
    UCSR1A |= (1 << U2X1);                                 // double speed
    UCSR1B |= (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1); // enable receiver and transmit
    UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11);               // 8 data bit, no parity
    // baud rate values up to 16 bit therefore to registers
    UBRR1H = (unsigned char)(ubrr >> 8);
    UBRR1L = (unsigned char)ubrr;
}

void putchUART0(char tx)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ; // wait for empty transmit buffer
    UDR0 = tx;
}
void putchUART1(char tx)
{
    while (!(UCSR1A & (1 << UDRE1)))
        ; // wait for empty transmit buffer
    UDR1 = tx;
}

char getchUART0(void)
{
    while (!(UCSR0A & (1 << RXC0)))
        ; // wait until a character is received
    return UDR0;
}

char getchUART1(void)
{
    while (!(UCSR1A & (1 << RXC1)))
        ; // wait until a character is received
    return UDR1;
}

void putstrUART0(char *ptr)
{
    while (*ptr)
    {
        putchUART0(*ptr);
        ptr++;
    }
}
void putstrUART1(char *ptr)
{
    while (*ptr)
    {
        putchUART1(*ptr);
        ptr++;
    }
}

char *getStrUART0(char buffer[], int maxLength)
{
    int index = 0; // Initialize index to track position in buffer

    while (index < maxLength)
    {
        // Read a character from USART
        buffer[index] = getchUART0();

        if (index > 0)
            if (buffer[index - 1] == '\n' || buffer[index] == '\r')
            {
                break;
            }
        index++;
    }

    buffer[index] = '\0'; // Null-terminate the string
    return buffer;
}

char *getStrUART1(char buffer[], int maxLength)
{
    int index = 0; // Initialize index to track position in buffer

    while (index < maxLength)
    {
        // Read a character from USART
        buffer[index] = getchUART1();

        if (index > 0)
            if (buffer[index - 1] == '\n' || buffer[index] == '\r')
            {
                break;
            }
        index++;
    }

    buffer[index] = '\0'; // Null-terminate the string
    return buffer;
}
