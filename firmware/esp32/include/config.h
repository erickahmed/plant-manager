#ifndef CONFIG_H
#define CONFIG_H

#pragma once

// WI-FI
#define SSID       "your_wifi_ssid"
#define PASSWORD   "your_wifi_password"
#define AUTH_MODE  WIFI_AUTH_WPA2_PSK

// MQTT
#define MQTT_ADDR  "mqtt://yourmqttserver"
#define MQTT_PORT  1883
#define MQTT_USER "your_username"
#define MQTT_PASS "your_password"

#define CMD_GET_MOISTURE        0x01
#define CMD_GET_SENSOR_READS    0x02
#define CMD_GET_MOISTURE_MIN    0x03
#define CMD_GET_MOISTURE_MAX    0x04
#define CMD_SET_SENSOR_READS    0x05   // [cmd, value]
#define CMD_SET_PARAMS          0x06   // [cmd, min, max, reads]
#define CMD_FORCE_PUMP          0x07   // [cmd]

// Change based on how many vases max to manage [1 to ~100]
// larger numberss will require larger arrays stored in memory
// see AVR limitation on TWI addresses
#define SLAVES_NUMBER 8


#endif
