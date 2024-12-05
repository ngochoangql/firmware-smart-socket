#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "lwip/apps/sntp.h"
#include "ntp_time.h"
#include "string.h"
// Địa chỉ server NTP mặc định
#define NTP_SERVER "pool.ntp.org"

static const char *TAG = "NTP Time";

// Hàm khởi tạo SNTP
void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Khởi tạo SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, NTP_SERVER);
    sntp_init();
}

// Hàm lấy thời gian từ NTP server
void obtain_time(void)
{
    initialize_sntp();
    setenv("TZ", "UTC-7", 1);
    tzset();
    // Đợi cho việc lấy thời gian từ server
    time_t now = 0;
    struct tm timeinfo = {0};
    int retry = 0;
    const int retry_count = 10;

    while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Chờ đồng bộ thời gian... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    if (retry == retry_count)
    {
        ESP_LOGE(TAG, "Không thể đồng bộ thời gian sau %d lần thử", retry_count);
        return;
    }

    // Cài đặt múi giờ Việt Nam (UTC+7)

    // Hiển thị thời gian đã đồng bộ thành công
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "Thời gian đã đồng bộ thành công: %s", strftime_buf);

    // Cài đặt thời gian hệ thống
    struct timeval tv = {.tv_sec = now};
    settimeofday(&tv, NULL);

    // Giải phóng tài nguyên sử dụng bởi SNTP client
    sntp_stop();
}

// Hàm in ra thời gian hiện tại
void print_current_time(void)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char strftime_buf[64];
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "Thời gian hiện tại: %s", strftime_buf);
}

// Hàm lấy thời gian hiện tại
time_t get_current_time(void)
{
    time_t now;
    time(&now);
    return now;
}
void get_current_hour_minute(char *buffer, size_t bufsize)
{
    time_t now = get_current_time();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    strftime(buffer, bufsize, "%H:%M", &timeinfo);
}

bool is_current_time_str(char *time_str)
{
    char current_time_str[6]; // Buffer to hold "HH:MM" format
    get_current_hour_minute(current_time_str, sizeof(current_time_str));
    printf("C: %s,S: %s.", current_time_str, time_str);
    return strcmp(current_time_str, time_str) == 0;
}

bool is_today_selected(bool days_selected[7]) {
    // Lấy ngày hiện tại
    time_t currentTime;
    time(&currentTime);
    struct tm *localTime = localtime(&currentTime);

    // Lấy thứ trong tuần (0: Chủ Nhật, 1: Thứ Hai, ..., 6: Thứ Bảy)
    int dayOfWeek = localTime->tm_wday;

    // Chuyển dayOfWeek từ Chủ Nhật = 0 thành Thứ Hai = 0
    dayOfWeek = (dayOfWeek == 0) ? 6 : dayOfWeek - 1;

    // Kiểm tra ngày hôm nay có nằm trong mảng days_selected không
    return days_selected[dayOfWeek];
}