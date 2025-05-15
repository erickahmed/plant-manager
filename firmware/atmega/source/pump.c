#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

void pumpsInit(void) {
    // Set as input
    DDRD &= ~((1 << PUMP_D7) |
              (1 << PUMP_D6) |
              (1 << PUMP_D5) |
              (1 << PUMP_D4) |
              (1 << PUMP_D3) |
              0 | 0 | 0 ));
}

void triggerPump(int8_t pump_pin, int16_t milliseconds) {
    // Turn on pump
    PORTD |= (1 << pump_pin);

    // timer for n milliseconds
    // NOTE: watchout for overflows!! create a safety net.

    // Turn off pump
    PORTD &= ~(1 << pump_pin);
}

void waterPlant(void) {
    //for each pump --> turn em on
    //better to have it here as function instead of doing it in main
}
