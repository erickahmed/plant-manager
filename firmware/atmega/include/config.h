#ifndef CONFIG_H
#define CONFIG_H

#include <sys/types.h>
#include <stdint.h>
#include <avr/io.h>
#include "eeprom.h"

#define MOISTURE_SENSOR_A0 PC0
//#define MOISTURE_SENSOR_A1 PC1
//#define MOISTURE_SENSOR_A2 PC2
//#define MOISTURE_SENSOR_A3 PC3
//#define MOISTURE_SENSOR_A4 PC4
//#define MOISTURE_SENSOR_A5 PC5
//#define MOISTURE_SENSOR_A6 PC6
//#define MOISTURE_SENSOR_A7 PC7

//#define PUMP_D3 PD3
//#define PUMP_D4 PD4
//#define PUMP_D5 PD5
//#define PUMP_D6 PD6
#define PUMP_D7 PD7

#define TWI_SDA (1 << PD2)
#define TWI_SCL (1 << PD3)

inline void eeprom_init() {
    #define DICT_SIZE 4

    #define TWI_ADDR        0x22
    #define SENSOR_READINGS 0x23
    #define MOISTURE_MIN    0x24
    #define MOISTURE_MAX    0x25

    uint8_t keys[DICT_SIZE] = {
        TWI_ADDR,
        SENSOR_READINGS,
        MOISTURE_MIN,
        MOISTURE_MAX
    };

    uint16_t values[DICT_SIZE] = {
        0x20,
        5,
        400,
        200
    };

    //NOTE: values are not re-initialized if != 0 because they may
    //      have been changed by the esp32 during previous runs
    for (int i = 0; i < DICT_SIZE; i++) if (eepromRead(keys[i]) == 0) eepromWrite(keys[i], values[i]);
}

#endif
