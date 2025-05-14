#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "moisture.h"
#include "config.h"
#include "adc.h"

volatile bool readSensors = false;

void initMoistureSensors(void) {
    // Set as input
    DDRC &= ~((1 << MOISTURE_SENSOR_A0) |
              (1 << MOISTURE_SENSOR_A1) |
              (1 << MOISTURE_SENSOR_A2) |
              (1 << MOISTURE_SENSOR_A3) |
              (1 << MOISTURE_SENSOR_A4) |
              (1 << MOISTURE_SENSOR_A5)) ;
              //(1 << MOISTURE_SENSOR_A6) |
              //(1 << MOISTURE_SENSOR_A7));
    // Disable pullup
    PORTC &= ~((1 << MOISTURE_SENSOR_A0) |
               (1 << MOISTURE_SENSOR_A1) |
               (1 << MOISTURE_SENSOR_A2) |
               (1 << MOISTURE_SENSOR_A3) |
               (1 << MOISTURE_SENSOR_A4) |
               (1 << MOISTURE_SENSOR_A5)) ;
               //(1 << MOISTURE_SENSOR_A6) |
               //(1 << MOISTURE_SENSOR_A7));
}

void triggerSensorsRead(void) {
    readSensors = true;
}

int16_t moistureSensorsAverage(int8_t sensorPin) {
    int16_t sum = 0;
    for (int i = 0; i < SENSOR_READINGS; i++) {
        sum += adcRead(sensorPin);
        //TODO: implement variance check (if over a certain variance discard measurements for this cycle)
        //TODO: maybe add a small delay using watchdog here?
        //TODO IMPORTANT: if value < 1 or > 1022, discard that measurement
    }
    return sum / SENSOR_READINGS;
}

int16_t readMoisture(void) {
    uint8_t sensorPins[SENSORS_NUM] = { MOISTURE_SENSOR_A0, MOISTURE_SENSOR_A1,
                                        MOISTURE_SENSOR_A2, MOISTURE_SENSOR_A3,
                                        MOISTURE_SENSOR_A4, MOISTURE_SENSOR_A5 };
                                        //MOISTURE_SENSOR_A6, MOISTURE_SENSOR_A7 };

    int16_t overallSum = 0;

    for (int i = 0; i < SENSORS_NUM; i++) {
        int16_t sensorAverage = moistureSensorsAverage(sensorPins[i]);
        //TODO: if a sensor gives 0 or 1023, discard that sensor entirely (means it is disconnected or not working)
        overallSum += sensorAverage;
    }

    return overallSum / SENSORS_NUM;
}
