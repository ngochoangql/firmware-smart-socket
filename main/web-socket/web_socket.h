#ifndef WEB_SOCKET_H
#define WEB_SOCKET_H

#include "esp_websocket_client.h"
#include "esp_event.h"
#include "esp_log.h"
#include "device/device.h"
#include "cJSON.h"
#include "task-manager/task_manager.h"
#include "task/schedule/schedule.h"

void websocket_app_start(void);
void handle_update_schedule(char *id);
void handle_update_relay_from_device(int relayNumber,int status);

extern esp_websocket_client_handle_t client;
#endif // WEB_SOCKET_H