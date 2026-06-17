
#include "timer.h"

// void InitTimer1(unsigned int _compareValue)
// {
//     TCCR1B |= (1 << CS10) | (1 << CS11) | (0 << CS12); // pre-scaLing 64
//     TCCR1B |= (1 << WGM12);                            // CTC mode TOP value = OCR1A
//     OCR1A = _compareValue;                             // for 1 ms between each compare match use 249 as parameter
//     TIMSK1 |= (1 << OCIE1A);                           // interrupt when TCNT1 - OCR1A value
// }
// Added code to check if the input frequency is valid and handle it accordingly
void InitTimer1(uint16_t freq)
{
    if (freq == 0)
    {
        TCCR1B = (1 << WGM12);
        TIMSK1 &= ~(1 << OCIE1A); // Disable interrupt when frequency is 0
        return;
    }
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC, prescaler 8
    OCR1A = (F_CPU / 8UL / freq) - 1;
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
    OCR1A = (F_CPU / 8UL / freq) - 1;
}

// ISR(TIMER1_COMPA_vect)
// {
//     static int count = 0;

//     if (count++ >= 999)
//     {
//         count = 0;
//         itsTime = 1;
//     }
// }
