#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void adcInit(void) {
    // Set reference voltage to AVcc (REFS0 = 1, REFS1 = 0)
    ADMUX = (1 << REFS0);

    // Enable ADC and set prescaler to 128 (for 16 MHz clock, gives 125 kHz ADC clock)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adcRead(uint8_t channel) {
    // Select ADC pin (A0-A7) by setting MUX bits in ADMUX
    // Pins A6 and A7 would make an 8 sensor array possible (for large vases).
    // Achievable only when using the 328p as a DIP, through port C (PC6, PC7)
    ADMUX = (ADMUX & 0xF8) | (channel & 0x07);

    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete (ADSC becomes '0' when done)
    while (ADCSRA & (1 << ADSC));

    // Read ADC (reading order: ADCL --> ADCH)
    return ADC;
}
