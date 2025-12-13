#ifndef MAIN_HPP
#define MAIN_HPP

#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

extern volatile bool criticalErrorFlag;
extern EventGroupHandle_t connectivity_event_group;

#define WIFI_CONNECTED_BIT BIT0
#define MQTT_CONNECTED_BIT BIT1

#endif
