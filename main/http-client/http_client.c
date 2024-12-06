#include "http_client.h"

static const char *TAG = "HTTP_POST";

// Hàm xử lý sự kiện phản hồi HTTP
esp_err_t http_event_handler(esp_http_client_event_t *evt) {
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s", evt->data_len, (char*)evt->data);
            }
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
    }
    return ESP_OK;
}

// Hàm gửi dữ liệu thiết bị qua HTTP POST
void send_device_data(device_t device) {
    // Cấu hình URL và xử lý sự kiện HTTP
    esp_http_client_config_t config = {
        .url = "http://192.168.1.3:8080/api/v1/devices",
        .event_handler = http_event_handler,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Tạo JSON từ struct device_t
    char post_data[256];
    snprintf(post_data, sizeof(post_data),
             "{\"productId\":\"%s\", \"deviceMqtt\":\"%s\", \"deviceHttp\":\"%s\", \"numOfRelay\":%d}",
             device.productId, device.deviceMqtt, get_ip_address_as_string(), device.numOfRelay);

    // Thiết lập yêu cầu POST với dữ liệu JSON
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Gửi yêu cầu HTTP POST
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // Dọn dẹp
    esp_http_client_cleanup(client);
}

void post_device_to_api(device_t device) {
    // Cấu hình HTTP client
    esp_http_client_config_t config = {
        .url = "http://192.168.1.3:8080/api/v1/devices",
        .event_handler = NULL, // Bạn có thể định nghĩa handler nếu cần
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    // Chuẩn bị dữ liệu JSON
    char post_data[512];
    snprintf(post_data, sizeof(post_data),
             "{\"productId\":\"%s\",\"deviceMqtt\":\"%s\",\"deviceHttp\":\"%s\",\"numOfRelay\":3,\"name\":\"Smart Socket\",\"type\":\"Plug\",\"status\":false}",
             device.productId, device.deviceMqtt, get_ip_address_as_string());

    // Cấu hình yêu cầu POST
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    // Gửi yêu cầu HTTP POST
    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    // Dọn dẹp HTTP client
    esp_http_client_cleanup(client);
}