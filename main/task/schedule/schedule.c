#include "schedule.h"
char *change_first_char_to_1(const char *code)
{
    if (code == NULL || strlen(code) == 0)
    {
        printf("Invalid code!\n");
        return NULL; // Trả về NULL nếu chuỗi không hợp lệ
    }

    // Cấp phát bộ nhớ cho chuỗi mới
    char *new_code = (char *)malloc(strlen(code) + 1); // +1 cho ký tự null-terminator
    if (new_code == NULL)
    {
        printf("Memory allocation failed!\n");
        return NULL; // Trả về NULL nếu không thể cấp phát bộ nhớ
    }

    // Sao chép chuỗi cũ vào chuỗi mới
    strcpy(new_code, code);

    // Thay đổi ký tự đầu tiên thành '1'
    new_code[0] = '1';

    return new_code; // Trả về chuỗi mới
}

void parse_code(const char *code, bool *is_active, bool *is_open, bool *relay_chosen, bool *is_once, bool *days_selected)
{
    if (strlen(code) != 13)
    {
        printf("Invalid code length!\n");
        return;
    }

    // Phân tích từng phần của code
    *is_active = (code[0] == '1');
    *is_open = (code[1] == '1');

    // Chọn relay (R0 - R2)
    relay_chosen[0] = (code[2] == '1'); // R0
    relay_chosen[1] = (code[3] == '1'); // R1
    relay_chosen[2] = (code[4] == '1'); // R2

    // Phân tích chế độ lặp lại (once hoặc weekend)
    *is_once = (code[5] == '0'); // '0' là once, '1' là weekend

    // Chọn các ngày trong tuần (Mon - Sun)
    for (int i = 0; i < 7; i++)
    {
        days_selected[i] = (code[6 + i] == '1');
    }
}

void task_schedule(void *pvParameters)
{
    task_info_t *schedule = (task_info_t *)pvParameters;

    bool is_active;
    bool is_open;
    bool relay_chosen[3]; // R0, R1, R2
    bool is_once;
    bool days_selected[7]; // Mon - Sun

    parse_code(schedule->code, &is_active, &is_open, relay_chosen, &is_once, days_selected);
    char time_24[6];
    strcpy(time_24, schedule->time_24h);
    bool complete = false;
    int count = 5;
    while (1)
    {
        // esp_task_wdt_reset();
        char current_time_str[6]; // Buffer to hold "HH:MM" format
        get_current_hour_minute(current_time_str, sizeof(current_time_str));

        ESP_LOGI("HTTP_SERVER_TAG", "Current time: %s, Scheduled time: %s", current_time_str, time_24);
        // printf("%s  \n: ",schedule->time_24h);
        // Kiểm tra xem thời gian hiện tại có trùng với thời gian đặt lịch không
        if (strcmp(current_time_str, time_24) == 0 && !complete && is_active)
        {
            for (int i = 0; i < 3; i++)
            {
                if (relay_chosen[i] && (is_once || (!is_once && is_today_selected(days_selected))))
                {
                    if (is_open)
                    {
                        if (!device.relayModules[i].status)
                        {
                            turnOnRelay(&device.relayModules[i]);
                            handle_update_relay_from_device(i + 1, 1);
                            ESP_LOGI("HTTP_SERVER_TAG", "Turn ON RELAY at Time %s", current_time_str);
                        }
                    }
                    else
                    {
                        if (device.relayModules[i].status)
                        {
                            turnOffRelay(&device.relayModules[i]);
                            handle_update_relay_from_device(i + 1, 0);
                            ESP_LOGI("HTTP_SERVER_TAG", "Turn OFF RELAY at Time %s", current_time_str);
                        }
                    }
                }
            }

            if (is_once)
            {
                char *new_id = (char *)malloc(strlen(schedule->id) + 1); // +1 để chứa ký tự '\0'

            
                if (new_id == NULL)
                {
                    printf("Memory allocation failed!\n");
                    return; // Nếu không cấp phát được bộ nhớ, thoát
                }
                 strcpy(new_id, schedule->id);
               
              
                printf("After: %s\n", new_id);
                handle_update_schedule(new_id);
                  

                // Xóa task nếu là lặp một lần
                ESP_LOGI("HTTP_SERVER_TAG", "Deleting task with ID: %s", schedule->id);
                complete = true;
                // Đảm bảo chuỗi dest kết thúc bằng ký tự null
            }
        }

        // complete = true;

        if (complete)
        {
            if (count == 0)
            {
                break;
            }
            count--;
        }
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay 1 giây
    }
    delete_task(schedule->id);
    vTaskDelete(NULL);
}