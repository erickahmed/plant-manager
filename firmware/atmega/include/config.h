#ifndef CONFIG_H
#define CONFIG_H

//#define VASE_NUM 0

#define MOISTURE_SENSOR_A0 PC0
#define MOISTURE_SENSOR_A1 PC1
#define MOISTURE_SENSOR_A2 PC2
#define MOISTURE_SENSOR_A3 PC3
#define MOISTURE_SENSOR_A4 PC4
#define MOISTURE_SENSOR_A5 PC5
// NOTE: PC6 PC7 are not accessible by default on Arduino Uno R3
// You need to use the ATMega328p on a custom PCB and use pins 1 and 2
//#define MOISTURE_SENSOR_A6 PC6
//#define MOISTURE_SENSOR_A7 PC7

// You may just connect the pumps that actually are needed in the vase
// or define only the connected pumps on your system
#define PUMP_D3 PD3
#define PUMP_D4 PD4
#define PUMP_D5 PD5
#define PUMP_D6 PD6
#define PUMP_D7 PD7

#define SENSOR_READINGS 5
#define SENSORS_NUM 6

#endif
