#include "bleapi.h"
#include "esp_log.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "gap.h"

#include "store/config/ble_store_config.h"

static const char* TAG = "BLE_API";

static void nimble_host_config(void);

int ble_setup(void) {

    int rc = 0;
    esp_err_t ret;

    /* NimBLE host stack initialization */
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "failed to initialize nimble stack, error code: %d ",
                 ret);
        return 1;
    }

    rc = gap_setup();    // Initialize the GAP service

    if (rc != 0) {
        ESP_LOGE(TAG, "failed to setup GAP service, error code: %d ", rc);
        return rc;
    }

    nimble_host_config(); // Configure the host stack

    return rc;
}

static void nimble_host_config(void){

    /* Configure the host. */
    ble_hs_cfg.reset_cb = NULL;
    ble_hs_cfg.sync_cb = NULL;
    ble_hs_cfg.store_status_cb = NULL;

    /* Store host configuration */
    ble_store_config_init();

}