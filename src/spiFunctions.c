#include <avr/io.h>
#include "spiFunctions.h"

void SPI_MasterInit(void)
{
    // outputs
    DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2);

    // SPE aktiverer SPI generelt
    // MSTR = 1 betyder den er master, 0 betyder slave
    // SPR0 er med clk rate at gøre
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << DORD) | (1 << SPR1);
}

uint8_t SPI_MasterTransfer(uint8_t out)
{
    SPDR = out;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

void SPI_SlaveInit(void)
{
    /* Set MISO output, all others input */
    DDRB = (1 << PB3);
    /* Enable SPI */
    SPCR = (1 << SPE);
}

uint8_t SPI_SlaveReceive(void)
{
    SPDR = 0X00; // send dummy byte
    /* Wait for reception complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    /* Return Data Register */
    return SPDR;
}

void spi_select(void)
{
    PORTB &= ~(1 << SPI_SS_PIN); // SS low = FPGA selected
}

void spi_deselect(void)
{
    PORTB |= (1 << SPI_SS_PIN); // SS high = packet ended
}
