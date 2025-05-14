#ifndef MOISTURE_H
#define MOISTURE_H

#include <stdint.h>
#include <stdbool.h>

void initMoistureSensors(void);
void triggerSensorsRead(void);
int16_t moistureSensorsAverage(int8_t sensorPin);
int16_t readMoisture(void);

extern volatile bool readSensors;

#endif
