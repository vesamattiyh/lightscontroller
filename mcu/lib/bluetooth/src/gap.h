#pragma once

#include "esp_random.h"

/* NimBLE GAP APIs */
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"

/* NimBLE stack APIs */
#include "host/util/util.h"

/* Defines */
#define DEVICE_NAME "LightsController"
#define BLE_GAP_APPEARANCE_GENERIC_TAG 0x0200
#define BLE_GAP_URI_PREFIX_HTTPS 0x17
#define BLE_GAP_LE_ROLE_PERIPHERAL 0x00

// GAP EVENT CODES
#define CONNECT_STATUS_FAILED 1
#define CONNECT_STATUS_SUCCESS 0

/* Public function declarations */
int gap_init(void);
void adv_init(void);
