
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
