#ifndef MOISTURE_H
#define MOISTURE_H

#include <stdint.h>
#include <stdbool.h>

void initMoistureSensor(void);
int16_t readMoisture(void);
void triggerSensorRead(void);

extern volatile bool readSensors;

#endif
