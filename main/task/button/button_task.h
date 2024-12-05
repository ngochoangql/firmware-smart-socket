#ifndef BUTTON_TASK_H
#define BUTTON_TASK_H

#include "esp_system.h"
#include "device/device.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "smartconfig/smart_config.h"
#include "config/config.h"
#include "nvs/nvs.h"

typedef enum
{
    BUTTON_IDLE,    // Trạng thái không có sự kiện
    BUTTON_PRESSED, // Nút nhấn đã được nhấn
    BUTTON_HOLD     // Nút nhấn đang được giữ
} button_state_t;

typedef struct
{
    int buttonGpio;
    button_state_t buttonState;
} button_t;

void buttonTask(void *pvParameter);

#endif //BUTTON_TASK_H