
#ifndef NVS_H
#define NVS_H

#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"



void nvs_init();
void write_nvs(const char *key, int32_t value);
int32_t read_nvs(const char *key);


void save_string_to_nvs(const char *key, const char *value);
void read_string_from_nvs(const char *key, char *buffer, size_t buffer_size);
#endif //NVS_H
