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
void putchUSART0(char tx);
void putchUSART1(char tx);

/**
 * @brief receive 1 byte
 * @return char
 */
char getchUSART0(void);
char getchUSART1(void);

/**
 * @brief This function calls the putchUSART0 function for each byte referred to by the ptr!
 * @param ptr string to transmit, requiered to be null-terminated
 */
void putstrUSART0(char *ptr);
void putstrUSART1(char *ptr);

/**
 * @brief Receives a string through USART0 until a newline or carriage return is encountered.
 * @param buffer Pointer to the buffer where the string will be stored.
 * @param maxLength Maximum length of the buffer to prevent overflow.
 * @return Pointer to the buffer containing the received string.
 */
char *getStrUSART0(char buffer[], int maxLength);
char *getStrUSART1(char buffer[], int maxLength);