#include "ac_measure_task.h"

UNIT_ACMEASURE unit_acmeasure;
int data_index = 0;

#define SAMPLES_PER_SECOND 20
#define SAMPLE_INTERVAL_MS (1000 / SAMPLES_PER_SECOND)

#define I2C_ACMEASURE_SCL_IO 21     // Chân SCL
#define I2C_ACMEASURE_SDA_IO 25     // Chân SDA
#define I2C_ACMEASURE_NUM I2C_NUM_1 // Số I2C

sensor_data_t data_buffer[SAMPLES_PER_SECOND];
void acmeasure_task(void *pvParameter)
{

    int ready_count = 0;
    int count = 0;
    while (1)
    {

        if (UNIT_ACMEASURE_getReady(&unit_acmeasure) == 1)
        {
            ready_count++;

            uint16_t voltage = UNIT_ACMEASURE_getVoltage(&unit_acmeasure);
            uint16_t current = UNIT_ACMEASURE_getCurrent(&unit_acmeasure);
            uint32_t active_power = UNIT_ACMEASURE_getPower(&unit_acmeasure);
            uint32_t apparent_power = UNIT_ACMEASURE_getApparentPower(&unit_acmeasure);

            printf("Voltage: %d , Current: %d , Active Power: %d , Apparent Power: %d .\n", voltage, current, active_power, apparent_power);
            data_buffer[data_index].current = current;
            data_buffer[data_index].voltage = voltage * 10;
            data_buffer[data_index].power = active_power;
            data_buffer[data_index].apparentPower = apparent_power;
            data_index++;

            // Nếu đã thu thập đủ 20 mẫu, gửi dữ liệu qua HTTP POST
            if (data_index >= SAMPLES_PER_SECOND)
            {
                // Tạo JSON data từ buffer
                char *post_data = malloc(5000 * sizeof(char));
                if (post_data == NULL)
                {
                    // Xử lý khi cấp phát thất bại
                    perror("Không thể cấp phát bộ nhớ động cho post_data");
                    exit(EXIT_FAILURE);
                }
                char *temp_data = malloc(1000 * sizeof(char));
                if (temp_data == NULL)
                {
                    // Xử lý khi cấp phát thất bại
                    perror("Không thể cấp phát bộ nhớ động cho temp_data");
                    exit(EXIT_FAILURE);
                }

                strcpy(post_data, "[");

                for (int i = 0; i < SAMPLES_PER_SECOND; i++)
                {
                    snprintf(temp_data, 1000,
                             "{\"current\":%d,\"voltage\":%d,\"power\":%d,\"apparentPower\":%.d}",
                             data_buffer[i].current, data_buffer[i].voltage,
                             data_buffer[i].power, data_buffer[i].apparentPower);
                    strcat(post_data, temp_data);
                    if (i < SAMPLES_PER_SECOND - 1)
                    {
                        strcat(post_data, ",");
                    }
                }
                strcat(post_data, "]");

                // Gửi dữ liệu qua HTTP POST
                esp_http_client_config_t config = {
                    .url = "http://172.20.10.3:8080/api/v1/data",
                    .event_handler = http_event_handler,
                };
                esp_http_client_handle_t client = esp_http_client_init(&config);

                esp_http_client_set_method(client, HTTP_METHOD_POST);
                esp_http_client_set_header(client, "Content-Type", "application/json");
                esp_http_client_set_post_field(client, post_data, strlen(post_data));

                esp_err_t err = esp_http_client_perform(client);

                if (err == ESP_OK)
                {
                    ESP_LOGI("TAG", "HTTP POST Status = %d, content_length = %d",
                             esp_http_client_get_status_code(client),
                             esp_http_client_get_content_length(client));
                }
                else
                {
                    ESP_LOGE("TAG", "HTTP POST request failed: %s", esp_err_to_name(err));
                }

                esp_http_client_cleanup(client);
                free(post_data);
                free(temp_data);
                // Reset buffer
                data_index = 0;
            }
        }
        count++;
        if (count == 10)
        {
            printf("Ready: %i \n", ready_count);
            count = 0;
            ready_count = 0;
        }
        vTaskDelay(50); // Đợi 50ms trước khi đo lại
    }
}

void acmeasure_init()
{
    if (UNIT_ACMEASURE_begin(&unit_acmeasure, I2C_ACMEASURE_NUM, I2C_ACMEASURE_SDA_IO, I2C_ACMEASURE_SCL_IO, 100000))
    {
        printf("I2C initialized successfully\n");

        uint16_t voltage = UNIT_ACMEASURE_getVoltage(&unit_acmeasure);
        printf("Voltage: %d\n", voltage);
        xTaskCreate(&acmeasure_task, "acmeasure_task", 4096, NULL, 1, NULL);

        // Thêm các hàm khác tương tự như vậy
    }
    else
    {
        printf("Failed to initialize I2C\n");
    }
}