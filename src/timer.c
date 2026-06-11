
#include "timer.h"


void InitTimer1(unsigned int _compareValue)
{
    TCCR1B |= (1 << CS10) | (1 << CS11) | (0 << CS12); // pre-scaLing 64
    TCCR1B |= (1 << WGM12);     // CTC mode TOP value = OCR1A
    OCR1A = _compareValue;      // for 1 ms between each compare match use 249 as parameter
    TIMSK1 |= (1 << OCIE1A);    // interrupt when TCNT1 - OCR1A value
}


ISR(TIMER1_COMPA_vect)
{
    static int count = 0;

    if (count++ >= 999)
    {
        count = 0;
        itsTime = 1;
    }
}
