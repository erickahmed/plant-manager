#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"

void pumpsInit(void);
void pumpsTrigger(uint8_t digital_pins, int16_t milliseconds);
