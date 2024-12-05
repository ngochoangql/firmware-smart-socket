#ifndef NTP_TIME_H
#define NTP_TIME_H

#include <time.h>

#ifdef __cplusplus
extern "C"
{
#endif

    void initialize_sntp(void);
    void obtain_time(void);
    void print_current_time(void);
    time_t get_current_time(void);
    void get_current_hour_minute(char *buffer, size_t bufsize);
    bool is_current_time_str(char *time_str);
    bool is_today_selected(bool days_selected[7]);
#ifdef __cplusplus
}
#endif

#endif // NTP_TIME_H
