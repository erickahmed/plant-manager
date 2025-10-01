#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "moisture.h"
#include "config.h"
#include "adc.h"

volatile bool readSensors = false;

void sensorsInit(void) {
    uint8_t sensorMask = 0;
    for (uint8_t i = 0; i < SENSORS_NUM; i++) {
        sensorMask |= (1 << sensorPins[i]);
    }
    DDRC &= ~sensorMask;
    PORTC &= ~sensorMask;
}

void triggerMoistureRead(void) {
    readSensors = true;
}

static uint16_t moistureAverage(uint8_t sensorPin) {
    uint16_t sum = 0;
    for (uint8_t i = 0; i < SENSOR_READINGS; i++) {
        sum += adcRead(sensorPin);
        //TODO: implement variance check (if over a certain variance discard measurements for this cycle)
        //TODO IMPORTANT: if value < 1 or > 1022, discard that measurement
    }
    return sum / SENSOR_READINGS;
}

uint16_t moistureRead(void) {
    uint16_t overallSum = 0;

    for (uint8_t i = 0; i < SENSORS_NUM; i++) {
        uint16_t sensorAverage = moistureAverage(sensorPins[i]);
        //TODO: if a sensor gives 1023, discard that sensor entirely (means it is disconnected or not working)
        overallSum += sensorAverage;
    }

    return overallSum / SENSORS_NUM;
}
