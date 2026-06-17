// #include <avr/io.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include "I2C.h" //include library for i2c driver
// #include <util/delay.h>
// #include "ssd1306.h" //include display driver
// #include <avr/interrupt.h>
// #include "timer.h"
// #include "usart.h"

// #define BAUD 19200
// #define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
// #define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

// #define InputLength 16

// unsigned long int time = INT16_MAX;
// volatile uint8_t receiveFlag;
// volatile char byte, button4Flag = 0;
// char bytes[InputLength];

// // marco to define an interrupt based on number input
// #define INIT_INTERRUPT(nr)            \
//     do                                \
//     {                                 \
//         EICR##B |= (1 << ISC##nr##1); \
//         PORT##E |= (1 << PE##nr);     \
//         EIMSK |= (1 << INT##nr);      \
//     } while (0)

// /**
//  * @brief sends a string using sendStrXY, but with fixed XY coordianates
//  * @param _string the string
//  * @param _x x coordiante
//  * @param _y y coordinate
//  */
// void SendStrActualXY(char *_string, int _x, int _y)
// {
//     sendStrXY(_string, _y, _x);
// }

// /**
//  * @brief Constructs a time string from an UL int value.
//  * @param _timebuffer Pointer to the buffer where the time string will be stored.
//  * @param _time unsinged long Int value representing the time.
//  * @return char*
//  */
// char *IntToTimeFormatter(char *_timebuffer, unsigned long int _time)
// {
//     int hours = _time / 3600;
//     int minutes = (_time / 60) % 60;
//     int seconds = _time % 60;
//     sprintf(_timebuffer, "%02d:%02d:%02d", hours, minutes, seconds);
//     return _timebuffer;
// }

// /**
//  * @brief Construct a new Input String To Int Time object
//  * @param _input char* input string(char pointer)
//  */
// unsigned long int InputStringToIntTime(char *_input)
// {
//     unsigned int hours, minutes, seconds;
//     sscanf(_input, "%u:%u:%u", &hours, &minutes, &seconds);
//     unsigned long int temp = hours * 3600UL + minutes * 60UL + seconds; // Use UL suffix
//     return temp;
// }

// int main2()
// {
//     char timebuffer[InputLength];

//     I2C_Init();          // initialize i2c interface to display
//     InitializeDisplay(); // initialize  display
//     clear_display();     // use this before writing you own text

//     SendStrActualXY("display works", 0, 0);
//     uart0_Init(MY_UBRRD); // Initialize UART1

//     // initialize timer 1 with a comparevalue
//     InitTimer1(249);
//     itsTime = 0;

//     INIT_INTERRUPT(4);
//     sei();

//     while (1)
//     {
//         if (receiveFlag)
//         {
//             receiveFlag = 0;
//             putstrUSART0("input is: ");
//             putstrUSART0(bytes);
//             putchUSART0('\n');
//             // SendStrActualXY(bytes, 0, 2);
//             time = InputStringToIntTime(bytes);

//             clear_display();
//             SendStrActualXY("display works", 0, 0);

//             memset(bytes, 0, InputLength);
//         }

//         if (itsTime && time != INT16_MAX)
//         {
//             // reset time if more than 24h
//             if (time >= 86400)
//             {
//                 time = time % 86400;
//             }

//             IntToTimeFormatter(timebuffer, time); // Wrap at 24 hours
//             SendStrActualXY(timebuffer, 0, 4);
//             putstrUSART0(timebuffer);
//             putchUSART0('\n');

//             time++;
//             itsTime = 0;
//         }

//         if (button4Flag)
//         {
//             _delay_ms(100);
//             button4Flag = 0;
//             putstrUSART0("type in a new time");
//             SendStrActualXY("type in a new time", 0, 0);
//             time = INT16_MAX;
//         }
//     }
// }

// /**
//  * @brief interrupt
//  */
// ISR(USART0_RX_vect)
// {
//     static int i = 0;
//     char received = UDR0; // Read the received byte
//     if (received == '\n' || i >= InputLength - 1) // Check for newline or buffer limit
//     {
//         bytes[i] = '\0'; // Null-termination
//         receiveFlag = 1; // Set the flag the indicates that we've recieved the input
//         i = 0;           // Reset index
//     }
//     else
//     {
//         bytes[i++] = received; // Store the received character
//     }
// }

// ISR(INT4_vect)
// {
//     button4Flag = 1;
// }




// /**
//  * @brief
//  *
//  * @param pkt
//  */
// void ProcessLabViewCommand(Packet *pkt)
// {
//     switch (pkt->type)
//     {
//     case 0x01: // BTN pressed
//         ProcessButtonCommand(pkt);
//         break;

//     case 0x02: // SEND pressed (sample rate + record length)
//         ProcessSendCommand(pkt);
//         break;

//     case 0x03: // START pressed (Bode plot)
//         ProcessStartCommand(pkt);
//         break;

//     default:
//         putstrUSART0("Unknown command type\n");
//         break;
//     }
// }

// /**
//  * @brief Process BTN command (Type 0x01)
//  * Data: [BTN_Value (1 byte)][SW_Value (1 byte)]
//  *
//  * Note: LabVIEW uses 0-based button indexing (BTN0=0x00, BTN1=0x01, etc.)
//  *       FPGA uses 1-based button indexing (BTN1=0x01, BTN2=0x02, etc.)
//  *       Therefore we increment btnValue by 1 before sending to FPGA
//  */
// void ProcessButtonCommand(Packet *pkt)
// {
//     if (pkt->dataLength < 2)
//         return;

//     uint8_t btnValue = pkt->data[0]; // 0x00=BTN0, 0x01=BTN1, 0x02=BTN2, 0x03=BTN3 (from LabVIEW)
//     uint8_t swValue = pkt->data[1];  // Software value (parameter)

//     char buffer[64];
//     sprintf(buffer, "BTN %d pressed, SW=0x%02X\n", btnValue, swValue);
//     putstrUSART0(buffer);

//     // Increment btnValue for FPGA (FPGA uses 1-based indexing)
//     uint8_t fpgaBtnValue = btnValue + 1; // Convert to FPGA indexing: 0x00->0x01, 0x01->0x02, etc.
//     sprintf(buffer, "BTNFpga %hx\n", fpgaBtnValue);
//     putstrUSART0(buffer);

//     // Send to FPGA via SPI
//     SPI_MasterTransfer(swValue);
//     SPI_MasterTransfer(fpgaBtnValue);
// }

// /**
//  * @brief Process SEND command (Type 0x02)
//  * Data: [SampleRate_H][SampleRate_L][RecordLength_H][RecordLength_L]
//  */
// void ProcessSendCommand(Packet *pkt)
// {
//     if (pkt->dataLength < 4)
//         return;

//     uint16_t sampleRate = ((uint16_t)pkt->data[0] << 8) | (uint16_t)pkt->data[1];
//     uint16_t recordLength = ((uint16_t)pkt->data[2] << 8) | (uint16_t)pkt->data[3];

//     char buffer[64];
//     sprintf(buffer, "Sample Rate: %d sps, Record Length: %d\n", sampleRate, recordLength);
//     putstrUSART0(buffer);

//     // Update ADC configuration
//     // SetADCParameters(sampleRate, recordLength);
// }

// /**
//  * @brief Process START command (Type 0x03)
//  * Data: empty
//  */
// void ProcessStartCommand(Packet *pkt)
// {
//     putstrUSART0("Bode plot START received\n");
//     // Start frequency sweep or trigger action
// }

// /**
//  * @brief Send oscilloscope data packet back to LabVIEW
//  * Type: 0x02 (OSCILLOSCOPE telecommand)
//  */
// void SendOscilloscopeData(uint8_t *samples, uint16_t numSamples)
// {
//     uint16_t packetLength = 5 + numSamples + 2; // Sync(2) + Len(2) + Type(1) + Data(n) + CRC(2)

//     // Send header
//     putchUSART0(0x55);
//     putchUSART0(0xAA);
//     putchUSART0((packetLength >> 8) & 0xFF); // Length H (big-endian)
//     putchUSART0(packetLength & 0xFF);        // Length L
//     putchUSART0(0x02);                       // Type: OSCILLOSCOPE

//     // Send sample data
//     uint8_t checksum = 0;
//     checksum ^= 0x02; // Start with type in checksum

//     for (uint16_t i = 0; i < numSamples; i++)
//     {
//         putchUSART0(samples[i]);
//         checksum ^= samples[i];
//     }

//     // Send checksum
//     putchUSART0(0x00);     // CRC high (unused for XOR8)
//     putchUSART0(checksum); // CRC low (XOR8)
// }

// /**
//  * @brief Send generator status back to LabVIEW
//  * Type: 0x01 (GENERATOR telecommand)
//  */
// void SendGeneratorStatus(uint8_t active, uint8_t shape, uint8_t amplitude, uint8_t frequency)
// {
//     uint16_t packetLength = 9; // Sync(2) + Len(2) + Type(1) + Data(4) + CRC(2)

//     putchUSART0(0x55);
//     putchUSART0(0xAA);
//     putchUSART0(0x00);
//     putchUSART0(0x09);
//     putchUSART0(0x01); // Type: GENERATOR

//     uint8_t checksum = 0x01;
//     putchUSART0(active);
//     checksum ^= active;

//     putchUSART0(shape);
//     checksum ^= shape;

//     putchUSART0(amplitude);
//     checksum ^= amplitude;

//     putchUSART0(frequency);
//     checksum ^= frequency;

//     putchUSART0(0x00);
//     putchUSART0(checksum);
// }
