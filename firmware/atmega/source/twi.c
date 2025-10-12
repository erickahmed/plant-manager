#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "pump.h"
#include "twi.h"

#define BUFFER_SIZE 4

static inline void twiReset(void) {
    // Enable TWI; enable ACK; clear interrupt flag; enable twi external interrupt
    TWCR = (1 << TWEN) | (1 << TWEA) | (1 << TWINT) | (1 << TWIE);
}

void twiInit(void) {
    TWAR = (TWI_ADDR << 1); //address definition found in config.h
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

static void twiRespond(uint8_t buffer[]) {
    switch(buffer[0]) {
        case 0x01: // Send moisture
            twiSendData(*pLastMoistureVal);
            break;
        case 0x02: // Get MOISTURE_MIN
            twiSendData(eepromRead(MOISTURE_MIN));
            break;
        case 0x03: // Get MOISTURE_MAX
            twiSendData(eepromRead(MOISTURE_MAX));
            break;
        case 0x04: // Set parameters
            eepromWrite(MOISTURE_MIN, buffer[1]);
            eepromWrite(MOISTURE_MAX, buffer[2]);
            eepromWrite(SENSOR_READINGS, buffer[3]);
            twiSendAck();
            break;
        case 0x05: // Get SENSOR_READINGS
            twiSendData(eepromRead(SENSOR_READINGS));
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

    /*
        HOW TO SPEAK MASTER'S LANGUAGE:
            Message is saved bit per bit on a buffer array like this:
            [ message, param0, param1, param2 ]

            So when parsing:
            buffer[0] is the message itself
            buffer[1] to buffer[3] are parameters

            TODO: see comment on line 29
    */

    static uint8_t twiBuffer[BUFFER_SIZE] = {0};
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
