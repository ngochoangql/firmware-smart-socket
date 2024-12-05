#ifndef WIFI_H
#define WIFI_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "nvs/nvs.h"
#include "tcpip_adapter.h"

void wifi_init();
char* get_ip_address_as_string();

#endif // WIFI_H