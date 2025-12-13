#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include "config.h"
#include "pump.h"
#include "twi.h"

#define BUFFER_SIZE 4

static uint8_t twiBuffer[BUFFER_SIZE] = {0};
static uint8_t bufferIndex = 0;
bool respondFlag = false;

static inline void twiReset(void) {
    // Enable TWI; enable ACK; clear interrupt flag; enable twi external interrupt
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT) | (1 << TWIE);
}

void twiInit(void) {
    TWAR = ((eepromRead(TWI_ADDR)) << 1); //address definition found in config.h
    twiReset();
}

static inline void twiSendAck(void) {
    TWCR = (1 << TWEA) | (1 << TWINT);
}

static inline void twiSendNack(void) {
    TWCR = (TWCR & ~(1 << TWEA)) | (1 << TWINT);
}

static void twiSendData(uint8_t data) {
    TWDR = data;
    twiSendAck();
}

void twiRespond(void) {
    if (!respondFlag) return;

    uint8_t cmd   = twiBuffer[0];
    uint8_t param = twiBuffer[1];

    switch(cmd) {
        // READ
        case 0x01:
            twiSendData(*pLastMoistureVal);
            break;
        case 0x02:
            twiSendData(eepromRead(SENSOR_READINGS));
            break;
        case 0x03:
            twiSendData(eepromRead(MOISTURE_MIN));
            break;
        case 0x04:
            twiSendData(eepromRead(MOISTURE_MAX));
            break;
        // WRITE
        case 0x05:
            eepromWrite(SENSOR_READINGS, param);
            twiSendAck();
            break;
        case 0x06:
            eepromWrite(MOISTURE_MIN, param);
            twiSendAck();
            break;
        case 0x07:
            eepromWrite(MOISTURE_MAX, param);
            twiSendAck();
            break;
        case 0x08:
            waterPlant();
            twiSendAck();
            break;
        default:
            twiSendNack();
            break;
    }
}


void twiListen(void) {
    switch (TWSR & 0xF8) {
        case 0x60: // SLA+W received
            bufferIndex = 0;
            twiSendAck();
            break;
        case 0x80: // data received
            if(bufferIndex < BUFFER_SIZE) {
                twiBuffer[bufferIndex++] = TWDR;
                twiSendAck();
            } else twiSendNack();
            break;
        case 0xA0: // stop condition
            respondFlag = true;
            twiReset();
            break;
        default:
            twiSendNack();
            break;
    }
}
