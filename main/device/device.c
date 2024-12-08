#include "device.h"

device_t device = {
    .productId = "24082000",
    .deviceMqtt = "mqtt.ohstem.vn",
    .numOfRelay = 3};

static void relay_init(int num)
{
    // Cấu hình GPIO 18 như là đầu ra
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;        // Không dùng ngắt
    io_conf.mode = GPIO_MODE_OUTPUT;              // Chế độ đầu ra
    io_conf.pin_bit_mask = (1ULL << num);         // Chọn GPIO 18
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE; // Không kích hoạt pull-down
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;     // Không kích hoạt pull-up
    gpio_config(&io_conf);
}

void turnOffRelay(relay_t *relay)
{
    if (relay->index == 2)
    {
        gpio_set_level(relay->relayGpio, 1);
    }
    else
    {
        gpio_set_level(relay->relayGpio, 0);
    }
    relay->status = false;
    printf("Turn off Relay %i \n", relay->index);
}
void turnOnRelay(relay_t *relay)
{
    if (relay->index == 2)
    {
        gpio_set_level(relay->relayGpio, 0);
    }
    else
    {
        gpio_set_level(relay->relayGpio, 1);
    }
    relay->status = true;
    printf("Turn on Relay %i \n", relay->index);
}

void device_init()
{
    char buffer[128];
    read_string_from_nvs("httpServer", buffer, sizeof(buffer));

    // Cấp phát bộ nhớ cho device.deviceServer
    device.relayModules[0] = (relay_t){.index = 0, .relayGpio = 22, .status = false};
    device.relayModules[1] = (relay_t){.index = 1, .relayGpio = 23, .status = false};
    device.relayModules[2] = (relay_t){.index = 2, .relayGpio = 33, .status = false};

    for (int i = 0; i < 3; i++)
    {
        relay_init(device.relayModules[i].relayGpio);
    }
    gpio_set_level(33, 1);

    
}
