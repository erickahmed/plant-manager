#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "pump.h"
#include "config.h"

void pumpsInit(void) {
    uint8_t actuatorMask = 0;
    for (uint8_t i = 0; i < ACTUATORS_NUM; i++) {
        actuatorMask |= (1 << actuatorPins[i]);
    }
    DDRD |= actuatorMask;
}

void triggerPump(int8_t pump_pin) {
    // Turn on pump
    PORTD |= (1 << pump_pin);

    // timer for a fixed n milliseconds
    // hardcoded for now

    // Turn off pump
    PORTD &= ~(1 << pump_pin);
}

void waterPlant(void) {
    for (int i=0; i < ACTUATORS_NUM; i++) {
        triggerPump(actuatorPins[i]);
    }


}
