#include "web_server.h"

#include "esp_log.h"
#include "string.h"
#include <cJSON.h>

#include "device/device.h"

#define HTTP_SERVER_TAG "WEB_SERVER"

esp_err_t get_info(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    // Thu thập thông tin của ESP32
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    // Dung lượng bộ nhớ tự do
    int free_heap = esp_get_free_heap_size();

    // Thời gian hoạt động
    int uptime = esp_timer_get_time() / 1000000;

    // Chuẩn bị JSON response
    char response[200];
    snprintf(response, sizeof(response),
             "{\"product_id\":\"%s\",\"device_name\":\"M5 Atom\",\"chip_model\":\"%s\",\"cores\":%d,\"revision\":%d,\"free_heap\":%d,\"uptime\":%d}",
             device.productId,
             CONFIG_IDF_TARGET,
             chip_info.cores,
             chip_info.revision,
             free_heap,
             uptime);

    // Gửi response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

esp_err_t get_device(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");

    // Chuẩn bị JSON response
    char response[200];
    snprintf(response, sizeof(response),
             "{\"product_id\":\"%s\",\"ipAddress\":\"%s\",\"numOfRelay\":3,\"name\":\"Smart Socket\",\"type\":\"Plug\",\"status\":false}",
             device.productId, get_ip_address_as_string());

    // Gửi response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}
esp_err_t disconnect_device(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    save_string_to_nvs("WIFI_MODE", "SMART_CONFIG");
    smart_config_start();
    // Chuẩn bị JSON response
    char response[200];
    snprintf(response, sizeof(response),
             "{\"code\":2000,\"message\":\"successfully\"}");

    // Gửi response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

esp_err_t get_relays(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    // Thu thập thông tin của ESP32

    // Chuẩn bị JSON response
    char response[200];
    snprintf(response, sizeof(response),
             "[{\"id\":0,\"status\":%i},{\"id\":1,\"status\":%i},{\"id\":2,\"status\":%i}]",
             device.relayModules[0].status,
             device.relayModules[1].status,
             device.relayModules[2].status);

    // Gửi response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, response, strlen(response));
    return ESP_OK;
}

esp_err_t schedule_add_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    int remaining = req->content_len;
    char buf[remaining + 1];
    int ret;
    ESP_LOGI(HTTP_SERVER_TAG, "%i   ,  %i", remaining, sizeof(buf));

    if (remaining >= sizeof(buf))
    {
        ESP_LOGE("SERVER", "Content too long");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }

    // Read the content
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0)
    {
        ESP_LOGE("SERVER", "Failed to receive post content");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive post content");
        return ESP_FAIL;
    }
    buf[ret] = '\0';
    printf(buf);
    // Parse JSON
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL)
    {
        ESP_LOGE("SERVER", "Invalid JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    task_info_t new_task;

    strcpy(new_task.id, cJSON_GetObjectItem(json, "id")->valuestring);
    strcpy(new_task.time_24h, cJSON_GetObjectItem(json, "time_24h")->valuestring);
    strcpy(new_task.code, cJSON_GetObjectItem(json, "code")->valuestring);

    create_task(new_task.id, "MY TASK", task_schedule, 2048, new_task, 1);

    cJSON_Delete(json);

    char *param_value = "{\"message\":\"Schedule Added Successfully\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, param_value, strlen(param_value));
    return ESP_OK;
}
esp_err_t schedule_delete_id_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    char *id = NULL;
    char buf[64];

    // Lấy giá trị của id từ URI
    if (httpd_req_get_url_query_str(req, buf, sizeof(buf)) == ESP_OK)
    {
        if (httpd_query_key_value(buf, "id", buf, sizeof(buf)) == ESP_OK)
        {
            id = buf;
        }
    }

    if (id == NULL)
    {
        const char *resp_err = "Missing id parameter";
        httpd_resp_send(req, resp_err, strlen(resp_err));
        return ESP_FAIL;
    }
    delete_task(id);
    // Gửi phản hồi thành công
    char *param_value = "{\"message\":\"Schedule Deleted Successfully\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, param_value, strlen(param_value));
    return ESP_OK;
}
esp_err_t schedule_update_handler(httpd_req_t *req)
{
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    int remaining = req->content_len;
    char buf[remaining + 1];
    int ret;
    ESP_LOGI(HTTP_SERVER_TAG, "%i   ,  %i", remaining, sizeof(buf));

    if (remaining >= sizeof(buf))
    {
        ESP_LOGE("SERVER", "Content too long");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }

    // Read the content
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0)
    {
        ESP_LOGE("SERVER", "Failed to receive post content");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive post content");
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL)
    {
        ESP_LOGE("SERVER", "Invalid JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    task_info_t new_task;

    strcpy(new_task.id, cJSON_GetObjectItem(json, "id")->valuestring);
    strcpy(new_task.time_24h, cJSON_GetObjectItem(json, "time_24h")->valuestring);
    strcpy(new_task.code, cJSON_GetObjectItem(json, "code")->valuestring);

    // create_task(new_schedule.id, "MY TASK", task_schedule, 2048, &new_schedule, 1);
    delete_task(new_task.id);
    create_task(new_task.id, "MY TASK", task_schedule, 2048, new_task, 1);

    cJSON_Delete(json);

    // Send response
    httpd_resp_sendstr(req, "Schedule added successfully");

    return ESP_OK;
}

esp_err_t device_update_handler(httpd_req_t *req)
{
    // Allow all origins for CORS
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    int remaining = req->content_len;
    char buf[remaining + 1]; // Buffer to hold the incoming data
    int ret;
    ESP_LOGI(HTTP_SERVER_TAG, "Remaining data: %i, Buffer size: %i", remaining, sizeof(buf));

    if (remaining > sizeof(buf) - 1)
    {
        ESP_LOGE("SERVER", "Content too long");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Content too long");
        return ESP_FAIL;
    }

    // Read the content from the HTTP request
    ret = httpd_req_recv(req, buf, remaining);
    if (ret <= 0)
    {
        ESP_LOGE("SERVER", "Failed to receive post content");
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to receive post content");
        return ESP_FAIL;
    }
    buf[ret] = '\0'; // Null-terminate the buffer

    // Parse the JSON content
    cJSON *json = cJSON_Parse(buf);
    if (json == NULL)
    {
        ESP_LOGE("SERVER", "Invalid JSON");
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
        return ESP_FAIL;
    }

    // Extract deviceServer from the JSON
    cJSON *device_server_item = cJSON_GetObjectItem(json, "deviceServer");
    if (device_server_item == NULL || !cJSON_IsString(device_server_item))
    {
        ESP_LOGE("SERVER", "Invalid or missing 'deviceServer' in JSON");
        cJSON_Delete(json);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "'deviceServer' missing or invalid");
        return ESP_FAIL;
    }

    // Copy the deviceServer value into a buffer
    char buffer[128];
    strncpy(buffer, device_server_item->valuestring, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0'; // Ensure null-termination

    // Save the deviceServer to NVS
    save_string_to_nvs("deviceServer", buffer);

    // Clean up JSON object
    cJSON_Delete(json);

    // Send a successful response
    httpd_resp_sendstr(req, "Device server updated successfully");

    return ESP_OK;
}

// Configuration SERVER
httpd_handle_t start_http_server(void)
{
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    config.max_uri_handlers = 100; // Đặt lại giới hạn URI nếu cần
    config.max_resp_headers = 8;   // Số lượng header phản hồi tối đa
    if (httpd_start(&server, &config) == ESP_OK)
    {
        // Countdown URI
        httpd_uri_t get_relays_uri = {
            .uri = "/relays",
            .method = HTTP_GET,
            .handler = get_relays,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &get_relays_uri);
        // Countdown URI
        httpd_uri_t get_info_uri = {
            .uri = "/info",
            .method = HTTP_GET,
            .handler = get_info,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &get_info_uri);
        httpd_uri_t get_device_uri = {
            .uri = "/device",
            .method = HTTP_GET,
            .handler = get_device,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &get_device_uri);
        httpd_uri_t disconnect_device_uri = {
            .uri = "/disconnect",
            .method = HTTP_GET,
            .handler = disconnect_device,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &disconnect_device_uri);

        httpd_uri_t post_schedule_uri = {
            .uri = "/schedule",
            .method = HTTP_POST,
            .handler = schedule_add_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &post_schedule_uri);
        httpd_uri_t delete_schedule_uri = {
            .uri = "/schedule/delete",
            .method = HTTP_GET,
            .handler = schedule_delete_id_handler,
            .user_ctx = NULL};
        httpd_register_uri_handler(server, &delete_schedule_uri);
        httpd_uri_t update_device_server_uri = {
            .uri = "/device/update",          // Địa chỉ API
            .method = HTTP_POST,              // Phương thức POST
            .handler = device_update_handler, // Handler xử lý yêu cầu
            .user_ctx = NULL                  // Không có thông tin người dùng bổ sung
        };
        httpd_register_uri_handler(server, &update_device_server_uri);
    }
    return server;
}

// Start HTTP_Server
void web_server_init()
{
    // Bắt đầu HTTP server
    httpd_handle_t server = start_http_server();
    if (server == NULL)
    {
        ESP_LOGE(HTTP_SERVER_TAG, "Failed to start HTTP server");
    }
}

// Stop HTTP_Server
void http_server(httpd_handle_t server)
{
    if (server)
    {
        httpd_stop(server);
    }
}
