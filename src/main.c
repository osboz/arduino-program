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
#include "adcFunctions.h"
#include "Labview.h"

#define BAUD 115200
#define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
#define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

#define MaxInputLength 512

volatile int receiveFlag = 0, AdcReady = 0;
volatile char byte = 0;
uint8_t inputBytes[MaxInputLength];

uint8_t generatorSettings[5] = {0};
uint16_t oscilloscopeSettings[2] = {10000, 300};

uint8_t AdcData1[1024];
uint8_t AdcData2[1024];

Packet storedInput = {0};

// marco to define an interrupt based on number input
#define INIT_INTERRUPT(nr)            \
    do                                \
    {                                 \
        EICR##B |= (1 << ISC##nr##1); \
        PORT##E |= (1 << PE##nr);     \
        EIMSK |= (1 << INT##nr);      \
    } while (0)

int main()
{
    I2C_Init();          // initialize i2c interface to display
    InitializeDisplay(); // initialize  display
    clear_display();     // use this before writing you own text

    SendStrActualXY("display", 9, 0);
    uart1_Init(MY_UBRRD); // Initialize UART1
    uart0_Init(MY_UBRRD); // Initialize UART0

    SPI_MasterInit();
    init_adc();
    sei();

    // initialize timer 1 with a comparevalue
    InitTimer1(oscilloscopeSettings[0]);

    // INIT_INTERRUPT(4);

    while (1)
    {
        if (AdcReady)
        {
            SendDataToLabView(oscilloscopeSettings[1], AdcData1, 2, 0x0000);
            AdcReady = false;
        }

        if (receiveFlag)
        {
            receiveFlag = 0;
            // Process the command
            PrintPackageToDisplay(storedInput);
            ProcessLabViewCommand(&storedInput);
        }
    }
}

/**
 * @brief UART1 RX interrupt - receives and parses LabVIEW protocol packets
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
        uint8_t receivedChecksum = inputBytes[expectedLength];

        // if (calculatedChecksum == receivedChecksum)
        if (receivedChecksum == 0x00)
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

// Timer interrupt service routine. updates channel and enables adc
ISR(TIMER1_COMPA_vect)
{
    // Start ADC conversion
    ADCSRA |= (1 << ADSC); // enables adc
}

// read adc data
ISR(ADC_vect)
{
    static int i = 0;
    AdcData1[i++] = ADC;
    if (i >= oscilloscopeSettings[1])
    {
        memcpy(AdcData2, AdcData1, oscilloscopeSettings[1]);
        i = 0;
        AdcReady = true;
    }
}
