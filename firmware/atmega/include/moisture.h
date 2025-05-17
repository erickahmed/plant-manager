#ifndef MOISTURE_H
#define MOISTURE_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

static const int8_t sensorPins[] = {
    #ifdef MOISTURE_SENSOR_A0
        MOISTURE_SENSOR_A0,
    #endif

    #ifdef MOISTURE_SENSOR_A1
        MOISTURE_SENSOR_A1,
    #endif

    #ifdef MOISTURE_SENSOR_A2
        MOISTURE_SENSOR_A2,
    #endif

    #ifdef MOISTURE_SENSOR_A3
        MOISTURE_SENSOR_A3,
    #endif

    #ifdef MOISTURE_SENSOR_A4
        MOISTURE_SENSOR_A4,
    #endif

    #ifdef MOISTURE_SENSOR_A5
        MOISTURE_SENSOR_A5,
    #endif

    #ifdef MOISTURE_SENSOR_A6
        MOISTURE_SENSOR_A6,
    #endif

    #ifdef MOISTURE_SENSOR_A7
        MOISTURE_SENSOR_A7,
    #endif
    };

#define SENSORS_NUM (sizeof(sensorPins) / sizeof(sensorPins[0]))

extern volatile bool readSensors;

void sensorsInit(void);
void triggerMoistureRead(void);
int16_t moistureAverage(int8_t sensorPin);
int16_t moistureRead(void);

#endif
