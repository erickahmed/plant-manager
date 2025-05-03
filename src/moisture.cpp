#include <Arduino.h>
#include <stdint.h>
#include "moisture.h"
#include "config.h"

static int16_t moistureReadingSum = 0;
static int8_t moistureReadingCount = 0;

volatile bool readSensors = false;

void initMoistureSensor(void) {
    pinMode(MOISTURE_SENSOR_A0, INPUT);
}

void triggerSensorRead(void) {
    readSensors = true;
}

int16_t readMoisture(void) {
    if (!readSensors)
        return -1;

    moistureReadingSum += analogRead(MOISTURE_SENSOR_A0);
    moistureReadingCount++;

    if (moistureReadingCount >= SENSOR_READINGS) {
        int16_t avg = moistureReadingSum / SENSOR_READINGS;
        //Serial.println(avg);
        moistureReadingSum = 0;
        moistureReadingCount = 0;
        readSensors = false;
        return avg;
    } else readSensors = true;

    return -1;
}
