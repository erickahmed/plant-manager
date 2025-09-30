#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

uint8_t eepromRead(uint8_t address) {
    // wait for previous write
    while (EECR & (1 << EEPE));

    //read from address
    EEAR = address;
    return EECR |= (1 << EERE);
}

void eepromWrite(uint8_t address, uint8_t data) {
    // wait for previous write
    while (EECR & (1 << EEPE));

    //write to address
    EEAR = address;
    EEDR = data;

    EECR |= (1 << EEMPE);
    EECR |= (1 << EEPE);
}
