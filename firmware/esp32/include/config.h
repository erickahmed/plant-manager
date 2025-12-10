#ifndef CONFIG_H
#define CONFIG_H

// WI-FI
#define SSID       "your_wifi_ssid"
#define PASSWORD   "your_wifi_password"
#define AUTH_MODE  WIFI_AUTH_WPA2_PSK

// MQTT
#define MQTT_ADDR  "mqtt://yourmqttserver"
#define MQTT_PORT  1883
#define MQTT_USER "your_username"
#define MQTT_PASS "your_password"

// Hardware

// Change based on how many vases max to manage [1 to ~100]
// larger numberss will require larger arrays stored in memory
// see AVR limitation on TWI addresses
#define SLAVES_NUMBER 8


#endif
