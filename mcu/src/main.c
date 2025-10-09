#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "bleapi.h"

void app_main() 
{
    const char *TAG = "example";

    for(;;){    
        ESP_LOGI(TAG, "Hello, from ESP32!\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    };
}