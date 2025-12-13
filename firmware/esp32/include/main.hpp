   #ifndef MAIN_HPP
   #define MAIN_HPP

#pragma once

   extern volatile bool criticalErrorFlag;
   extern EventGroupHandle_t connectivity_event_group;

   #define WIFI_CONNECTED_BIT BIT0
   #define MQTT_CONNECTED_BIT BIT1

   #endif
