#include <stdint.h>
#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "config.h"
#include "adc.h"
#include "moisture.h"
#include "pump.h"
#include "twi.h"

volatile uint16_t lastMoistureVal = 0;
volatile uint16_t *pLastMoistureVal = &lastMoistureVal;
extern bool respondFlag;

ISR(TWI_vect) {
    twiListen();
}

static void watchdogs(void) {
    cli();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
    sei();
}

static inline void enterSleep(void) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}

static void setup(void) {
    adcInit();
    twiInit();
    sensorsInit();
    pumpsInit();
    watchdogs();
}

int main(void) {
    setup();

    pLastMoistureVal = &lastMoistureVal;

    while(true) {
        *pLastMoistureVal = moistureRead();
        // nested if: (result outside range (see config.h)) {waterPlant()}


        if(respondFlag) {
            twiRespond();
            respondFlag = false;
        }

        enterSleep();
    }
    return -1;
}
