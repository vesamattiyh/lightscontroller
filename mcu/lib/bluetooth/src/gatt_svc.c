#include "common.h"
#include "gatt_svc.h"

static const char* TAG = "GATT_SVC";

/* Automation IO service */
static const ble_uuid16_t auto_io_svc_uuid = BLE_UUID16_INIT(0x1815);

/* Relay characteristics */

static const ble_uuid128_t relay1_chr_uuid =
    BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
                     0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF1);

// static const ble_uuid128_t relay2_chr_uuid =
//     BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
//                      0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF2);

// static const ble_uuid128_t relay3_chr_uuid =
//     BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
//                      0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF3);

// static const ble_uuid128_t relay4_chr_uuid =
//     BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
//                      0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF4);

// static const ble_uuid128_t relay5_chr_uuid =
//     BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
//                      0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF5);

// static const ble_uuid128_t relay6_chr_uuid =
//     BLE_UUID128_INIT(0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78,
//                      0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF6);
struct ble_gatt_svc_def gatt_svr_svcs[2];

static uint16_t relay_val_handle[6]; // Handles for each relay characteristic

static int relay_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg) {
    uint8_t relay_num = (uint8_t)(uintptr_t)arg;
    uint8_t state;

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        // Extract 1-byte payload (0 = off, 1 = on)
        state = ctxt->om->om_data[0];
        ESP_LOGI(TAG, "Relay %d set to %s\n", relay_num, state ? "ON" : "OFF");

        // Control the relay hardware
        relay_set_state(relay_num, state);

        return 0; // success
    }

    return BLE_ATT_ERR_UNLIKELY;
}

/* GATT Services table */

static int relay_descriptor_access(uint16_t conn_handle, uint16_t attr_handle,
                                    struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    const char *desc = (const char *)arg;

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_DSC) {
        os_mbuf_append(ctxt->om, desc, strlen(desc));
        return 0; // success
    }
    return BLE_ATT_ERR_UNLIKELY;
}

void gatt_init_services(void){
    // struct ble_gatt_svc_def gatt_svr_svcs[2];

    struct ble_gatt_svc_def auto_io;
    auto_io.type = BLE_GATT_SVC_TYPE_PRIMARY;
    auto_io.uuid = &auto_io_svc_uuid.u;

    struct ble_gatt_chr_def relay_chr_defs[MAX_RELAYS+1];

    for(int i=0; i<MAX_RELAYS; i++){
        relay_chr_defs[i] = 
        (struct ble_gatt_chr_def){
            .uuid = &relay1_chr_uuid.u,
            .access_cb = relay_access,
            .flags = BLE_GATT_CHR_F_WRITE,
            .val_handle = &relay_val_handle[i],
            .arg = (void *)(i+1),
            .descriptors = (struct ble_gatt_dsc_def[]) {
                USER_READ_DESCRIPTOR(relay_get_name(i), relay_descriptor_access),
                {0}} // Terminator
        };
    }
    relay_chr_defs[MAX_RELAYS] = (struct ble_gatt_chr_def){0}; // Terminator for defs

    auto_io.characteristics = relay_chr_defs;

    gatt_svr_svcs[0] = auto_io;
    gatt_svr_svcs[1] = (struct ble_gatt_svc_def) {0};
};

// struct ble_gatt_svc_def gatt_svr_svcs[] = {
//     /* Automation IO service */
//     {
//         .type = BLE_GATT_SVC_TYPE_PRIMARY,
//         .uuid = &auto_io_svc_uuid.u,
//         .characteristics =
//             (struct ble_gatt_chr_def[]) {
//             { .uuid = &relay1_chr_uuid.u,
//               .access_cb = relay_access,
//               .flags = BLE_GATT_CHR_F_WRITE,
//               .val_handle = &relay_val_handle[0],
//               .arg = (void *)1,
//               .descriptors = (struct ble_gatt_dsc_def[]) {
//                 USER_READ_DESCRIPTOR("Relay 1", relay_descriptor_access),
//                 {0}} // Terminator
//             },
//             { .uuid = &relay2_chr_uuid.u,
//               .access_cb = relay_access,
//               .flags = BLE_GATT_CHR_F_WRITE,
//               .val_handle = &relay_val_handle[1],
//               .arg = (void *)2,
//               .descriptors = (struct ble_gatt_dsc_def[]) {
//                 USER_READ_DESCRIPTOR("Relay 2", relay_descriptor_access), 
//                 {0}} // Terminator
//             },
//             { .uuid = &relay3_chr_uuid.u,
//               .access_cb = relay_access,
//               .flags = BLE_GATT_CHR_F_WRITE,
//               .val_handle = &relay_val_handle[2],
//               .arg = (void *)3, 
//               .descriptors = (struct ble_gatt_dsc_def[]) {
//                 USER_READ_DESCRIPTOR("Relay 3", relay_descriptor_access), 
//                 {0}} // Terminator
//             },
//             { .uuid = &relay4_chr_uuid.u,
//               .access_cb = relay_access,
//               .flags = BLE_GATT_CHR_F_WRITE,
//               .val_handle = &relay_val_handle[3],
//               .arg = (void *)4, 
//               .descriptors = (struct ble_gatt_dsc_def[]) {
//                 USER_READ_DESCRIPTOR("Relay 4", relay_descriptor_access), 
//                 {0}} // Terminator
//             },
//             { .uuid = &relay5_chr_uuid.u,
//               .access_cb = relay_access,
//               .flags = BLE_GATT_CHR_F_WRITE,
//               .val_handle = &relay_val_handle[4],
//               .arg = (void *)5, 
//               .descriptors = (struct ble_gatt_dsc_def[]) {
//                 USER_READ_DESCRIPTOR("Relay 5", relay_descriptor_access), 
//                 {0}} // Terminator
//             },
//             { .uuid = &relay6_chr_uuid.u,
//               .access_cb = relay_access,
//               .flags = BLE_GATT_CHR_F_WRITE,
//               .val_handle = &relay_val_handle[5],
//               .arg = (void *)6,
//               .descriptors = (struct ble_gatt_dsc_def[]) {
//                 USER_READ_DESCRIPTOR("Relay 6", relay_descriptor_access), 
//                 {0}} // Terminator
//             },
//             { 0 } // terminator
//             }, // End of characteristics array
//     }, // End of service
    
//     {
//         0, /* Termination, No more services. */
//     },
// };

int gatt_svc_init(void){
    int rc = 0;
    
    /* GATT service initialization */
    ble_svc_gatt_init();

    gatt_init_services();

    /* Update GATT services counter */
    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to count GATT services, error code: %d", rc);
        return rc;
    }

    /* Add GATT services */
    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to add GATT services error code: %d", rc);
        return rc;
    }

    return rc;

}

void gatt_svr_register_cb(struct ble_gatt_register_ctxt *ctxt, void *arg) {
    /* Local variables */
    char buf[BLE_UUID_STR_LEN];
    ESP_LOGI(TAG, "GATT service registered");
    /* Handle GATT attributes register events */
    switch (ctxt->op) {

    /* Service register event */
    case BLE_GATT_REGISTER_OP_SVC:
        ESP_LOGD(TAG, "registered service %s with handle=%d",
                 ble_uuid_to_str(ctxt->svc.svc_def->uuid, buf),
                 ctxt->svc.handle);
        break;

    /* Characteristic register event */
    case BLE_GATT_REGISTER_OP_CHR:
        ESP_LOGD(TAG,
                 "registering characteristic %s with "
                 "def_handle=%d val_handle=%d",
                 ble_uuid_to_str(ctxt->chr.chr_def->uuid, buf),
                 ctxt->chr.def_handle, ctxt->chr.val_handle);
        break;

    /* Descriptor register event */
    case BLE_GATT_REGISTER_OP_DSC:
        ESP_LOGD(TAG, "registering descriptor %s with handle=%d",
                 ble_uuid_to_str(ctxt->dsc.dsc_def->uuid, buf),
                 ctxt->dsc.handle);
        break;

    /* Unknown event */
    default:
        assert(0);
        break;
    }
}

void gatt_svr_subscribe_cb(struct ble_gap_event *event) {
    /* Check connection handle */
    if (event->subscribe.conn_handle != BLE_HS_CONN_HANDLE_NONE) {
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d",
                 event->subscribe.conn_handle, event->subscribe.attr_handle);
    } else {
        ESP_LOGI(TAG, "subscribe by nimble stack; attr_handle=%d",
                 event->subscribe.attr_handle);
    }

}