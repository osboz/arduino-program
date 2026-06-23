/*--------------------------------------------------------
Purpose: "Initialization and control functions for Timer1. 

Input: Desired frequency (Hz) via SetFreqTimer1().
Output: Generates interrupts at the specified frequency on Timer1 Compare Match A.

interrupts: Timer1 Compare Match A.

Author: OKC
Version: 1
Date and year: 2026.06.23
--------------------------------------------------------*/

#include "timer.h"

// Added code to check if the input frequency is valid and handle it accordingly
void InitTimer1(uint16_t freq)
{
    if (freq == 0)
    {
        TCCR1B = (1 << WGM12);
        TIMSK1 &= ~(1 << OCIE1A); // Disable interrupt when frequency is 0
        return;
    }
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC, prescaler 64
    OCR1A = (F_CPU / 64UL / freq) - 1;

    TIMSK1 |= (1 << OCIE1A); // enables interrupt
}

void SetFreqTimer1(uint16_t freq)
{
    if (freq == 0)
    {
        TCCR1B = (1 << WGM12);
        TIMSK1 &= ~(1 << OCIE1A); // Disable interrupt when frequency is 0
        return;
    }
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC, prescaler 64
    OCR1A = (F_CPU / 64UL / freq) - 1;
    TIMSK1 |= (1 << OCIE1A); // enables interrupt
}
