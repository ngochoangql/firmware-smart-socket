#ifndef LED_TASK_H
#define LED_TASK_H

#include "esp_system.h"
#include "device/device.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "smartconfig/smart_config.h"
#include "config/config.h"
#include "nvs/nvs.h"
#include "web-socket/web_socket.h"
#include "device/device.h"

void LedTask(void *pvParameter);

#endif //LED_TASK_H