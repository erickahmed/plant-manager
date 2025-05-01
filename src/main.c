#include <avr/sleep.h>
#include <avr/wdt.h>

#define MOISTURE_SENSOR_A0 A0

volatile bool readSensors = false;

ISR(WDT_vect) {
    readSensors = true;
}

void watchdogs() {
    cli();
    MCUSR &= ~(1 << WDRF);
    WDTCSR |= (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);
    sei();
}

void enterSleep() {
    // Arduino disables USB when sleeping
    // if using Serial, use flag : SLEEP_MODE_IDLE
    // otherwise for deep sleep  : SLEEP_MODE_PWR_DOWN
    set_sleep_mode(SLEEP_MODE_IDLE);

    sleep_enable();
    sleep_cpu();
    sleep_disable();
}

void setup() {
    pinMode(MOISTURE_SENSOR_A0, INPUT);
    Serial.begin(9600);
    watchdogs();
}

void loop() {
    if (readSensors) {
        uint16_t moisture = analogRead(MOISTURE_SENSOR_A0);
        Serial.println(moisture);
        readSensors = false;
    }
    enterSleep();
}
