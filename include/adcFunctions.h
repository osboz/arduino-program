#include <avr/io.h>

/**
 * @brief  set up ports / pins
 */
void setupPins();


/**
 * @brief initializes ADC
 */
void init_adc();


/**
 * @brief selects a channel to read from ADMUX,
 * @param channel
 */
void select_ch(char channel);


/**
 * @brief Get the sample objectreturns a 10 bit sample from a chosen channel
 *
 * @param channel channel to read
 * @return unsigned int
 */
unsigned int get_sample(char channel);