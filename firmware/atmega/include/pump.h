#ifndef PUMP_H
#define PUMP_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "config.h"


static const int8_t actuatorPins[] = {
    #ifdef PUMP_D3
        PUMP_D3,
    #endif

    #ifdef PUMP_D4
        PUMP_D4,
    #endif

    #ifdef PUMP_D5
        PUMP_D5,
    #endif

    #ifdef PUMP_D6
        PUMP_D6,
    #endif

    #ifdef PUMP_D7
        PUMP_D7,
    #endif
    };

#define ACTUATORS_NUM (sizeof(actuatorPins) / sizeof(actuatorPins[0]))

void pumpsInit(void);
void waterPlant(void);

#endif
