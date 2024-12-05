#ifndef SMPL_TASK_MANAGER_H
#define SMPL_TASK_MANAGER_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "string.h"


#include "esp_log.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

#include "ntp/ntp_time.h"

#define MAX_TASKS 100

typedef struct
{
    TaskHandle_t handle;
    char id[40];
    char task_name[50];
    void (*task_function)(void *);
    char time_24h[6];     // e.g., "13:45"
    char code[14];
    // 0101010000000
    // Số thứ nhất          0 : inactive ,  1 : active
    // Số thứ hai           0 : close ,     1 : open 
    // Số thứ ba-năm        0 : not chosen, 1 : chosen  (R0 - R2)
    // Số thứ sáu           0 : once,       1 : weekend  
    // Số thứ bảy-mười ba  0 : not chosen, 1 : chosen  (Mon - Sun)
} task_info_t;

void initialize_task_manager();
void create_task(const char *task_id, const char *task_name, void (*task_function)(void *), uint32_t stack_size_words, task_info_t task_info, UBaseType_t priority);
void delete_task(const char *task_id);


void delete_all_tasks_to_nvs();
void save_tasks_to_nvs();
void load_tasks_from_nvs();
void start_loaded_tasks();

#endif /* SMPL_TASK_MANAGER_H */