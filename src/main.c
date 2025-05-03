#include <stdint.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "config.h"
#include "moisture.h"

ISR(WDT_vect) {
    readSensors = true;
}

void watchdogs(void) {
    cli();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
    sei();
}

void enterSleep(void) {
    set_sleep_mode(SLEEP_MODE_IDLE);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}

void setup(void) {
    Serial.begin(9600);
    initMoistureSensor();
    watchdogs();
}

void loop(void) {
    if (readSensors) {
        int16_t result = readMoisture();
        if (result != -1) {
            Serial.println(result);
            readSensors = false;
        }
    }

    enterSleep();
}
