#ifndef AC_MEASURE_TASK_H
#define AC_MEASURE_TASK_H

#include "esp_log.h"
#include "ac-measure/unit_acmeasure.h"
#include "driver/gpio.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_http_client.h"
#include "http-client/http_client.h"
#include "device/device.h"

typedef struct {
    uint16_t current;
    uint16_t voltage;
    uint16_t power;
    uint16_t apparentPower;
} sensor_data_t;

void acmeasure_task(void *pvParameter);
void acmeasure_init();
extern UNIT_ACMEASURE unit_acmeasure;


#endif // AC_MEASURE_TASK_H