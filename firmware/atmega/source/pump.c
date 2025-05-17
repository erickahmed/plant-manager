#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "pump.h"
#include "config.h"

void pumpsInit(void) {
    uint8_t actuatorMask = 0;
    for (uint8_t i = 0; i < ACTUATORS_NUM; i++) {
        actuatorMask |= (1 << actuatorPins[i]);
    }
    DDRD |= actuatorMask;
    //PORTD &= ~(1 << actuatorMask);
}

void triggerPump(uint8_t pump_pin) {
    // Turn on pump
    PORTD |= (1 << pump_pin);

    _delay_ms(500);
    // Here there could be the issue of the AVR hanging when pump is on

    // Turn off pump
    PORTD &= ~(1 << pump_pin);
}

void waterPlant(void) {
    for (uint8_t i=0; i < ACTUATORS_NUM; i++) {
        triggerPump(actuatorPins[i]);
    }
}
