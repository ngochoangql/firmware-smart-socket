#include "config.h"

void config_init()
{
    save_string_to_nvs("wifi", "4070");
    save_string_to_nvs("password", "00000000");
    save_string_to_nvs("WIFI_MODE", "SMART_CONFIG");
    save_string_to_nvs("mqttBroker","mqtt.ohstem.vn");
    save_string_to_nvs("httpServer","192.168.1.2");
}