#include "relaycontrol.h"
#include "driver/gpio.h"

#define RELAY_GPIO_MASK ((1ULL << GPIO_NUM_12) | (1ULL << GPIO_NUM_13) | (1ULL << GPIO_NUM_14) | \
                        (1ULL << GPIO_NUM_15) | (1ULL << GPIO_NUM_16) | (1ULL << GPIO_NUM_17))
                       
static const char *TAG = "RELAYCONTROL";
static uint16_t relay_statusword = 0x0000; // Each bit represents the state of a relay (0 = off, 1 = on)

static uint8_t relay_pins[MAX_RELAYS] = {
    GPIO_NUM_12,
    GPIO_NUM_13,
    GPIO_NUM_14,
    GPIO_NUM_15,
    GPIO_NUM_16,
    GPIO_NUM_17
};

static gpio_config_t relay_conf = {
    .pin_bit_mask = RELAY_GPIO_MASK,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
};

void relay_setup(void){
    esp_err_t ret = ESP_OK;
    
    ret = gpio_config(&relay_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIOs for relays: %s", esp_err_to_name(ret));
        return;
    }

    for(int i = 0; i < MAX_RELAYS; i++){ // Initialize all relays to off state
        gpio_set_level(relay_pins[i], 0);
    }
    
    relay_update_statusword();
}

uint16_t relay_get_status(void){
    return relay_statusword;
}

bool relay_get_state(int relay_num){
    
    if(relay_num < 0 || relay_num >= MAX_RELAYS){
        ESP_LOGE(TAG, "Invalid get relay state argument: %d", relay_num);
        return false; // Invalid relay number
    }

    return (relay_statusword & (1 << relay_num) > 0);

}

bool relay_set_state(int relay_num, bool state){
    esp_err_t ret = ESP_OK;
    
    if(relay_num < 0 || relay_num >= MAX_RELAYS){
        ESP_LOGE(TAG, "Invalid set relay state argument: %d", relay_num);
        return false; // Invalid relay number
    }

    ret = gpio_set_level(relay_pins[relay_num], state);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "Failed to set relay %d state to %d: %s", relay_num, state, esp_err_to_name(ret));
        return false;
    }

    relay_update_statusword();

    return true; 
}

static void relay_update_statusword(void){

    for(int i = 0; i < MAX_RELAYS; i++){     // Set each bit in the status word according to the actual GPIO level
        gpio_get_level(relay_pins[i]) == 1 ? relay_statusword |= (1 << i) : \
                                             relay_statusword &= ~(1 << i);
    }

}
