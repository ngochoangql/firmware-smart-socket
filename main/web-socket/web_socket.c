#include "web_socket.h"
static const char *WEBSOCKET_URL = "ws://172.20.10.8:8080/websocket";
static const char *TAG = "WebSocket";
esp_websocket_client_handle_t client;

void handle_update_schedule(char *id)
{
    // Tạo thông điệp JSON để gửi thông báo
    char register_message[256];
    snprintf(register_message, sizeof(register_message),
             "{\"event\":\"notify\",\"productId\":\"24082000\",\"data\":{\"event\":\"notifyUpdateSchedule\",\"id\":\"%s\"}}",
             id); // Sử dụng các giá trị đã lấy trước đó

    ESP_LOGI(TAG, "Sending register message: %s", register_message);

    // Gửi thông điệp
    esp_err_t err = esp_websocket_client_send_text(client, register_message, strlen(register_message), portMAX_DELAY);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Register message sent successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send register message: %s", esp_err_to_name(err));
    }
}

void handle_update_relay_from_device(int relayNumber, int status)
{
    // Tạo thông điệp JSON để gửi thông báo
    char register_message[256];
    snprintf(register_message, sizeof(register_message),
             "{\"event\":\"notify\",\"productId\":\"24082000\",\"data\":{\"event\":\"notifyRelay\",\"relayNumber\":%d,\"status\":%d}}",
             relayNumber, status); // Sử dụng các giá trị đã lấy trước đó

    ESP_LOGI(TAG, "Sending register message: %s", register_message);

    // Gửi thông điệp
    esp_err_t err = esp_websocket_client_send_text(client, register_message, strlen(register_message), portMAX_DELAY);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Register message sent successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send register message: %s", esp_err_to_name(err));
    }
}

// Hàm điều khiển relay
void update_relay(int relay_index, int relay_state)
{
    if (relay_state == 1)
    {
        ESP_LOGI("RELAY_CONTROL", "Relay %d turned ON", relay_index);
        // Thực hiện bật relay tại relay_index
        turnOnRelay(&device.relayModules[relay_index]);
    }
    else
    {
        ESP_LOGI("RELAY_CONTROL", "Relay %d turned OFF", relay_index);
        // Thực hiện tắt relay tại relay_index
        turnOffRelay(&device.relayModules[relay_index]);
    }
}

// Hàm xử lý sự kiện updateRelay
void handle_update_relay(cJSON *json)
{
    cJSON *relayNumber = cJSON_GetObjectItemCaseSensitive(json, "relayNumber");
    cJSON *id = cJSON_GetObjectItemCaseSensitive(json, "id");
    cJSON *status = cJSON_GetObjectItemCaseSensitive(json, "status");

    if (cJSON_IsNumber(relayNumber) && cJSON_IsString(id) && cJSON_IsBool(status))
    {
        int relayNum = relayNumber->valueint;
        const char *relayId = id->valuestring;
        bool relayStatus = status->valueint;
        update_relay(relayNum - 1, relayStatus);

        ESP_LOGI(TAG, "Relay Number: %d, ID: %s, Status: %d", relayNum, relayId, relayStatus);

        // Tạo thông điệp JSON để gửi
        char register_message[256]; // Cung cấp đủ không gian cho thông điệp
        snprintf(register_message, sizeof(register_message),
                 "{\"event\":\"notify\",\"productId\":\"24082000\",\"data\":{\"event\":\"notifyRelay\",\"relayNumber\":%d,\"status\":%d}}",
                 relayNum, relayStatus);

        ESP_LOGI(TAG, "Sending register message: %s", register_message);

        // Gửi thông điệp
        esp_err_t err = esp_websocket_client_send_text(client, register_message, strlen(register_message), portMAX_DELAY);
        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "Register message sent successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send register message: %s", esp_err_to_name(err));
        }
    }
    else
    {
        ESP_LOGE(TAG, "Invalid data format for updateRelay");
    }
}
// Hàm xử lý sự kiện createSchedule
void handle_create_schedule(cJSON *json)
{

    if (json == NULL)
    {
        ESP_LOGE("SERVER", "Invalid JSON");
        return;
    }

    task_info_t new_task;

    strcpy(new_task.id, cJSON_GetObjectItem(json, "id")->valuestring);
    strcpy(new_task.time_24h, cJSON_GetObjectItem(json, "time")->valuestring);
    strcpy(new_task.code, cJSON_GetObjectItem(json, "code")->valuestring);

    const char *id_value = cJSON_GetObjectItem(json, "id")->valuestring;
    const char *time_value = cJSON_GetObjectItem(json, "time")->valuestring;
    const char *code_value = cJSON_GetObjectItem(json, "code")->valuestring;

    delete_task(cJSON_GetObjectItem(json, "id")->valuestring);
    // Tạo một lịch trình mới
    create_task(new_task.id, "MY TASK", task_schedule, 2048, new_task, 1);

    // Tạo thông điệp JSON để gửi thông báo
    char register_message[256];
    snprintf(register_message, sizeof(register_message),
             "{\"event\":\"notify\",\"productId\":\"24082000\",\"data\":{\"event\":\"notifyCreateSchedule\",\"id\":\"%s\",\"time\":\"%s\",\"code\":\"%s\"}}",
             id_value, time_value, code_value); // Sử dụng các giá trị đã lấy trước đó

    ESP_LOGI(TAG, "Sending register message: %s", register_message);

    // Gửi thông điệp
    esp_err_t err = esp_websocket_client_send_text(client, register_message, strlen(register_message), portMAX_DELAY);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Register message sent successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send register message: %s", esp_err_to_name(err));
    }
}

void handle_delete_schedule(cJSON *json)
{

    if (json == NULL)
    {
        ESP_LOGE("SERVER", "Invalid JSON");
        return;
    }

    delete_task(cJSON_GetObjectItem(json, "id")->valuestring);

    // Tạo thông điệp JSON để gửi thông báo
    char register_message[256];
    snprintf(register_message, sizeof(register_message),
             "{\"event\":\"notify\",\"productId\":\"24082000\",\"data\":{\"event\":\"notifyDeleteSchedule\",\"id\":\"%s\"}}",
             cJSON_GetObjectItem(json, "id")->valuestring); // Sử dụng các giá trị đã lấy trước đó

    ESP_LOGI(TAG, "Sending register message: %s", register_message);

    // Gửi thông điệp
    esp_err_t err = esp_websocket_client_send_text(client, register_message, strlen(register_message), portMAX_DELAY);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "Register message sent successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to send register message: %s", esp_err_to_name(err));
    }
}

void reconnect_websocket(void *arg)
{
    while (true)
    {
        ESP_LOGI(TAG, "Trying to reconnect...");
        if (esp_websocket_client_start(client) == ESP_OK)
        {
            ESP_LOGI(TAG, "Reconnected to WebSocket");
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(5000)); // Đợi 5 giây trước khi thử lại
    }
    vTaskDelete(NULL); // Xóa task khi đã kết nối lại thành công
}

// Callback xử lý sự kiện WebSocket
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    client = (esp_websocket_client_handle_t)handler_args;

    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WebSocket connected");

        // Tạo thông điệp đăng ký
        const char *register_message = "{\"event\":\"register\",\"productId\":\"24082000\",\"events\":[\"updateRelay\",\"createSchedule\",\"deleteSchedule\",\"updateSchedule\"]}";
        ESP_LOGI(TAG, "Sending register message: %s", register_message);

        // Gửi thông điệp đăng ký
        esp_err_t err = esp_websocket_client_send_text(client, register_message, strlen(register_message), portMAX_DELAY);
        if (err == ESP_OK)
        {
            ESP_LOGI(TAG, "Register message sent successfully");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to send register message: %s", esp_err_to_name(err));
        }
        break;

    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WebSocket disconnected");
        xTaskCreate(&reconnect_websocket, "reconnect_task", 4096, NULL, 5, NULL);

        break;

    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "Data received: %.*s", data->data_len, (char *)data->data_ptr);

        // Parse JSON data
        cJSON *json = cJSON_Parse((char *)data->data_ptr);
        if (json != NULL && data->data_len > 0)
        {
            // Xử lý sự kiện updateRelay
            cJSON *event = cJSON_GetObjectItemCaseSensitive(json, "event");
            if (cJSON_IsString(event) && (event->valuestring != NULL))
            {
                if (strcmp(event->valuestring, "updateRelay") == 0)
                {
                    handle_update_relay(json);
                }
                else if (strcmp(event->valuestring, "createSchedule") == 0)
                {
                    handle_create_schedule(json);
                }
                else if (strcmp(event->valuestring, "deleteSchedule") == 0)
                {
                    handle_delete_schedule(json);
                }
                else if (strcmp(event->valuestring, "updateSchedule") == 0)
                {
                    handle_update_schedule(cJSON_GetObjectItem(json, "id")->valuestring);
                }
                else
                {
                    ESP_LOGW(TAG, "Unhandled event: %s", event->valuestring);
                }
            }
            else
            {
                ESP_LOGW(TAG, "Event field is missing or invalid");
            }

            cJSON_Delete(json);
        }

        break;

    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGE(TAG, "WebSocket error occurred");
        break;

    default:
        break;
    }
}

// Hàm khởi tạo và kết nối WebSocket
void websocket_app_start(void)
{
    esp_websocket_client_config_t websocket_cfg = {
        .uri = WEBSOCKET_URL, // Thay bằng URL WebSocket của bạn
    };

    // Khởi tạo WebSocket client
    esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
    esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);
    // Kết nối WebSocket
    esp_err_t err = esp_websocket_client_start(client);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "WebSocket client started successfully");
    }
    else
    {
        ESP_LOGE(TAG, "Failed to start WebSocket client: %s", esp_err_to_name(err));
    }
}
