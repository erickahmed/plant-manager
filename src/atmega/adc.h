#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

void adcInit(void);
uint16_t adcRead(uint8_t channel);

#endif
