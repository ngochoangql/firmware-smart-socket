#include "led_task.h"

void togleLed(relay_t *relay)
{
    if (relay->status)
    {
        gpio_set_level(32, 1);
        vTaskDelay(pdMS_TO_TICKS(400));
        gpio_set_level(32, 0);
        vTaskDelay(pdMS_TO_TICKS(400));
    }
}
void LedTask(void *pvParameter)
{
    gpio_pad_select_gpio(32);
    gpio_set_direction(32, GPIO_MODE_OUTPUT);
    gpio_set_pull_mode(32, GPIO_PULLUP_ONLY); // Nút nhấn có điện trở kéo lên
    gpio_set_level(32, 1);
    while (1)
    {
        togleLed(&device.relayModules[0]);
        togleLed(&device.relayModules[1]);
        togleLed(&device.relayModules[2]);
        vTaskDelay(pdMS_TO_TICKS(1200)); // Đọc dữ liệu mỗi 1.2 giây
    }
}