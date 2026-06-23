/*--------------------------------------------------------
Purpose: "Initialization and control functions for Timer1. 

Input: Desired frequency (Hz) via SetFreqTimer1().
Output: Generates interrupts at the specified frequency on Timer1 Compare Match A.

interrupts: Timer1 Compare Match A.

Author: OKC
Version: 1
Date and year: 2026.06.23
--------------------------------------------------------*/

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * @brief inits timer 1
 * @param _compareValue
 */
void InitTimer1(uint16_t _compareValue);

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