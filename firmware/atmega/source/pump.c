#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void pumpsInit(void) {
    // Set as input
    DDRD &= ~((1 << PUMP_D7) |
              (1 << PUMP_D6) |
              (1 << PUMP_D5) |
              (1 << PUMP_D4) |
              (1 << PUMP_D3) |
              0 | 0 | 0 ));
    // Disable pullup
    PORTD &= ~((1 << PUMP_D7) |
               (1 << PUMP_D6) |
               (1 << PUMP_D5) |
               (1 << PUMP_D4) |
               (1 << PUMP_D3) |
               0 | 0 | 0 ));
}

void triggerPump(int8_t pump_pin, int16_t milliseconds) {
    // turn on pump
    // watchdog for n milliseconds
    // turn of pump
}
