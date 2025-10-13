#include "common.h"
#include "gap.h"
#include "gatt_svc.h"

static const char* TAG = "GAP";

/* Private function declarations */
inline static void format_addr(char *addr_str, uint8_t addr[]);
static void start_advertising(void);
static int gap_event_handler(struct ble_gap_event *event, void *arg);
void gatt_svr_subscribe_cb(struct ble_gap_event *event);

/* Private variables */
static uint8_t own_addr_type;
static uint8_t addr_val[6] = {0};
static uint8_t esp_uri[] = {BLE_GAP_URI_PREFIX_HTTPS, '/', '/', 'e', 's', 'p', 'r', 'e', 's', 's', 'i', 'f', '.', 'c', 'o', 'm'};

static void print_conn_desc(const struct ble_gap_conn_desc *desc) {
    char addr_str[18] = {0};
    char peer_addr_str[18] = {0};

    format_addr(addr_str, desc->our_id_addr.val);
    format_addr(peer_addr_str, desc->peer_id_addr.val);

    ESP_LOGI(TAG, "handle=%d our_addr_type=%d our_addr=%s "
                 "peer_addr_type=%d peer_addr=%s conn_itvl=%d "
                 "conn_latency=%d supervision_timeout=%d "
                 "encrypted=%d authenticated=%d bonded=%d",
             desc->conn_handle,
             desc->our_id_addr.type,
             addr_str,
             desc->peer_id_addr.type,
             peer_addr_str,
             desc->conn_itvl,
             desc->conn_latency,
             desc->supervision_timeout,
             desc->sec_state.encrypted,
             desc->sec_state.authenticated,
             desc->sec_state.bonded);
}

/* Private functions */
inline static void format_addr(char *addr_str, uint8_t addr[]) {
    sprintf(addr_str, "%02X:%02X:%02X:%02X:%02X:%02X", addr[0], addr[1],
            addr[2], addr[3], addr[4], addr[5]);
}

int gap_init(void)
{
    int rc = 0;

    ble_svc_gap_init();                             // Initialize the GAP service

    rc = ble_svc_gap_device_name_set(DEVICE_NAME); // Set the GAP device name

    if(rc != 0){
        ESP_LOGE(TAG, "Error setting the device name %s, error code: %d", 
                DEVICE_NAME, 
                rc);
        return rc;
    }

    rc = ble_svc_gap_device_appearance_set(BLE_GAP_APPEARANCE_GENERIC_TAG);

    if(rc != 0){
        ESP_LOGE(TAG, "Error setting the appearance, error code: %d", rc);
        return rc;
    }

    return rc;
}

void adv_init(void) {
    int rc = 0;
    
    char addr_str[18] = {0};


    /* Make sure we have proper BT identity address set */
    rc = ble_hs_util_ensure_addr(0);
    if (rc != 0) {
        ESP_LOGE(TAG, "device does not have any available bt address!");
        return;
    }

    /* Figure out BT address to use while advertising */
    rc = ble_hs_id_infer_auto(0, &own_addr_type);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to infer address type, error code: %d", rc);
        return;
    }

    /* Copy device address to addr_val */
    rc = ble_hs_id_copy_addr(own_addr_type, addr_val, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to copy device address, error code: %d", rc);
        return;
    }

    format_addr(addr_str, addr_val);
    ESP_LOGI(TAG, "device address: %s", addr_str);

    /* Start advertising. */
    start_advertising();
}

static void start_advertising(void) {
    int rc = 0;

    const char *name;
    
    struct ble_hs_adv_fields adv_fields = {0};
    struct ble_hs_adv_fields rsp_fields = {0};
    struct ble_gap_adv_params adv_params = {0};

    /* Set advertising flags  (general discoverable) and BLE Support only */
    adv_fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;

    /* Set device name */
    name = ble_svc_gap_device_name();
    adv_fields.name = (uint8_t *)name;
    adv_fields.name_len = strlen(name);
    adv_fields.name_is_complete = 1;

    /* Set device tx power */
    adv_fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    adv_fields.tx_pwr_lvl_is_present = 1;

    /* Set device appearance */
    adv_fields.appearance = BLE_GAP_APPEARANCE_GENERIC_TAG;
    adv_fields.appearance_is_present = 1;

    /* Set device LE role */
    adv_fields.le_role = BLE_GAP_LE_ROLE_PERIPHERAL;
    adv_fields.le_role_is_present = 1;

    /* Set advertiement fields */
    rc = ble_gap_adv_set_fields(&adv_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set advertising data, error code: %d", rc);
        return;
    }

    /* Set advertizing interval */
    rsp_fields.adv_itvl = BLE_GAP_ADV_ITVL_MS(500);
    rsp_fields.adv_itvl_is_present = 1;

    /* Set device address */
    rsp_fields.device_addr = addr_val;
    rsp_fields.device_addr_type = own_addr_type;
    rsp_fields.device_addr_is_present = 1;

    /* Set URI */
    rsp_fields.uri = esp_uri;
    rsp_fields.uri_len = sizeof(esp_uri);
    
    /* Set scan response fields */
    rc = ble_gap_adv_rsp_set_fields(&rsp_fields);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to set scan response data, error code: %d", rc);
        return;
    }

    /* Set connenection mode and general discoverable mode to be a beacon */
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;   // Undirected-connectable
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    /* Set advertising interval */
    adv_params.itvl_min = BLE_GAP_ADV_ITVL_MS(500);
    adv_params.itvl_max = BLE_GAP_ADV_ITVL_MS(510);

    /* Start advertising */
    rc = ble_gap_adv_start(own_addr_type, NULL, BLE_HS_FOREVER, &adv_params,
                           gap_event_handler, NULL);
    if (rc != 0) {
        ESP_LOGE(TAG, "failed to start advertising, error code: %d", rc);
        return;
    }
    ESP_LOGI(TAG, "advertising started!");
}

static int gap_event_handler(struct ble_gap_event *event, void *arg){
    int rc = 0;

    struct ble_gap_conn_desc desc;

    switch (event->type) {

    case BLE_GAP_EVENT_CONNECT:
        /* A new connection was established or a connection attempt failed */
        ESP_LOGI(TAG, "connection %s; status=%d",
                event->connect.status == 0 ? "established" : "failed",
                event->connect.status);

        if (event->connect.status == CONNECT_STATUS_SUCCESS){
            rc = ble_gap_conn_find(event->connect.conn_handle, &desc);

            if (rc != 0) {
                ESP_LOGE(TAG,
                        "failed to find connection by handle, error code: %d",
                        rc);
                return rc;
            }

        /* Print connection descriptor and turn on the LED */
        print_conn_desc(&desc);

        /* Try to update connection parameters */
        struct ble_gap_upd_params params = {.itvl_min = desc.conn_itvl,
                                            .itvl_max = desc.conn_itvl,
                                            .latency = 3,
                                            .supervision_timeout =
                                                desc.supervision_timeout};
        rc = ble_gap_update_params(event->connect.conn_handle, &params);
        if (rc != 0) {
            ESP_LOGE(TAG,
                    "failed to update connection parameters, error code: %d",
                    rc);
            return rc;
        }
    }
    else {
        /* Connection failed; resume advertising */
        start_advertising();
    }
    return rc;


    case BLE_GAP_EVENT_DISCONNECT:
        /* Connection terminated; resume advertising */
        ESP_LOGI(TAG, "disconnected from peer; reason=%d", event->disconnect.reason);
        start_advertising();
        return rc;

    case BLE_GAP_EVENT_CONN_UPDATE:
        /* The central has updated the connection parameters. */
        ESP_LOGI(TAG, "connection updated; status=%d", event->conn_update.status);

        /* Print connection descriptor */
        rc = ble_gap_conn_find(event->conn_update.conn_handle, &desc);
        if (rc != 0) {
            ESP_LOGE(TAG, "failed to find connection by handle, error code: %d",
                     rc);
            return rc;
        }
        print_conn_desc(&desc);
        return rc;

    /* Advertising complete event */
    case BLE_GAP_EVENT_ADV_COMPLETE:
        /* Advertising completed, restart advertising */
        ESP_LOGI(TAG, "advertise complete; reason=%d",
                 event->adv_complete.reason);
        start_advertising();
        return rc;

    case BLE_GAP_EVENT_SUBSCRIBE:
        /* A peer has subscribed or unsubscribed to notifications or
         * indications on a characteristic.
         */
        ESP_LOGI(TAG, "subscribe event; conn_handle=%d attr_handle=%d "
                     "reason=%d prev_notify=%d cur_notify=%d "
                     "prev_indicate=%d cur_indicate=%d",
                 event->subscribe.conn_handle,
                 event->subscribe.attr_handle,
                 event->subscribe.reason,
                 event->subscribe.prev_notify,
                 event->subscribe.cur_notify,
                 event->subscribe.prev_indicate,
                 event->subscribe.cur_indicate);
        
        /* GATT subscrribe event */
        gatt_svr_subscribe_cb(event);
        return rc;


    /* Notification sent event */
    case BLE_GAP_EVENT_NOTIFY_TX:
        if ((event->notify_tx.status != 0) &&
            (event->notify_tx.status != BLE_HS_EDONE)) {
            /* Print notification info on error */
            ESP_LOGI(TAG,
                     "notify event; conn_handle=%d attr_handle=%d "
                     "status=%d is_indication=%d",
                     event->notify_tx.conn_handle, event->notify_tx.attr_handle,
                     event->notify_tx.status, event->notify_tx.indication);
        }
        return rc;

    /* MTU update event */
    case BLE_GAP_EVENT_MTU:
        /* Print MTU update info to log */
        ESP_LOGI(TAG, "mtu update event; conn_handle=%d cid=%d mtu=%d",
                 event->mtu.conn_handle, event->mtu.channel_id,
                 event->mtu.value);
        return rc;
    }
        
    return rc;
}

