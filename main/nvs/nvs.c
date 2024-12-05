#include "nvs.h"

void nvs_init()
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_LOGE("NVS", "NO SPACE !");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();

    }
    ESP_ERROR_CHECK(ret);
}
void write_nvs(const char *key, int32_t value)
{
    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else
    {
        err = nvs_set_i32(my_handle, key, value);
        if (err != ESP_OK)
        {
            ESP_LOGE("NVS", "Error (%s) setting value!", esp_err_to_name(err));
        }
        err = nvs_commit(my_handle);
        if (err != ESP_OK)
        {
            ESP_LOGE("NVS", "Error (%s) committing value!", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
}
int32_t read_nvs(const char *key)
{
    nvs_handle_t my_handle;
    int32_t value = 0; // Giá trị mặc định
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE("NVS", "Error (%s) opening NVS handle!", esp_err_to_name(err));
    }
    else
    {
        err = nvs_get_i32(my_handle, key, &value);
        if (err != ESP_OK)
        {
            ESP_LOGE("NVS", "Error (%s) reading value!", esp_err_to_name(err));
        }
        nvs_close(my_handle);
    }
    return value;
}

void save_string_to_nvs(const char *key, const char *value)
{
    nvs_handle_t my_handle;
    esp_err_t err;

    // Mở NVS
    err = nvs_open("nvs", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        // Ghi chuỗi vào NVS
        err = nvs_set_str(my_handle, key, value);
        if (err != ESP_OK)
        {
            printf("Error (%s) setting value!\n", esp_err_to_name(err));
        }
        else
        {
            // Commit thay đổi
            err = nvs_commit(my_handle);
            if (err != ESP_OK)
            {
                printf("Error (%s) committing value!\n", esp_err_to_name(err));
            }
        }

        // Đóng NVS
        nvs_close(my_handle);
    }
}
void read_string_from_nvs(const char *key, char *buffer, size_t buffer_size)
{
    nvs_handle_t my_handle;
    esp_err_t err;
    // Mở NVS
    err = nvs_open("nvs", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        // Đọc chuỗi từ NVS
        size_t required_size;
        err = nvs_get_str(my_handle, key, NULL, &required_size);
        if (err == ESP_OK)
        {
            if (required_size <= buffer_size)
            {
                err = nvs_get_str(my_handle, key, buffer, &required_size);
                if (err == ESP_OK)
                {
                    printf("String read successfully: %s\n", buffer);
                }
                else
                {
                    printf("Error (%s) reading string!\n", esp_err_to_name(err));
                    buffer[0] = '\0'; // Đặt buffer thành chuỗi rỗng
                }
            }
            else
            {
                printf("Buffer size is too small! Required size: %d\n", required_size);
                buffer[0] = '\0'; // Đặt buffer thành chuỗi rỗng
            }
        }
        else if (err == ESP_ERR_NVS_NOT_FOUND)
        {
            printf("The value is not initialized yet!\n");
            buffer[0] = '\0'; // Đặt buffer thành chuỗi rỗng
        }
        else
        {
            printf("Error (%s) reading!\n", esp_err_to_name(err));
            buffer[0] = '\0'; // Đặt buffer thành chuỗi rỗng
        }

        // Đóng NVS
        nvs_close(my_handle);
    }
}
