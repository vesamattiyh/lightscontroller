/* Includes */
/* STD APIs */
#include <stdio.h>

/* ESP APIs */
#include "esp_log.h"
#include "nvs_flash.h"

/* FreeRTOS APIs */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* NimBLE stack APIs */
#include "bleapi.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

/* Defines */


void app_main() 
{
    const char *TAG = "main";
    esp_err_t ret;

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





}