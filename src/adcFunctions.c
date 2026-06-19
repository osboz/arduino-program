#include <avr/io.h>
#include "adcFunctions.h"

void init_adc()
{
    ADCSRA |= (1 << ADEN) | (1 << ADIE);                  // enable adc and interrupt adc complete
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 125 kHz adc clock frequnecy

    ADMUX |= (1 << REFS0);
    DIDR0 = 0xFF; // turn off the digital i/O
}

unsigned int get_sample(char channel)
{
    /*returns a 10 bit sample from a chosen channel*/
    ADCSRA |= (1 << ADSC); // her start adc'en sin sampling

    // Polling: Venter på sample -> bliver klar ADIF bit bliver høj
    while (!(ADCSRA & (1 << ADIF)))
        ;
    // returnere 10 bit sampleværdi}
    return (ADCL + (ADCH << 8)); // returnere 10 bit sampleværdi
}
