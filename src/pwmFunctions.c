#include <avr/io.h>
#include "pwmFunctions.h"

void setupPins()
{
    // Pin 11 (OC1A) - Timer1
    {
        DDRB |= (1 << PB5);                  // pin 11
        TCCR1A |= (1 << COM1A1);             // Clear OC1A on Compare Match
        TCCR1B = (1 << CS11) | (1 << WGM13); // prescaling by 8
        ICR1 = 20000;                        // top value
        OCR1A = 1500;                        // duty cycle
    }

    // Pin 5 (OC3A) - Timer3
    {
        DDRE |= (1 << PE3);                  // pin 5
        TCCR3A |= (1 << COM3A1);             // Clear OC3A on Compare Match
        TCCR3B = (1 << CS31) | (1 << WGM33); // prescaling by 8
        ICR3 = 20000;                        // top value
        OCR3A = 1500;                        // duty cycle
    }

    // Pin 6 (OC4A) - Timer4
    {
        DDRH |= (1 << PH3);                  // pin 6
        TCCR4A |= (1 << COM4A1);             // Clear OC4A on Compare Match
        TCCR4B = (1 << CS41) | (1 << WGM43); // prescaling by 8
        ICR4 = 20000;                        // top value
        OCR4A = 1500;                        // duty cycle
    }

    // Pin 46 (OC5A) - Timer5
    {
        DDRL |= (1 << PL3);                  // pin 46
        TCCR5A |= (1 << COM5A1);             // Clear OC5A on Compare Match
        TCCR5B = (1 << CS51) | (1 << WGM53); // prescaling by 8
        ICR5 = 20000;                        // top value
        OCR5A = 1500;                        // duty cycle
    }
}


void init_adc()
{
    ADCSRA |= (1 << ADEN) | (1 << ADIE);                  // enable adc and interrupt adc complete
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 125 kHz adc clock frequnecy

    ADMUX |= (1 << REFS0);
    DIDR0 = 0xFF; // turn off the digital i/O
}


void select_ch(char channel)
{
    ADMUX = 0b01000000; // clear to channel 0 before updating
    ADMUX |= channel;
}

 
unsigned int get_sample(char channel)
{
    ADCSRA |= (1 << ADSC); // her start adc'en sin sampling
    while (!(ADCSRA & (1 << ADIF)))
        ;
    return (ADCL + (ADCH << 8));

    /*returns a 10 bit sample from a chosen channel*/
    ADCSRA |= (1 << ADSC); // her start adc'en sin sampling

    // Polling: Venter på sample -> bliver klar ADIF bit bliver høj
    while (!(ADCSRA & (1 << ADIF)))
        ;
    // returnere 10 bit sampleværdi}
    return (ADCL + (ADCH << 8)); // returnere 10 bit sampleværdi
}
