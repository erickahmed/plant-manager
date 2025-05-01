#define MOISTURE_SENSOR_A0 A0

void setup() {
    pinMode(MOISTURE_SENSOR_A0, INPUT);
    Serial.begin(9600);
}

void loop() {
    analogRead(MOISTURE_SENSOR_A0);
    
}
