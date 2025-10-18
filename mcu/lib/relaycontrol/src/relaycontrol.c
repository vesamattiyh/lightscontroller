#include "relaycontrol.h"

static const char *TAG = "RELAYCONTROL";
static uint16_t relay_statusword = 0x0000; // Each bit represents the state of a relay (0 = off, 1 = on)

static void relay_update_statusword(void);

relay_t relay_table[MAX_RELAYS] = 
{
    {.name="Reverse Light", .pin=GPIO_NUM_9},
    {.name="Right Side Light", .pin=GPIO_NUM_10},
    {.name="Left Side Light", .pin=GPIO_NUM_11},
    {.name="RELAY 4", .pin=GPIO_NUM_18},
    {.name="RELAY 4", .pin=GPIO_NUM_19},
    {.name="RELAY 4", .pin=GPIO_NUM_20},
};

void relay_setup(void){
    esp_err_t ret = ESP_OK;
    const uint64_t mask = ((1ULL<<relay_table[0].pin) | 
                            (1ULL<<relay_table[1].pin) | 
                            (1ULL<<relay_table[2].pin) | 
                            (1ULL<<relay_table[3].pin) | 
                            (1ULL<<relay_table[4].pin) | 
                            (1ULL<<relay_table[5].pin)) ;

    gpio_config_t relay_conf = {
    .pin_bit_mask = mask,
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_DISABLE,
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type = GPIO_INTR_DISABLE,
    };

    ret = gpio_config(&relay_conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure GPIOs for relays: %s", esp_err_to_name(ret));
        return;
    }

    for(int i = 0; i < MAX_RELAYS; i++){ // Initialize all relays to off state
        gpio_set_level(relay_table[i].pin, 0);
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

    return ((relay_statusword & (1 << relay_num)) > 0);

}

bool relay_set_state(int relay_num, bool state){
    esp_err_t ret = ESP_OK;
    
    if(relay_num < 0 || relay_num >= MAX_RELAYS){
        ESP_LOGE(TAG, "Invalid set relay state argument: %d", relay_num);
        return false; // Invalid relay number
    }

    ret = gpio_set_level(relay_table[relay_num].pin, state);

    if(ret != ESP_OK){
        ESP_LOGE(TAG, "Failed to set relay %d state to %d: %s", relay_num, state, esp_err_to_name(ret));
        return false;
    }

    relay_update_statusword();

    return true; 
}

relay_t relay_get_info(int num){
    return relay_table[num];
}

char* relay_get_name(int num){
    return relay_table[num].name;
}

static void relay_update_statusword(void){

    for(int i = 0; i < MAX_RELAYS; i++){     // Set each bit in the status word according to the actual GPIO level
        gpio_get_level(relay_table[i].pin) == 1 ? (relay_statusword |= (1 << i)) : \
                                             (relay_statusword &= ~(1 << i));
    }


}
