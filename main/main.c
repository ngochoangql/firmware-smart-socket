#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

#include "device/device.h"
#include "task/button/button_task.h"
#include "ntp/ntp_time.h"
#include "smartconfig/smart_config.h"
#include "mqtt/mqtt.h"
#include "nvs/nvs.h"
#include "wifi/wifi.h"
#include "web-socket/web_socket.h"
#include "web-server/web_server.h"
#include "task-manager/task_manager.h"
#include "task/ac-measure/ac_measure_task.h"
void app_main(void)
{

    device_init();
    xTaskCreate(&buttonTask, "buttonTask", 4096, NULL, 1, NULL);

    nvs_init();
    initialize_task_manager();
    char wifi_mode[64];
    read_string_from_nvs("WIFI_MODE", wifi_mode, sizeof(wifi_mode));
    if (wifi_mode[0] != '\0')
    {
        if (strcmp(wifi_mode, "STA") == 0)
        {
            wifi_init();
        }
        else if (strcmp(wifi_mode, "AP") == 0)
        {
        }
        else if (strcmp(wifi_mode, "SMART_CONFIG") == 0)
        {
            smart_config_start();
        }
    }
    else
    {
        smart_config_start();
    }

    websocket_app_start();
    web_server_init();
    send_device_data(device);
    // save_string_to_nvs("WIFI_MODE","STA");

    // smart_config_start();
    // mqtt_app_start();
    obtain_time();
    acmeasure_init();

    // int i = 0;
    // Main loop
    while (1)
    {
        // if (i % 2 == 0){
        //     turnOffRelay(&device.relayModules[0]);
        // }else{
        //     turnOnRelay(&device.relayModules[0]);
        // }
        // i++;
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
