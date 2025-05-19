#include <stdint.h>
#include <stdbool.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include "adc.h"
#include "moisture.h"
#include "pump.h"

ISR(WDT_vect) {
    triggerMoistureRead();
}

void watchdogs(void) {
    cli();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
    sei();
}

void enterSleep(void) {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}

void setup(void) {
    adcInit();
    sensorsInit();
    pumpsInit();
    watchdogs();
}

int main(void) {
    setup();

    while(true) {
        if (readSensors) {
            uint16_t result = moistureRead();
            //espSend(VASE_NUM, result)
        }
        // if(espGet("ask if i need to water plants") == true) {waterPlant()}
        // or maybe it's better to do this with interrupts?

        readSensors = false;
        enterSleep();
    }
    return -1;
}
