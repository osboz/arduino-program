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
