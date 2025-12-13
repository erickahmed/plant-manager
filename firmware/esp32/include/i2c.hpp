#ifndef I2C_HPP
#define I2C_HPP

#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern TaskHandle_t i2c_task;

void i2cTask(void *pvParameters);

#endif
