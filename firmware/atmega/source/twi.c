#include <stdint.h>
#include <avr/io.h>
#include "config.h"
#include "globals.h"

#define BUFFER_SIZE 8

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

static void twiRespond(int8_t response[]) {
    // Respond to TWI requests
}

static void twiExecute(int8_t buffer[]) {
    switch(buffer[0]) {
        case 0x01: // Send moisture
            twiRespond(*moisturePtr());
            break;
        case 0x02:
            // Perform action for command 0x02
            break;
        default:
            // twiRespond with unkown command
            break;
    }
    // if needed:
    // twiRespond();
}

void twiAlert(void) {
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
            twiExecute(twiBuffer);
            twiReset();
            break;
        default:
            twiSendNack();
            break;
    }
}
