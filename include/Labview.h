#include <avr/io.h>
#include <stdio.h>
#include "timer.h"
#include "usart.h"
#include "functions.h"
#include "timer.h"
#include "spiFunctions.h"
#define MaxInputLength 512

extern uint8_t generatorSettings[5];
extern uint16_t oscilloscopeSettings[2];

typedef struct
{
    uint16_t packetLength;
    uint8_t type;
    uint16_t dataLength;
    uint8_t data[MaxInputLength - 7];
    uint16_t crc;
} Packet;

/**
 * @brief prints the packet info to the OLED
 * @param pkt_ package with data
 */
void PrintPackageToDisplay(Packet pkt_);

/**
 * @brief sends data to labview
 * @param dataLenght length of data, not packet
 * @param data data to send
 * @param type 1-3
 * @param crc checksum
 */
void SendDataToLabView(uint16_t dataLenght, uint8_t *data, uint8_t type, uint16_t crc);

/**
 * @brief Process command from labview
 * @param pkt
 */
void ProcessLabViewCommand(Packet *pkt);

/**
 * @brief Process SEND command (Type 0x02)
 * Data: [SampleRate_H][SampleRate_L][RecordLength_H][RecordLength_L]
 */
void ProcessSendCommand(Packet *pkt);

/**
 * @brief Process START command (Type 0x03)
 * Data: empty
 */
void ProcessStartCommand(Packet *pkt);

/**
 * @brief Calculate XOR8 checksum
 */
uint8_t CalculateXOR8(uint8_t *data, size_t length);

/**
 * @brief sends to FPGA
 * 
 */
void SendDataToFPGA();