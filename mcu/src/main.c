/* Includes */
/* FreeRTOS APIs */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "nvs_flash.h"
#include "esp_log.h"
#include "bleapi.h"
#include "common.h"

/* STD APIs */

/* Defines */

void app_main() 
{
    const char *TAG = "MAIN";
    esp_err_t ret = ESP_OK;

    // initialize the flash - depencency for ble stack
    ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nvs flash, error code: %d ", ret);
        return;
    }

    if(ble_setup() != ESP_OK){
        ESP_LOGE(TAG, "Failed to start the BLE-API");
        return;
    }

    for(;;){
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP_LOGI(TAG, "Running...");
    };

    return;
}

