#include <avr/io.h>
#include "adcFunctions.h"

void init_adc()
{
    ADCSRA |= (1 << ADEN); // Global ADC enable 
    ADCSRA |= (1 << ADIE); // B: Enable interrupt

    //Set Prescaler 256
    ADCSRA |= (1 << ADPS2);
    ADCSRA |= (1 << ADPS1);
    ADCSRA |= (1 << ADPS0);

    // --- 3. Set Reference Voltage ---
    // Sets the voltage standard used for measurement (e.g., Vcc).
    // ADMUX = 0;
    // ADMUX |= (1 << ADLAR); //
    // ADMUX |= (1 << REFS0);
    DIDR0 = 0xFF; // turn off the digital i/O
}
