#include "bleapi.h"
#include "common.h"

#include "gap.h"
#include "gatt_svc.h"

#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

static const char* TAG = "BLE_API";

/* Global function declarations */
void ble_store_config_init(void);

/* Static declarations */
static void nimble_host_config(void);
static void nimble_host_task(void *param);

static void on_stack_reset(int reason);
static void on_stack_sync(void);

/* functions declarations */
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

    rc = gap_init();    // Initialize the GAP service

    if (rc != 0) {
        ESP_LOGE(TAG, "failed to setup GAP service, error code: %d ", rc);
        return rc;
    }

    /* GATT server initialization */
    rc = gatt_svc_init();
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to initialize GATT server, error code: %d", rc);
        return rc;
    }

    nimble_host_config(); // Configure the host stack

    xTaskCreate(nimble_host_task, "NimBLE Host", 4*1024, NULL, 5, NULL);

    ESP_LOGI(TAG, "BLE-API Started");

    return rc;
}

static void nimble_host_config(void){

    /* Configure the host. */
    ble_hs_cfg.sm_io_cap = BLE_HS_IO_DISPLAY_ONLY; /* Security related configuration */
    ble_hs_cfg.sm_bonding = 1; /* Security related configuration */
    ble_hs_cfg.sm_mitm = 1; /* Security related configuration */
    ble_hs_cfg.sm_our_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID; /* Security related configuration */
    ble_hs_cfg.sm_their_key_dist |= BLE_SM_PAIR_KEY_DIST_ENC | BLE_SM_PAIR_KEY_DIST_ID; /* Security related configuration */
    ble_hs_cfg.reset_cb = on_stack_reset;
    ble_hs_cfg.sync_cb = on_stack_sync;
    ble_hs_cfg.gatts_register_cb = gatt_svr_register_cb;
    ble_hs_cfg.store_status_cb = ble_store_util_status_rr;

    /* Store host configuration */
    ble_store_config_init();
}

static void nimble_host_task(void *param) {
    /* Task entry log */
    ESP_LOGI(TAG, "nimble host task has been started!");

    /* This function won't return until nimble_port_stop() is executed */
    nimble_port_run();

    /* Clean up at exit */
    vTaskDelete(NULL);
}

static void on_stack_sync(void) {
    /* On stack sync, do advertising initialization */
    adv_init();
}

static void on_stack_reset(int reason) {
    /* On reset, print reset reason to console */
    ESP_LOGI(TAG, "nimble stack reset, reset reason: %d", reason);
}