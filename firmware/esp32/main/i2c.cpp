#include <cstdint>
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
#include "driver/i2c_master.h"
#include "esp_log.h"
#include "config-erick.h"

#define I2C_TASK_TIMEOUT_MS 4500

// for ESP32-C3
#define I2C_MASTER_SCL_IO ((gpio_num_t)9)
#define I2C_MASTER_SDA_IO ((gpio_num_t)8)

#define DATA_LENGTH 100

static const char* TAG = "I2C";

TaskHandle_t i2c_task = NULL;

i2c_master_bus_handle_t bus_handle;
static i2c_master_dev_handle_t slave_handles[SLAVES_NUMBER];
uint8_t slave_count = 0;
uint8_t slave_addrs[SLAVES_NUMBER];

uint8_t data_rd[DATA_LENGTH];

static void register_slave(uint8_t addr) {
    i2c_device_config_t slave_dev_cfg = {};
    slave_dev_cfg.dev_addr_length = I2C_ADDR_BIT_LEN_7;
    slave_dev_cfg.device_address = addr;
    slave_dev_cfg.scl_speed_hz = 100000;
    ESP_ERROR_CHECK(i2c_master_bus_add_device(bus_handle, &slave_dev_cfg, &slave_handles[slave_count]));
    slave_addrs[slave_count++] = addr;
}

static void i2c_init(void) {
    i2c_master_bus_config_t i2c_mst_config = {};
    i2c_mst_config.i2c_port = I2C_NUM_0;
    i2c_mst_config.scl_io_num = I2C_MASTER_SCL_IO;
    i2c_mst_config.sda_io_num = I2C_MASTER_SDA_IO;
    i2c_mst_config.clk_source = I2C_CLK_SRC_DEFAULT;
    i2c_mst_config.glitch_ignore_cnt = 7;
    i2c_mst_config.intr_priority = 1;
    i2c_mst_config.trans_queue_depth = 4;
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    register_slave(0x30);
    register_slave(0x31);
}

static void i2c_send(uint32_t packet) {
    uint8_t cmd = (packet >> 24) & 0xFF;
    uint8_t slave_addr = (packet >> 16) & 0xFF;
    uint8_t param = packet & 0xFF;

    i2c_master_dev_handle_t dev = NULL;
    for(uint8_t i = 0; i < slave_count; i++) {
        if(slave_addrs[i] == slave_addr) {
            dev = slave_handles[i];
            break;
        }
    }
    if(!dev) { ESP_LOGE(TAG,"Slave 0x%02X not found", slave_addr); return; }

    uint8_t tx[2] = {cmd, param};
    esp_err_t err;
    switch(cmd) {
        case CMD_GET_MOISTURE:
        case CMD_GET_SENSOR_READS:
        case CMD_GET_MOISTURE_MIN:
        case CMD_GET_MOISTURE_MAX: {
            uint8_t rx;
            err = i2c_master_transmit_receive(dev, tx, 2, &rx, 1, 250);
            if(err == ESP_OK) ESP_LOGI(TAG,"I2C READ cmd=0x%02X val=%u", cmd, rx);
            break;
        }
        default:
            err = i2c_master_transmit(dev, tx, 2, 250);
            break;
    }
    if(err != ESP_OK) ESP_LOGE(TAG,"I2C transaction failed: %s", esp_err_to_name(err));
}

void i2cTask(void *pvParameters) {
    i2c_init();
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));

    uint32_t packet;
    for(;;) {
        if(xTaskNotifyWait(0, 0, &packet, pdMS_TO_TICKS(I2C_TASK_TIMEOUT_MS)) == pdPASS) {
            i2c_send(packet);
        }
        ESP_ERROR_CHECK(esp_task_wdt_reset());
        vTaskDelay(pdMS_TO_TICKS(150));
    }
}
