#include <avr/io.h>

/**
 * @brief initilialise the SPI_master; from "dataOgMega2560"
 */
void SPI_MasterInit(void)
{
    // outputs
    DDRB = (1 << PB0) | (1 << PB1) | (1 << PB2);

    // SPE aktiverer SPI generelt
    // MSTR = 1 betyder den er master, 0 betyder slave
    // SPR0 er med clk rate at gøre
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}
/**
 * @brief sends a 8bit value tothe slave and retu
 *
 * @param out 8bit value to send
 * @return uint8_t 8bit return value 
 */
uint8_t SPI_MasterTransfer(uint8_t out)
{
    SPDR = out;
    while (!(SPSR & (1 << SPIF)))
        ;
    return SPDR;
}

/**
 * @brief initilialise the board as the SPI_Slave; from "dataOgMega2560"
 */
void SPI_SlaveInit(void)
{
    /* Set MISO output, all others input */
    DDRB = (1 << PB3);
    /* Enable SPI */
    SPCR = (1 << SPE);
}

/**
 * @brief transmit data from slave to master by sending an empty / dummy byte(0x00)
 */
uint8_t SPI_SlaveReceive(void)
{
    SPDR = 0X00; // send dummy byte
    /* Wait for reception complete */
    while (!(SPSR & (1 << SPIF)))
        ;
    /* Return Data Register */
    return SPDR;
}