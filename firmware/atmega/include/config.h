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

inline void init() {
    #define DICT_SIZE 7

    #define TWI_SDA         0x20
    #define TWI_SCL         0x21
    #define TWI_ADDR        0x22
    #define SENSOR_READINGS 0x23
    #define VASE_NUM        0x24
    #define MOISTURE_MIN    0x25
    #define MOISTURE_MAX    0x26

    uint8_t keys[DICT_SIZE] = {
        TWI_SDA,
        TWI_SCL,
        TWI_ADDR,
        SENSOR_READINGS,
        VASE_NUM,
        MOISTURE_MIN,
        MOISTURE_MAX
    };

    uint16_t values[DICT_SIZE] = {
        (1 << PD2),
        (1 << PD3),
        0x20,
        5,
        0,
        400,
        200
    };

    for (int i = 0; i < DICT_SIZE; i++) {
        if (eepromRead(keys[i]) != 0xFF) eepromWrite(keys[i], values[i]);
    }
}

#endif
