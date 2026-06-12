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
#include "spiFunctions.h"

#define BAUD 115200
#define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
#define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

#define MaxInputLength 512

volatile int receiveFlag = 0, receiveFlag2 = 0;
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
    putstrUSART0("Packet received:\n");
    putstrUSART1("Packet received:\n");

    // Print packet length
    sprintf(buffer, "PkLen: %d\n", pkt_.packetLength);
    putstrUSART0(buffer);
    putstrUSART1(buffer);

    // Print type
    sprintf(buffer, "Type: %d\n", pkt_.type);
    putstrUSART0(buffer);
    putstrUSART1(buffer);

    // Print data as hex (not as raw binary)
    putstrUSART0("Data (hex): ");
    putstrUSART1("Data (hex): ");
    for (int k = 0; k < pkt_.dataLength; k++)
    {
        sprintf(buffer, "%02X ", pkt_.data[k]);
        putstrUSART0(buffer);
        putstrUSART1(buffer);
    }
    putchUSART0('\n');
    putchUSART1('\n');

    // Print CRC
    sprintf(buffer, "CRC: %04X\n", pkt_.crc);
    putstrUSART0(buffer);
    putstrUSART1(buffer);
}

/**
 * @brief
 *
 * @param pkt
 */
void ProcessLabViewCommand(Packet *pkt)
{
    switch (pkt->type)
    {
    case 0x01: // BTN pressed
        ProcessButtonCommand(pkt);
        break;

    case 0x02: // SEND pressed (sample rate + record length)
        ProcessSendCommand(pkt);
        break;

    case 0x03: // START pressed (Bode plot)
        ProcessStartCommand(pkt);
        break;

    default:
        putstrUSART0("Unknown command type\n");
        break;
    }
}

/**
 * @brief Process BTN command (Type 0x01)
 * Data: [BTN_Value (1 byte)][SW_Value (1 byte)]
 *
 * Note: LabVIEW uses 0-based button indexing (BTN0=0x00, BTN1=0x01, etc.)
 *       FPGA uses 1-based button indexing (BTN1=0x01, BTN2=0x02, etc.)
 *       Therefore we increment btnValue by 1 before sending to FPGA
 */
void ProcessButtonCommand(Packet *pkt)
{
    if (pkt->dataLength < 2)
        return;

    uint8_t btnValue = pkt->data[0]; // 0x00=BTN0, 0x01=BTN1, 0x02=BTN2, 0x03=BTN3 (from LabVIEW)
    uint8_t swValue = pkt->data[1];  // Software value (parameter)

    char buffer[64];
    sprintf(buffer, "BTN %d pressed, SW=0x%02X\n", btnValue, swValue);
    putstrUSART0(buffer);

    // Increment btnValue for FPGA (FPGA uses 1-based indexing)
    uint8_t fpgaBtnValue = btnValue + 1; // Convert to FPGA indexing: 0x00->0x01, 0x01->0x02, etc.
    sprintf(buffer, "BTNFpga %hx\n", fpgaBtnValue);
    putstrUSART0(buffer);

    // Send to FPGA via SPI
    SPI_MasterTransfer(swValue);
    SPI_MasterTransfer(fpgaBtnValue);
}

/**
 * @brief Process SEND command (Type 0x02)
 * Data: [SampleRate_H][SampleRate_L][RecordLength_H][RecordLength_L]
 */
void ProcessSendCommand(Packet *pkt)
{
    if (pkt->dataLength < 4)
        return;

    uint16_t sampleRate = ((uint16_t)pkt->data[0] << 8) | (uint16_t)pkt->data[1];
    uint16_t recordLength = ((uint16_t)pkt->data[2] << 8) | (uint16_t)pkt->data[3];

    char buffer[64];
    sprintf(buffer, "Sample Rate: %d sps, Record Length: %d\n", sampleRate, recordLength);
    putstrUSART0(buffer);

    // Update ADC configuration
    // SetADCParameters(sampleRate, recordLength);
}

/**
 * @brief Process START command (Type 0x03)
 * Data: empty
 */
void ProcessStartCommand(Packet *pkt)
{
    putstrUSART0("Bode plot START received\n");
    // Start frequency sweep or trigger action
}

/**
 * @brief Send oscilloscope data packet back to LabVIEW
 * Type: 0x02 (OSCILLOSCOPE telecommand)
 */
void SendOscilloscopeData(uint8_t *samples, uint16_t numSamples)
{
    uint16_t packetLength = 5 + numSamples + 2; // Sync(2) + Len(2) + Type(1) + Data(n) + CRC(2)

    // Send header
    putchUSART0(0x55);
    putchUSART0(0xAA);
    putchUSART0((packetLength >> 8) & 0xFF); // Length H (big-endian)
    putchUSART0(packetLength & 0xFF);        // Length L
    putchUSART0(0x02);                       // Type: OSCILLOSCOPE

    // Send sample data
    uint8_t checksum = 0;
    checksum ^= 0x02; // Start with type in checksum

    for (uint16_t i = 0; i < numSamples; i++)
    {
        putchUSART0(samples[i]);
        checksum ^= samples[i];
    }

    // Send checksum
    putchUSART0(0x00);     // CRC high (unused for XOR8)
    putchUSART0(checksum); // CRC low (XOR8)
}

/**
 * @brief Send generator status back to LabVIEW
 * Type: 0x01 (GENERATOR telecommand)
 */
void SendGeneratorStatus(uint8_t active, uint8_t shape, uint8_t amplitude, uint8_t frequency)
{
    uint16_t packetLength = 9; // Sync(2) + Len(2) + Type(1) + Data(4) + CRC(2)

    putchUSART0(0x55);
    putchUSART0(0xAA);
    putchUSART0(0x00);
    putchUSART0(0x09);
    putchUSART0(0x01); // Type: GENERATOR

    uint8_t checksum = 0x01;
    putchUSART0(active);
    checksum ^= active;

    putchUSART0(shape);
    checksum ^= shape;

    putchUSART0(amplitude);
    checksum ^= amplitude;

    putchUSART0(frequency);
    checksum ^= frequency;

    putchUSART0(0x00);
    putchUSART0(checksum);
}

int main()
{
    I2C_Init();          // initialize i2c interface to display
    InitializeDisplay(); // initialize  display
    clear_display();     // use this before writing you own text

    SendStrActualXY("display", 9, 0);
    uart1_Init(MY_UBRRD); // Initialize UART1
    uart0_Init(MY_UBRRD); // Initialize UART0

    SPI_MasterInit();

    // initialize timer 1 with a comparevalue
    // InitTimer1(249);

    // INIT_INTERRUPT(4);
    sei();

    while (1)
    {
        if (receiveFlag)
        {
            _delay_ms(10);
            SendStrActualXY("RecFlag", 0, 0);
            receiveFlag = 0;
            // Process the command
            ProcessLabViewCommand(&storedInput);
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
        if (inputBytes[0] != 0x55)
        {
            i = 0;
            return;
        }

    if (i == 2)
        if (inputBytes[1] != 0xAA)
        {
            i = 0;
            return;
        }

    if (i < 4)
        return;
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
        return; // Keep collecting bytes

    // State 4: Complete packet received - validate and parse
    if (i >= expectedLength)
    {
        // Validate checksum (simplified XOR8 for now)
        uint8_t calculatedChecksum = CalculateXOR8(&inputBytes[4], expectedLength - 6);
        uint8_t receivedChecksum = inputBytes[expectedLength - 1];

        // if (calculatedChecksum == receivedChecksum || receivedChecksum == 0x00)
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
