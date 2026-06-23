#include <avr/io.h>

/**
 * @brief initilialise the board as the SPI_master; from "dataOgMega2560"
 */
void SPI_MasterInit(void);

/**
 * @brief transmit data to slave, and return the data received from the slave
 *
 * @param out 8bit value to send
 * @return uint8_t 8bit return value
 */
uint8_t SPI_MasterTransfer(uint8_t out);

/**
 * @brief initilialise the board as the SPI_Slave; from "dataOgMega2560"
 */
void SPI_SlaveInit(void);

/**
 * @brief transmit data from slave to master by sending an empty / dummy byte(0x00)
 */
uint8_t SPI_SlaveReceive(void);

#define SPI_SS_PIN PB0
#define SPI_SCK_PIN PB1
#define SPI_MOSI_PIN PB2
#define SPI_MISO_PIN PB3

/**
 * @brief set ss pin low
 *
 */
void spi_select(void);

/**
 * @brief set ss pin high
 *
 */
void spi_deselect(void);
