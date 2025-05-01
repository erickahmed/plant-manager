#include <avr/sleep.h>

#define MOISTURE_SENSOR_A0 A0

volatile bool readSensors = false;

ISR(WDT_vect) {
    readSensorsPtr = true;
}

void watchdog() {
    cli();
    // Set watchdog timer to 8 seconds
    WDTCSR = (1 << WDCE) | (1 << WDE);
    WDTCSR = (1 << WDIE) | (1 << WDP3);
    sei();
}

void setup() {
    pinMode(MOISTURE_SENSOR_A0, INPUT);
    Serial.begin(9600);
}

void loop() {
    if (readSensorsPtr) {
        uint16_t moisture = analogRead(MOISTURE_SENSOR_A0);
        Serial.println(moisture);

        readSensorsPtr = false;
    }



}
