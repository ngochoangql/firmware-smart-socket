#include "button_task.h"

#define BUTTON_DEVICE 26

button_t button = {
    .buttonGpio = BUTTON_DEVICE,
    .buttonState = BUTTON_IDLE};

static void button_init(button_t *button)
{
    gpio_pad_select_gpio(button->buttonGpio);
    gpio_set_direction(button->buttonGpio, GPIO_MODE_INPUT);
    gpio_set_pull_mode(button->buttonGpio, GPIO_PULLUP_ONLY); // Nút nhấn có điện trở kéo lên
}

void handle_button_press()
{
    printf("Nút nhấn đã được nhấn!\n");
    if (!device.relayModules[0].status || !device.relayModules[1].status || !device.relayModules[2].status)
    {
        turnOnRelay(&device.relayModules[0]);
        turnOnRelay(&device.relayModules[1]);
        turnOnRelay(&device.relayModules[2]);
        handle_update_relay_from_device(1, 1);
        handle_update_relay_from_device(2, 1);
        handle_update_relay_from_device(3, 1);
    }
    else
    {

        turnOffRelay(&device.relayModules[0]);
        turnOffRelay(&device.relayModules[1]);
        turnOffRelay(&device.relayModules[2]);
        handle_update_relay_from_device(1, 0);
        handle_update_relay_from_device(2, 0);
        handle_update_relay_from_device(3, 0);
    }
}

// Hàm xử lý sự kiện khi nút nhấn được giữ
static void handle_button_hold()
{
    printf("Nút nhấn đang được giữ sang SmartConfig!\n");
    save_string_to_nvs("WIFI_MODE", "SMART_CONFIG");
    smart_config_start();
}
static void handle_button_hold_config()
{
    printf("Nút nhấn đang được giữ sang Config !\n");
    config_init();
}
void buttonTask(void *pbParameter)
{

    button_init(&button);
    TickType_t last_wake_time = xTaskGetTickCount();
    const TickType_t delay_interval = 50 / portTICK_PERIOD_MS; // Chờ 50ms

    TickType_t press_start_time = 0;                                    // Thời điểm bắt đầu nhấn nút
    TickType_t hold_threshold_smart_config = 2000 / portTICK_PERIOD_MS; // Ngưỡng thời gian giữ nút (1000ms = 1 giây)
    TickType_t hold_threshold_config = 4000 / portTICK_PERIOD_MS;
    while (1)
    {
        // Đọc trạng thái của nút nhấn
        int button_level = gpio_get_level(BUTTON_DEVICE);
        // printf(button_level);
        switch (button.buttonState)
        {
        case BUTTON_IDLE:
            if (button_level == 0)
            {
                button.buttonState = BUTTON_PRESSED;
                press_start_time = xTaskGetTickCount();
            }
            break;

        case BUTTON_PRESSED:
            if (button_level == 1)
            {

                TickType_t current_time = xTaskGetTickCount();
                if ((current_time - press_start_time) >= hold_threshold_config)
                {
                    button.buttonState = BUTTON_HOLD;
                    handle_button_hold_config();
                }
                else if ((current_time - press_start_time) >= hold_threshold_smart_config)
                {
                    button.buttonState = BUTTON_HOLD;
                    handle_button_hold();
                }
                else
                {
                    button.buttonState = BUTTON_IDLE;
                    handle_button_press();
                }
            }
            break;

        case BUTTON_HOLD:
            if (button_level == 1)
            {
                button.buttonState = BUTTON_IDLE;
            }
            break;

        default:
            break;
        }

        vTaskDelayUntil(&last_wake_time, delay_interval); // Chờ 50ms trước khi đọc lại
    }
}