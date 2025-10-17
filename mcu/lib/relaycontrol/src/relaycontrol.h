#pragma once

/* Standard C Libraries */
#include <stdint.h>
#include <stdbool.h>

// ESP-Related APIs
#include "driver/gpio.h"
#include "esp_log.h"

#define MAX_RELAYS 6

typedef struct relay_t {
    char name[24];
    gpio_num_t pin; 
} relay_t;

void relay_setup(void);
uint16_t relay_get_status(void);
bool relay_get_state(int relay_num);
bool relay_set_state(int relay_num, bool state);