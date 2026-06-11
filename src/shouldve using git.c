// #include <avr/io.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "I2C.h" //include library for i2c driver
// #include <util/delay.h>
// #include "ssd1306.h" //include display driver
// #include <avr/interrupt.h>
// #include "timer.h"
// #include "usart.h"

// #define BAUD 115200
// #define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
// #define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

// #define MaxInputLength 512

// volatile int receiveFlag = 0;
// volatile char byte = 0;
// uint8_t inputBytes[MaxInputLength];

// // define packet struct
// typedef struct
// {
//     uint16_t packetLength;
//     uint8_t type;
//     uint8_t data[MaxInputLength - 7]; // adjust size based on max payload
//     uint16_t crc;
// } Packet;

// volatile Packet storedInput = {0};

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

// int main()
// {

//     I2C_Init();          // initialize i2c interface to display
//     InitializeDisplay(); // initialize  display
//     clear_display();     // use this before writing you own text

//     SendStrActualXY("display works", 0, 0);
//     uart1_Init(MY_UBRRD); // Initialize UART1

//     // initialize timer 1 with a comparevalue
//     InitTimer1(249);
//     itsTime = 0;

//     INIT_INTERRUPT(4);
//     sei();

//     char buffer[512];

//     while (1)
//     {
//         if (receiveFlag)
//         {
//             receiveFlag = 0;
//             putstrUSART0("input is: ");
//             putstrUSART0(storedInput.data);
//             putchUSART0('\n');
//             putstrUSART1("input is: ");
//             putstrUSART1(storedInput.data);
//             putchUSART1('\n');

//             sprintf(buffer, "%s", storedInput.data);
//             SendStrActualXY(buffer, 0, 0);
//         }
//     }
// }

// /**
//  * @brief interrupt
//  */
// ISR(USART1_RX_vect)
// {
//     static int i = 0;
//     uint8_t received = UDR1; // Read the received byte

//     inputBytes[i++] = received;

//     char debugBuf[12];
//     sprintf(debugBuf, "%d", i);
//     putstrUSART1(debugBuf);
//     putstrUSART0(debugBuf);
//     SendStrActualXY(debugBuf, 0, 3);

//     // check if we received start bytes
//     if (i == 1)
//     {
//         if (inputBytes[0] != 0x55)
//         {
//             i = 0;
//             return;
//         }
//     }
//     if (i == 2)
//         if (inputBytes[1] != 0xAA)
//         {
//             i = 0;
//             return;
//         }

//     // get length bytes and type
//     if (i <= 5)
//         return;
        
//     sprintf(debugBuf, "%d", (int)(((uint16_t)inputBytes[2] << 8) | (uint16_t)inputBytes[3]));
//     SendStrActualXY(debugBuf, 0, 4);

//     // get data
//     if (i < (int)(((uint16_t)inputBytes[2] << 8) | (uint16_t)inputBytes[3]))
//         return;

//     // check parity
//     if (inputBytes[i] != 0x00 || inputBytes[i - 1] != 0x00)
//     {
//         i = 0;
//     }
//     else
//     { // store data in packet
//         storedInput.packetLength = ((uint16_t)inputBytes[2] << 8) | (uint16_t)inputBytes[3];
//         storedInput.type = inputBytes[4];
//         memcpy(storedInput.data, &inputBytes[5], storedInput.packetLength - 7);
//         storedInput.crc = ((uint16_t)inputBytes[storedInput.packetLength - 2] << 8) | (uint16_t)inputBytes[storedInput.packetLength - 1];

//         receiveFlag = 1; // Set the flag the indicates that we've recieved the input
//         i = 0;           // Reset index}
//     }
// }



// #include <avr/io.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include "I2C.h" //include library for i2c driver
// #include <util/delay.h>
// #include "ssd1306.h" //include display driver
// #include <avr/interrupt.h>
// #include "timer.h"
// #include "usart.h"

// #define BAUD 115200
// #define MY_UBRRD F_CPU / 8 / BAUD - 1  // double speed
// #define MY_UBRRH F_CPU / 16 / BAUD - 1 // half

// #define MaxInputLength 512

// volatile int receiveFlag = 0;
// volatile char byte = 0;
// volatile uint8_t bytes[MaxInputLength];
// volatile uint8_t storedinput[MaxInputLength];

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

// int main()
// {
//     I2C_Init();          // initialize i2c interface to display
//     InitializeDisplay(); // initialize  display
//     clear_display();     // use this before writing you own text

//     SendStrActualXY("display works", 0, 0);
//     uart1_Init(MY_UBRRD); // Initialize UART1

//     // initialize timer 1 with a comparevalue
//     InitTimer1(249);
//     itsTime = 0;

//     INIT_INTERRUPT(4);
//     sei();

//     char buffer[512];

//     while (1)
//     {
//         if (receiveFlag)
//         {
//             receiveFlag = 0;
//             putstrUSART0("input is: ");
//             putstrUSART0(bytes);
//             putchUSART0('\n');

//             sprintf(buffer, "%s", bytes);
//             SendStrActualXY(bytes, 0, 0);

//             memset(bytes, 0, MaxInputLength);
//         }
//     }
// }

// /**
//  * @brief interrupt
//  */
// ISR(USART1_RX_vect)
// {
//     static int i = 0;
//     uint8_t received = UDR1; // Read the received byte

//     char debugBuf[12];
//     sprintf(debugBuf, "%d", i);
//     putstrUSART1(debugBuf);
//     putstrUSART0(debugBuf);
//     SendStrActualXY(debugBuf, 0, 3);

//     // check if we received start byts
//     if (i < 2)
//     {
//         if (received == 0x55)
//         {
//             bytes[i] = received;
//             i = 1;
//             return;
//         }
//         else if (received == 0xAA)
//         {
//             bytes[i] = received;
//             i = 2;
//             return;
//         }
//         else
//         {
//             i = 0;
//         }
//     }

//     // get length bytes and type
//     if (i < 5)
//     {
//         bytes[i] = received;
//         i++;
//         return;
//     }

//     sprintf(debugBuf, "%d", (int)(((uint16_t)bytes[2] << 8) | (uint16_t)bytes[3]));
//     SendStrActualXY(debugBuf, 0, 4);
//     // get data
//     if (i < (int)((bytes[2] << 8) | bytes[3]) - 1)
//     {
//         bytes[i] = received;
//         i++;
//         return;
//     }

//     // check parity
//     if (received == 0x00 && bytes[i] == 0x00)
//     {
//         receiveFlag = 1; // Set the flag the indicates that we've recieved the input
//         i = 0;           // Reset index
//     }
// }
