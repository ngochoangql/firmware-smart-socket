#include "task_manager.h"


static const char *TAGG = "TaskManager";
static task_info_t task_list[MAX_TASKS];
static int task_count = 0;

void initialize_task_manager()
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    save_tasks_to_nvs();
    load_tasks_from_nvs();
    start_loaded_tasks();
}

void create_task(const char *task_id, const char *task_name, void (*task_function)(void *), uint32_t stack_size_words, task_info_t task_info, UBaseType_t priority)
{
    print_current_time();

    BaseType_t xReturned;
    TaskHandle_t xHandle = NULL;
    task_info.handle = xHandle;
    xReturned = xTaskCreate(task_function, task_name, stack_size_words, &task_info, priority, &xHandle);
    if (xReturned == pdPASS)
    {
        // Add task to task_list
        task_list[task_count].handle = xHandle;
        strncpy(task_list[task_count].id, task_id, sizeof(task_list[task_count].id) - 1);
        strncpy(task_list[task_count].task_name, task_name, sizeof(task_list[task_count].task_name) - 1);
        strncpy(task_list[task_count].time_24h, task_info.time_24h, sizeof(task_list[task_count].time_24h) - 1);
        strncpy(task_list[task_count].code, task_info.code, sizeof(task_list[task_count].code) - 1);
        task_list[task_count].task_function = task_function; // Lưu con trỏ hàm thực thi

        task_count++;
        save_tasks_to_nvs();
        ESP_LOGI(TAGG, "Created task '%s'", task_id);
    }
    else
    {
        ESP_LOGE(TAGG, "Failed to create task '%s'", task_name);
    }
}

void delete_task(const char *task_id)
{
    printf(" %i \n", task_count);
    for (int i = 0; i < task_count; i++)
    {

        if (strncmp(task_list[i].id, task_id, sizeof(task_list[i].id)) == 0)
        {
            // Found the task, delete it

            eTaskState task_state;
            // Lấy trạng thái của task sử dụng eTaskGetState()
            task_state = eTaskGetState(task_list[i].handle);
            // Yêu cầu xóa task
            // Shift tasks in task_list to fill the gap
            for (int j = i; j < task_count - 1; j++)
            {
                task_list[j] = task_list[j + 1];
            }
            task_count--;

            // Save tasks to NVS after deleting a task
            save_tasks_to_nvs();

            ESP_LOGI(TAGG, "Deleted task '%s'", task_id);
            if (task_state == eRunning)
            {
                ESP_LOGI(TAGG, "Task '%s' đang chạy, yêu cầu xóa...", task_id);
            }
            else if (task_state == eReady)
            {
                ESP_LOGI(TAGG, "Task '%s' đã sẵn sàng, yêu cầu xóa...", task_id);
            }
            else if (task_state == eBlocked)
            {
                ESP_LOGI(TAGG, "Task '%s' bị chặn, yêu cầu xóa...", task_id);
            }

            else if (task_state == eSuspended)
            {
                ESP_LOGI(TAGG, "Task '%s' đã bị đình chỉ, không thể xóa", task_id);
            }

            else if (task_state == eDeleted)
            {
                ESP_LOGW(TAGG, "Task '%s' đã bị xóa trước đó, không thể xóa lại", task_id);
            }

            else if (task_state == eInvalid)
            {
                ESP_LOGW(TAGG, "Task '%s' không hợp lệ, không thể xóa", task_id);
            }
            else
            {
                ESP_LOGW(TAGG, "Trạng thái của task '%s' không xác định, không thể xóa", task_id);
            }

            // Kiểm tra trạng thái của task
            if (task_state != eDeleted && task_state != eReady)
            {
                vTaskDelete(task_list[i].handle);
            }

                        return;
        }
    }

    ESP_LOGW(TAGG, "Task '%s' not found for deletion", task_id);
}

void save_tasks_to_nvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAGG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    err = nvs_set_blob(nvs_handle, "task_list", task_list, task_count * sizeof(task_info_t));
    if (err != ESP_OK)
    {
        ESP_LOGE(TAGG, "Error (%s) writing task_list to NVS", esp_err_to_name(err));
    }

    err = nvs_commit(nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAGG, "Error (%s) committing NVS changes", esp_err_to_name(err));
    }

    nvs_close(nvs_handle);
}
void delete_all_tasks_to_nvs()
{
    for (int i = 0; i < task_count; i++)
    {

        // Found the task, delete it
        eTaskState task_state;

        // Lấy trạng thái của task sử dụng eTaskGetState()
        task_state = eTaskGetState(task_list[i].handle);
        switch (task_state)
        {
        case eRunning:
            ESP_LOGI(TAGG, "Task '%s' đang chạy, yêu cầu xóa...", task_list[i].id);
            break;
        case eReady:
            ESP_LOGI(TAGG, "Task '%s' đã sẵn sàng, yêu cầu xóa...", task_list[i].id);
            break;
        case eBlocked:
            ESP_LOGI(TAGG, "Task '%s' bị chặn, yêu cầu xóa...", task_list[i].id);
            break;
        case eSuspended:
            ESP_LOGI(TAGG, "Task '%s' đã bị đình chỉ, không thể xóa", task_list[i].id);
            break;
        case eDeleted:
            ESP_LOGW(TAGG, "Task '%s' đã bị xóa trước đó, không thể xóa lại", task_list[i].id);
            break;
        case eInvalid:
            ESP_LOGW(TAGG, "Task '%s' không hợp lệ, không thể xóa", task_list[i].id);
            break;
        default:
            ESP_LOGW(TAGG, "Trạng thái của task '%s' không xác định, không thể xóa", task_list[i].id);
            break;
        }
        // Kiểm tra trạng thái của task
        if (task_state != eDeleted && task_state != eReady)
        {
            vTaskDelete(task_list[i].handle);
        }
    }
    task_count = 0;
    // Save tasks to NVS after deleting a task
    save_tasks_to_nvs();

    ESP_LOGI(TAGG, "Deleted All task ");
}

void load_tasks_from_nvs()
{
    nvs_handle_t nvs_handle;
    esp_err_t err = nvs_open("nvs", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAGG, "Error (%s) opening NVS handle!", esp_err_to_name(err));
        return;
    }

    size_t required_size = 0;
    err = nvs_get_blob(nvs_handle, "task_list", NULL, &required_size);
    if (err == ESP_OK && required_size <= sizeof(task_list))
    {
        err = nvs_get_blob(nvs_handle, "task_list", task_list, &required_size);
        if (err == ESP_OK)
        {
            task_count = required_size / sizeof(task_info_t);
            ESP_LOGI(TAGG, "Loaded %d tasks from NVS", task_count);
        }
        else
        {
            ESP_LOGE(TAGG, "Error (%s) reading task_list from NVS", esp_err_to_name(err));
        }
    }
    else
    {
        ESP_LOGW(TAGG, "No task_list found in NVS");
    }

    nvs_close(nvs_handle);
}
void start_loaded_tasks()
{
    for (int i = 0; i < task_count; i++)
    {
        BaseType_t xReturned;
        TaskHandle_t xHandle = NULL;

        // Kiểm tra task_function có hợp lệ không
        if (task_list[i].task_function == NULL) {
            ESP_LOGE(TAGG, "task_function cho task '%s' là NULL", task_list[i].task_name);
            continue; // Bỏ qua task nếu hàm không hợp lệ
        }

        // Cấp phát bộ nhớ cho new_task
        task_info_t *new_task = (task_info_t *)malloc(sizeof(task_info_t));
        if (new_task == NULL)
        {
            ESP_LOGE(TAGG, "Lỗi khi cấp phát bộ nhớ cho new_task");
            continue; // Bỏ qua task nếu không thể cấp phát bộ nhớ
        }

        // Sao chép dữ liệu từ task_list[i] vào new_task
        strcpy(new_task->id, task_list[i].id);
        strcpy(new_task->time_24h, task_list[i].time_24h);
        strcpy(new_task->code, task_list[i].code);

        // Tạo task
        xReturned = xTaskCreate(
            task_list[i].task_function,   // Hàm thực thi task
            task_list[i].task_name,       // Tên task
            2048,                         // Kích thước stack
            (void *)new_task,             // Tham số truyền vào
            i,                            // Độ ưu tiên của task
            &xHandle                      // Trả về handle của task
        );

        // Kiểm tra kết quả tạo task
        if (xReturned == pdPASS)
        {
            task_list[i].handle = xHandle;
            ESP_LOGI(TAGG, "Đã khởi tạo và chạy task '%s' từ NVS", task_list[i].id);
        }
        else
        {
            ESP_LOGE(TAGG, "Lỗi khi khởi tạo task '%s' từ NVS", task_list[i].task_name);
            free(new_task); // Giải phóng bộ nhớ nếu không tạo được task
        }
    }
}


