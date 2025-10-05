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

ISR(WDT_vect) {
    triggerMoistureRead();
}

ISR(TWI_vect) {
    twiAlert();
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

static uint16_t lastMoistureVal = 0;

// Encapsulate lastMoistureVal to read later from twi
const uint16_t *moisturePtr(void) { return &lastMoistureVal; }

int main(void) {
    setup();

    while(true) {
        if (readSensors) {
            lastMoistureVal = moistureRead();
            // nested if: (result outside range (see config.h)) {waterPlant()}

        }

        readSensors = false;
        enterSleep();
    }
    return -1;
}
