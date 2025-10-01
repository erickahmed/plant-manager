#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t eepromRead(uint8_t address);
void eepromWrite(uint8_t address, uint8_t data);

#endif
