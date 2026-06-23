#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "I2C.h"     //include library for i2c driver
#include "ssd1306.h" //include display driver
#include <avr/interrupt.h>
#include "uart.h"
#include "spiFunctions.h"
#include "adcFunctions.h"
#include "labview.h"

#define BAUD 115200
#define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
#define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

volatile int receiveFlag = 0, AdcReady = 0;
volatile char byte = 0;
uint8_t inputBytes[MaxInputLength];

uint8_t generatorSettings[5] = {0};
uint16_t oscilloscopeSettings[2] = {10000, 300};

uint8_t AdcData1[MaxInputLength];
uint8_t AdcData2[MaxInputLength];

Packet storedInput = {0};

void init_adc()
{
    ADCSRA |= (1 << ADEN); // Global ADC enable 
    ADCSRA |= (1 << ADIE); // B: Enable interrupt

    //Set Prescaler 256
    ADCSRA |= (1 << ADPS2);
    ADCSRA |= (1 << ADPS1);
    ADCSRA |= (1 << ADPS0);

    // --- 3. Set Reference Voltage ---
    // Sets the voltage standard used for measurement (e.g., Vcc).
    // ADMUX |= (1 << REFS0);
    DIDR0 = 0xFF; // turn off the digital i/O
}

int main()
{
    I2C_Init();          // initialize i2c interface to display
    InitializeDisplay(); // initialize  display
    clear_display();     // use this before writing you own text

    uart1_Init(MY_UBRRD); // Initialize UART1
    uart0_Init(MY_UBRRD); // Initialize UART0

    SPI_MasterInit();
    init_adc();
    sei();

    // initialize timer 1 with a comparevalue
    InitTimer1(oscilloscopeSettings[0]);

    while (1)
    {

        if (receiveFlag)
        {
            // Process the command
            PrintPackageToDisplay(storedInput);
            ProcessLabViewCommand(&storedInput);
            receiveFlag = 0;
        }
        if (AdcReady)
        {
            SendDataToLabViewLRC8(oscilloscopeSettings[1], AdcData2, 2);
            AdcReady = false;
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
        // Validate checksum (XOR of everything except the 2-byte checksum at the end)
        uint8_t calculatedChecksum = CalculateXOR8(inputBytes, expectedLength - 2);
        uint8_t receivedChecksum = inputBytes[expectedLength - 1]; // Low byte of 2-byte checksum

        if (calculatedChecksum == receivedChecksum)
        {
            // Valid packet - store it
            storedInput.packetLength = expectedLength;
            storedInput.type = inputBytes[4];
            storedInput.dataLength = expectedLength - 7; // Minus sync(2) + length(2) + type(1) + checksum(2)
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
    AdcData1[i++] = ADC >> 2;
    if (i >= oscilloscopeSettings[1])
    {
        memcpy(AdcData2, AdcData1, oscilloscopeSettings[1]);
        i = 0;
        AdcReady = true;
    }
}