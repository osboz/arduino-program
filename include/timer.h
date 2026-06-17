
#include <avr/io.h>
#include <avr/interrupt.h>

int itsTime;

/**
 * @brief inits timer 1
 * @param _compareValue
 */
void InitTimer1(unsigned int _compareValue);

/**
 * @brief timer 1 interrupt, se
 * @var itsTime = 1 every second
 */
ISR(TIMER1_COMPA_vect);

/**
 * @brief sets the freqeuence of the timer
 *
 */
void SetFreqTimer1(uint16_t freq);