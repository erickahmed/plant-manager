#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "moisture.h"
#include "config.h"
#include "adc.h"

static int16_t moistureReadingSum = 0;
static int8_t moistureReadingCount = 0;

volatile bool readSensors = false;

void initMoistureSensor(void) {
    DDRC &= ~(1 << MOISTURE_SENSOR_A0); // set as input
    PORTC &= ~(1 << MOISTURE_SENSOR_A0); //disable pullup
}

void triggerSensorRead(void) {
    readSensors = true;
}

int16_t readMoisture(void) {
    if (!readSensors)
        return -1;

    adcInit();
    moistureReadingSum += adcRead(MOISTURE_SENSOR_A0);
    moistureReadingCount++;

    //TODO: implement variance check (if over a certain variance discard measurements for this cicle)
    if (moistureReadingCount >= SENSOR_READINGS) {
        int16_t average = moistureReadingSum / SENSOR_READINGS;
        moistureReadingSum = 0;
        moistureReadingCount = 0;
        readSensors = false;
        return average;
    } else readSensors = true;

    return -1;
}
