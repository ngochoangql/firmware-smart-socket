#ifndef MQTT_H
#define MQTT_H

#include <stdio.h>
#include "string.h"
#include "stdbool.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_event.h"



#include "device/device.h"




void mqtt_app_start(void);
void publish_message(const char *topic, const char *message);
bool get_status_mqtt();
#endif //MQTT_H