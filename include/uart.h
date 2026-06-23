/*--------------------------------------------------------
Purpose: "The purpose of this module is to provide low-level functions for serial communication via USART0 and USART1. 
It handles initialization, transmitting single bytes or strings, and receiving characters or delimited strings."

Input: Baud rate for initialization; character data for transmission/reception.
Output: Transmitted characters on UART0/UART1; received characters into buffers.

Uses: avr/io.h for accessing hardware registers (UDRx, UCSRx).

Author: OKC
Version: 1
Date and year: 2026.06.23
--------------------------------------------------------*/

#include <avr/io.h>

/**
 * @brief Initializes UART0 at a specified baud rate.
 * @param ubrr The desired baud rate value for UART0.
 */
void uart0_Init(unsigned int ubrr);

/**
 * @brief Initializes UART1 at a specified baud rate.
 * @param ubrr The desired baud rate value for UART1.
 */
void uart1_Init(unsigned int ubrr);

/**
 * @brief Transmits one character via USART0.
 * @param tx The byte to transmit.
 */
void putchUART0(char tx);

/**
 * @brief Transmits one character via USART1.
 * @param tx The byte to transmit.
 */
void putchUART1(char tx);

/**
 * @brief Receives a single character from USART0, blocking until available.
 * @return The received character.
 */
char getchUART0(void);

/**
 * @brief Receives a single character from USART1, blocking until available.
 * @return The received character.
 */
char getchUART1(void);

/**
 * @brief Transmits an entire null-terminated string via USART0.
 * @param ptr Pointer to the string to transmit.
 */
void putstrUART0(char *ptr);

/**
 * @brief Transmits an entire null-terminated string via USART1.
 * @param ptr Pointer to the string to transmit.
 */
void putstrUART1(char *ptr);

/**
 * @brief Receives a string from USART0 until '\n' or '\r' is found.
 * @param buffer Pointer to store the received string.
 * @param maxLength Maximum size of the buffer.
 * @return A pointer to the buffer containing the received string (null-terminated).
 */
char *getStrUART0(char buffer[], int maxLength);

/**
 * @brief Receives a string from USART1 until '\n' or '\r' is found.
 * @param buffer Pointer to store the received string.
 * @param maxLength Maximum size of the buffer.
 * @return A pointer to the buffer containing the received string (null-terminated).
 */
char *getStrUART1(char buffer[], int maxLength);

/**
 * @brief initalize uart0
 * @param ubrr BAUD rate
 */
void uart0_Init(unsigned int ubrr);
void uart1_Init(unsigned int ubrr);

/**
 * @brief transmit one byte through USART0
 * @param txt the byte to transmit
 */
void putchUART0(char tx);
void putchUART1(char tx);

/**
 * @brief receive 1 byte
 * @return char
 */
char getchUART0(void);
char getchUART1(void);

/**
 * @brief This function calls the putchUSART0 function for each byte referred to by the ptr!
 * @param ptr string to transmit, requiered to be null-terminated
 */
void putstrUART0(char *ptr);
void putstrUART1(char *ptr);

/**
 * @brief Receives a string through USART0 until a newline or carriage return is encountered.
 * @param buffer Pointer to the buffer where the string will be stored.
 * @param maxLength Maximum length of the buffer to prevent overflow.
 * @return Pointer to the buffer containing the received string.
 */
char *getStrUART0(char buffer[], int maxLength);
char *getStrUART1(char buffer[], int maxLength);