#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <esp_http_server.h>
#include "task-manager/task_manager.h"
#include "task/schedule/schedule.h"
void web_server_init();

// Khởi tạo HTTP server
httpd_handle_t start_http_server(void);

// Dừng HTTP server
void stop_http_server(httpd_handle_t server);

#endif // WEB_SERVER_H
