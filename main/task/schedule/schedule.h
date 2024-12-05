#ifndef SCHEDULE_H
#define SCHEDULE_H


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"

#include "task-manager/task_manager.h"
#include "ntp/ntp_time.h"
#include "device/device.h"
#include "web-socket/web_socket.h"
void task_schedule(void *pvParameters);

#endif //SCHEDULE_H