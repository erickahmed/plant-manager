#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_task_wdt.h"
#include "esp_event.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "main.hpp"
//#include "i2c.hpp"
#include "config-erick.h"

#define MQTT_TASK_TIMEOUT_MS 3000

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
            if(len > sizeof(global_rx_buffer) - 1) len = sizeof(global_rx_buffer) - 1;

            memcpy(global_rx_buffer, event->data, len);
            global_rx_buffer[len] = '\0';
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
    mqtt_cfg.broker.address.port = MQTT_PORT;
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



        } else ESP_LOGV(TAG, "Wi-Fi or MQTT down");

        ESP_ERROR_CHECK(esp_task_wdt_reset());
        ESP_LOGV(TAG, "Task reset");
    }
}
