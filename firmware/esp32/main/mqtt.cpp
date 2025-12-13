#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "stdlib.h"
#include "string.h"
#include "mqtt_client.h"
#include "main.hpp"
#include "i2c.hpp"
#include "config.h"
#include <cstdint>

#define MQTT_TASK_TIMEOUT_MS 3000
#define DEFAULT_SLAVE_ADDR 0x30

static const char* TAG = "MQTT";

static esp_mqtt_client_handle_t mqtt_client = NULL;

static char rx_buffer[50];

static void mqtt_subscribe(void) {
    esp_mqtt_client_subscribe(mqtt_client, "/plant/moisture/qos0", 0);
    esp_mqtt_client_subscribe(mqtt_client, "/plant/humidity/qos0", 0);
}

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event =(esp_mqtt_event_handle_t)event_data;

    switch(event->event_id) {
        case MQTT_EVENT_CONNECTED:
            xEventGroupSetBits(connectivity_event_group, MQTT_CONNECTED_BIT);
            ESP_LOGV(TAG, "Connected to broker");
            mqtt_subscribe();
            ESP_LOGV(TAG, "Subscribed to topic");
            break;

        case MQTT_EVENT_DISCONNECTED:
            xEventGroupClearBits(connectivity_event_group, MQTT_CONNECTED_BIT);
            ESP_LOGW(TAG, "Disconnected from broker");
            break;

        case MQTT_EVENT_DATA:
        {
            ESP_LOGD(TAG, "Data received");

            int len = event->data_len;
            if(len > sizeof(rx_buffer) - 1) len = sizeof(rx_buffer) - 1;

            memcpy(rx_buffer, event->data, len);
            rx_buffer[len] = '\0';

            break;
        }

        case MQTT_EVENT_ERROR:
            ESP_LOGE(TAG, "Error event");
            if(event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) ESP_LOGE(TAG, "Network Error: %s", strerror(event->error_handle->esp_transport_sock_errno));
            else if(event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED) ESP_LOGE(TAG, "Connection Refused! Reason code: 0x%x", event->error_handle->connect_return_code);
            break;

        default:
            break;
    }
}

static void mqtt_init(void) {
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.broker.address.uri = MQTT_ADDR;
    mqtt_cfg.credentials.username = MQTT_USER;
    mqtt_cfg.credentials.authentication.password = MQTT_PASS;

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    if(mqtt_client == NULL) {
        ESP_LOGE(TAG, "esp_mqtt_client_init returned NULL");
        criticalErrorFlag = true;
    }

    esp_mqtt_client_register_event(mqtt_client, (esp_mqtt_event_id_t)ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    esp_mqtt_client_start(mqtt_client);
}

void mqtt_publish(void) {}

void mqttTask(void *pvParameters) {
    ESP_LOGV(TAG, "Task started");

    ESP_LOGV(TAG, "Waiting for Wi-Fi before initializing server");
    xEventGroupWaitBits(connectivity_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    ESP_LOGV(TAG, "Server initialized");

    mqtt_init();
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    for(;;) {
        ESP_LOGV(TAG, "Checking Wi-Fi connection...");
        EventBits_t bits = xEventGroupWaitBits(connectivity_event_group, WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT, pdFALSE, pdTRUE, pdMS_TO_TICKS(MQTT_TASK_TIMEOUT_MS));

        if ((bits & (WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT)) == (WIFI_CONNECTED_BIT | MQTT_CONNECTED_BIT)) {
            ESP_LOGV(TAG, "Wi-Fi and MQTT up");

            uint8_t params[3] = {0};
            uint8_t param_len = 0;

            uint8_t i2c_cmd = 0;
            uint8_t slave_addr = DEFAULT_SLAVE_ADDR;

            char *action = strtok(rx_buffer, ":");
            char *target = strtok(NULL, ":");
            char *value  = strtok(NULL, ":");
            char *slave  = strtok(NULL, ":");

            if(slave) slave_addr = (uint8_t)strtol(slave, NULL, 0);

            if(action && target) {
                if (strcmp(action, "READ") == 0) {
                    if (strcmp(target, "moisture") == 0) i2c_cmd = CMD_GET_MOISTURE;
                    else if (strcmp(target, "eeprom") == 0 && value) {
                        if (strcmp(value, "sensor_reads") == 0) i2c_cmd = CMD_GET_SENSOR_READS;
                        else if (strcmp(value, "moisture_min") == 0) i2c_cmd = CMD_GET_MOISTURE_MIN;
                        else if (strcmp(value, "moisture_max") == 0) i2c_cmd = CMD_GET_MOISTURE_MAX;
                    }
                } else if (strcmp(action, "WRITE") == 0) {
                    if (strcmp(target, "force") == 0) i2c_cmd = CMD_FORCE_PUMP;
                    else if (strcmp(target, "eeprom") == 0 && value) {
                        if (strncmp(value, "params:", 7) == 0) {
                            sscanf(value + 7, "%hhu,%hhu,%hhu", &params[0], &params[1], &params[2]);
                            i2c_cmd = CMD_SET_PARAMS;
                            param_len = 3;
                        }
                        else {
                            uint8_t v = (uint8_t)atoi(value);

                            if (strcmp(value, "sensor_reads") == 0) {
                                i2c_cmd = CMD_SET_SENSOR_READS;
                                params[0] = v;
                                param_len = 1;
                            }
                        }
                    }
                }
            }

            uint32_t packet = (i2c_cmd<<24) | (slave_addr<<16) | params[0];
            xTaskNotify(i2c_task, packet, eSetValueWithOverwrite);
        } else ESP_LOGV(TAG, "Wi-Fi or MQTT down");

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGV(TAG, "Task reset");
        vTaskDelay(pdMS_TO_TICKS(3000));
    }
    ESP_LOGW(TAG, "Task exited");
}
