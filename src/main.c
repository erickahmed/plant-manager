#include <avr/sleep.h>

#define MOISTURE_SENSOR_A0 A0

volatile bool readSensors = false;

ISR(WDT_vect) {
    readSensors = true;
}

void watchdog() {
    cli();
    // Set watchdog timer to 8 seconds
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3);
    sei();
}

void sleep_mode() {
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();
    sleep_cpu();
    sleep_disable();
}

void setup() {
    pinMode(MOISTURE_SENSOR_A0, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (readSensors) {
        uint16_t moisture = analogRead(MOISTURE_SENSOR_A0);
        Serial.println(moisture);

        readSensors = false;
    }
    sleep_mode();
}
