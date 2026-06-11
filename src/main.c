#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "I2C.h" //include library for i2c driver
#include <util/delay.h>
#include "ssd1306.h" //include display driver
#include <avr/interrupt.h>
#include "timer.h"
#include "usart.h"
#include "functions.h"

#define BAUD 115200
#define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
#define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

#define MaxInputLength 512

volatile int receiveFlag = 0;
volatile char byte = 0;
uint8_t inputBytes[MaxInputLength];

typedef struct
{
    uint16_t packetLength;
    uint8_t type;
    uint16_t dataLength;
    uint8_t data[MaxInputLength - 7];
    uint16_t crc;
} Packet;

volatile Packet storedInput = {0};

// marco to define an interrupt based on number input
#define INIT_INTERRUPT(nr)            \
    do                                \
    {                                 \
        EICR##B |= (1 << ISC##nr##1); \
        PORT##E |= (1 << PE##nr);     \
        EIMSK |= (1 << INT##nr);      \
    } while (0)

/**
 * @brief prints the packet info to the OLED
 *
 * @param pkt_ package with data
 */
void PrintPackageToDisplay(Packet pkt_)
{
    char buffer[64];
    putstrUSART0("input is: ");
    putstrUSART0(pkt_.data);
    putchUSART0('\n');
    putstrUSART1("input is: ");
    putstrUSART1(pkt_.data);
    putchUSART1('\n');

    sprintf(buffer, "PkLn: %2d", pkt_.packetLength);
    SendStrActualXY(buffer, 0, 1);
    sprintf(buffer, "type: %2d", pkt_.type);
    SendStrActualXY(buffer, 0, 2);
    SendStrActualXY("data:", 0, 3);

    for (int k = 0; k < pkt_.packetLength - 7; k++)
    {
        char hexBuf[8];
        sprintf(hexBuf, "%02X ", pkt_.data[k]);
        SendStrActualXY(hexBuf, k * 2 + 6, 3);
    }
    sprintf(buffer, "crc:  %2x", pkt_.crc);
    SendStrActualXY(buffer, 0, 4);
}

int main()
{
    I2C_Init();          // initialize i2c interface to display
    InitializeDisplay(); // initialize  display
    clear_display();     // use this before writing you own text

    SendStrActualXY("display works", 0, 0);
    uart1_Init(MY_UBRRD); // Initialize UART1

    // initialize timer 1 with a comparevalue
    InitTimer1(249);

    INIT_INTERRUPT(4);
    sei();

    while (1)
    {
        if (receiveFlag)
        {
            receiveFlag = 0;
            PrintPackageToDisplay(storedInput);
        }
    }
}

/**
 * @brief Calculate XOR8 checksum
 */
uint8_t CalculateXOR8(uint8_t *data, uint16_t length)
{
    uint8_t checksum = 0;
    for (uint16_t i = 0; i < length; i++)
    {
        checksum ^= data[i];
    }
    return checksum;
}

/**
 * @brief USART1 RX interrupt - receives and parses LabVIEW protocol packets
 */
ISR(USART1_RX_vect)
{
    static int i = 0;
    static uint16_t expectedLength = 0;
    uint8_t received = UDR1;

    inputBytes[i++] = received;

    // State 1: Look for sync bytes (0x55 0xAA)
    if (i == 1)
    {
        if (inputBytes[0] != 0x55)
        {
            i = 0;
            return;
        }
    }
    if (i == 2)
    {
        if (inputBytes[1] != 0xAA)
        {
            i = 0;
            return;
        }
    }

    // State 2: Get length (bytes 2-3, big-endian)
    if (i == 4)
    {
        expectedLength = ((uint16_t)inputBytes[2] << 8) | (uint16_t)inputBytes[3];

        // Safety check: packet too long
        if (expectedLength > MaxInputLength || expectedLength < 5)
        {
            i = 0;
            return;
        }
    }

    // State 3: Read rest of packet until we have expectedLength bytes
    if (i < expectedLength)
    {
        return; // Keep collecting bytes
    }

    // State 4: Complete packet received - validate and parse
    if (i >= expectedLength)
    {
        // Validate checksum (simplified XOR8 for now)
        uint8_t calculatedChecksum = CalculateXOR8(&inputBytes[4], expectedLength - 6);
        uint8_t receivedChecksum = inputBytes[expectedLength - 1];

        if (calculatedChecksum == receivedChecksum || receivedChecksum == 0x00)
        {
            // Valid packet - store it
            storedInput.packetLength = expectedLength;
            storedInput.type = inputBytes[4];
            storedInput.dataLength = expectedLength - 7; // Minus sync, length, type, crc
            memcpy(storedInput.data, &inputBytes[5], storedInput.dataLength);
            storedInput.crc = ((uint16_t)inputBytes[expectedLength - 2] << 8) | (uint16_t)inputBytes[expectedLength - 1];

            receiveFlag = 1; // Signal main loop
        }

        i = 0; // Reset for next packet
    }
}
