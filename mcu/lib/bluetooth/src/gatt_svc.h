#pragma once

/* Includes */
/* NimBLE GATT APIs */
#include "host/ble_gatt.h"
#include "services/gatt/ble_svc_gatt.h"

/* NimBLE GAP APIs */
#include "host/ble_gap.h"

/* Custom APIs */
#include "relaycontrol.h"

/**
 * @brief Macro for initializing a user read descriptor.
 *
 * This macro initializes a user read descriptor with the provided value.
 *
 * @param name    Text to be shown in descriptor.
 *
 * @return        The initialized user descriptor structure.
 */
#define USER_READ_DESCRIPTOR(name, cb)      \
    {                                       \
    .uuid = BLE_UUID16_DECLARE(0x2901),     \
    .att_flags = BLE_ATT_F_READ,            \
    .access_cb = (cb),                      \
    .arg = (name),                          \
    }

/* Public function declarations */
int gatt_svc_init(void);
void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg);
void gatt_svr_subscribe_cb(struct ble_gap_event *event);
