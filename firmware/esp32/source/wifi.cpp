#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
//#include "esp_pm.h"
#include "esp_task_wdt.h"
#include "esp_wifi.h"
#include "main.hpp"
#include "wifi-credentials.h"

//check this
wifi_mode_t wifi_mode = WIFI_MODE_STA;
wifi_init_config_ wifi_cfg = {
    .sta = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
        .threshold.authmode = WIFI_AUTH,
    },
};

static void wifiInit() {
    ESP_ERROR_CHECK(esp_wifi_init(*wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_set_mode(wifi_mode));

    esp_wifi_start();
}

static inline void wifiTryConnect() {
    wifiInit();

    if(esp_wifi_get_mode(wifi_mode)) ESP_ERROR_CHECK(esp_wifi_connect());
    else criticalErrorFlag = true;
}

static void wifiManager() {
    if(!esp_wifi_get_mode(wifi_mode)) wifiTryConnect();
}

void wifiTask(void *pvParameters) {
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    wifiManager();

    esp_task_wdt_reset();
    vTaskDelay(pdMS_TO_TICKS(2500));
}
