#include "mqtt.h"

static const char *MQTT = "MQTT_EXAMPLE";

static esp_mqtt_client_handle_t mqtt_client = NULL;
static bool mqtt_status = false;

static void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;
    esp_mqtt_client_handle_t client = event->client;

    switch (event->event_id)
    {
    case MQTT_EVENT_CONNECTED:
        mqtt_status = true;
        ESP_LOGI(MQTT, "MQTT_EVENT_CONNECTED");
        esp_mqtt_client_subscribe(client, "smart_plug/24082000/R0/get", 0);
        esp_mqtt_client_subscribe(client, "smart_plug/24082000/R1/get", 0);
        esp_mqtt_client_subscribe(client, "smart_plug/24082000/R2/get", 0);
        esp_mqtt_client_subscribe(client, "smart_plug/feeds/schedule/delete", 0);
        esp_mqtt_client_subscribe(client, "smart_plug/24082000/led", 0);
        break;
    case MQTT_EVENT_DISCONNECTED:
        mqtt_status = false;
        ESP_LOGI(MQTT, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(MQTT, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(MQTT, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        // ESP_LOGI(MQTT, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(MQTT, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;
    case MQTT_EVENT_ERROR:
        ESP_LOGI(MQTT, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(MQTT, "Other event id:%d", event->event_id);
        break;
    }
}
void publish_message(const char *topic, const char *message)
{
    if (mqtt_client != NULL && mqtt_status)
    {
        esp_mqtt_client_publish(mqtt_client, topic, message, 0, 1, 0);
    }
    else
    {
        ESP_LOGE(MQTT, "MQTT client is not initialized");
    }
}
bool get_status_mqtt(){
    return mqtt_status;
}
void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .uri = device.deviceMqtt, // Thay bằng địa chỉ MQTT broker của bạn
    };

    mqtt_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, mqtt_client);
    esp_mqtt_client_start(mqtt_client);
}
