#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "I2C.h" //include library for i2c driver
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ssd1306.h"      //include display driver
#include "spiFunctions.h" // vores funktioner

volatile uint8_t buttonFlag4 = 0;

/**
 * @brief initilaises ports F, E, K, G,
 */
void InitPorts()
{
    PORTF |= 0xFF;
    PORTE |= 0xFF;
    PORTK |= 0xFF;
    DDRG |= 0b00100000; // D4 as output
}

/**
 * @brief initilise external interrupts!
 */
void InitInterrupts()
{
    EICRB |= (1 << ISC41) | (1 << ISC40); // aktiver på rising edge
    EIMSK |= (1 << INT4);                 // enable interrupt 4
    sei();
}

int main2(void)
{
    InitPorts();
    InitInterrupts();
    I2C_Init();          // initialize i2c interface to display
    InitializeDisplay(); // initialize  display
    clear_display();     // use this before writing you own text

    SPI_MasterInit();

    _i2c_address = 0X78; // write address for i2c interface

    while (1)
    {
        // waits and then resets button
        _delay_ms(1000);

        // assert SS low (active)
        PORTB &= ~(1 << PB0);

        SPI_MasterTransfer(0x00);

        // deassert SS high
        PORTB |= (1 << PB0);
    }
}
