#ifndef MOISTURE_H
#define MOISTURE_H

#include <stdint.h>
#include <stdbool.h>

void sensorsInit(void);
void triggerRead(void);
int16_t moistureAverage(int8_t sensorPin);
int16_t moistureRead(void);

extern volatile bool readSensors;

#endif
