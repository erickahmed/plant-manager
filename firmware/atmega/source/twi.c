#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "globals.h"
#include "pump.h"

#define BUFFER_SIZE 4

static inline void twiReset(void) {
    // Enable TWI; enable ACK; clear interrupt flag; enable twi external interrupt
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT) | (1 << TWIE);
}

void twiInit(void) {
    TWAR = TWI_ADDR; //address definition found in config.h
    twiReset();
}

static inline void twiSendAck(void) {
    TWCR = (1 << TWEA) | (1 << TWINT);
}

static inline void twiSendNack(void) {
    TWCR = (TWCR & ~(1 << TWEA)) | (1 << TWINT);
}

static void twiRespond(int8_t buffer[]) {
    switch(buffer[0]) {
        case 0x01: // Send moisture
            twiRespond(*moisturePtr());
            break;
        case 0x02: // Get MOISTURE_MIN
            twiRespond(eepromRead(MOISTURE_MIN));
            break;
        case 0x03: // Get MOISTURE_MAX
            twiRespond(eepromRead(MOISTURE_MAX));
            break;
        case 0x04: // Set parameters (master will always send all three params)
            eepromWrite(MOISTURE_MIN, buffer[1]);
            eepromWrite(MOISTURE_MAX, buffer[2]);
            eepromWrite(SENSOR_READINGS, buffer[3]);
            twiSendAck();
            break;
        case 0x05: // Get SENSOR_READINGS
            twiRespond(eepromRead(SENSOR_READINGS));
            break;
        case 0x06: // Set SENSOR_READINGS
            eepromWrite(SENSOR_READINGS, buffer[1]);
            twiSendAck();
            break;
        case 0x07: // Force water plants (one pumping)
            waterPlant();
            twiSendAck();
            break;
        default:
            twiSendNack();
            break;
    }
}

void twiListen(void) {
    static int8_t twiBuffer[BUFFER_SIZE] = {0};
    static uint8_t bufferIndex = 0;

    switch (TWSR & 0xF8) {
        case 0x60: // SLA+W received
            twiSendAck();
            break;
        case 0x80: // data received
            if(bufferIndex < BUFFER_SIZE) {
                twiBuffer[bufferIndex++] = TWDR;
                twiSendAck();
            } else twiSendNack();
            break;
        case 0xA0: // stop condition
            twiRespond(twiBuffer);
            twiReset();
            break;
        default:
            twiSendNack();
            break;
    }
}
