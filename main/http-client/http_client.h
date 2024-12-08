#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_http_client.h"
#include "device/device.h"
#include "wifi/wifi.h"

esp_err_t http_event_handler(esp_http_client_event_t *evt);
void send_device_data(device_t device) ;
void post_device_to_api(device_t device);
void post_relay_to_api(device_t device);
#endif //HTTP_CLIENT_H