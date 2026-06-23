/*--------------------------------------------------------
Purpose: "The purpose of this module is to handle incoming commands from LabVIEW, process them, and send responses back to LabVIEW."

Input: Incoming commands from LabVIEW (packets)
Output: Responses sent back to LabVIEW (packets)

Author: OKC
Version: 1
Date and year: 2026.06.23

Global parameters used:
* generatorSettings array for storing generator settings
* oscilloscopeSettings array for storing oscilloscope settings

The main function is to kommunicate between labview, the MCU and the FPGA/ossiliscope

Interrupt Service Routine:
None
--------------------------------------------------------*/

#include <avr/io.h>
#include <stdio.h>
#include "timer.h"
#include "uart.h"
#include "timer.h"
#include "spiFunctions.h"
#include "ssd1306.h"


#define MaxInputLength 1024

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
void SendDataToLabView(uint16_t dataLenght, uint8_t *data, uint8_t type);
void SendDataToLabViewLRC8(uint16_t dataLenght, uint8_t *data, uint8_t type);

/**
 * @brief Process command from labview
 * @param pkt
 */
void ProcessLabViewCommand(Packet *pkt);

/**
 * @brief process type 1 command
 * 
 * @param pkt 
 */
void ProcessButtonCommand(Packet *pkt);

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
 */
void SendDataToFPGA();